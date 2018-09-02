#include <utility>

//
// Created by zhongzhendong on 2018/8/11.
//

#include "http_session_read_task.hpp"
#include "http_session_task.hpp"
#include "curl.h"
#include <sstream>

#pragma mark- CURL callback

static size_t header_callback(char *buffer, size_t size,
                              size_t nitems, void *userdata)
{
    auto * readTask = (HttpSessionReadTask *) userdata;
    return readTask->ReceiveHeader(buffer, size, nitems);
}

int xfer_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                  curl_off_t , curl_off_t )
{
    auto * readTask = (HttpSessionReadTask *) clientp;
    return readTask->ReceiveProgress(dltotal, dlnow);
}

int progress_callback(void *clientp,   double dltotal,   double dlnow,   double ultotal,   double ulnow)
{
    return xfer_callback(clientp,
                         (curl_off_t)dltotal,
                         (curl_off_t)dlnow,
                         (curl_off_t)ultotal,
                         (curl_off_t)ulnow);
}

size_t write_callback(char *data, size_t size, size_t nmemb, void *userdata)
{
    auto * readTask = (HttpSessionReadTask *) userdata;
    return readTask->ReceiveData(data, size, nmemb);
}

#define kDefaultRetryCount 3

#pragma mark- Life cycle

HttpSessionReadTask::HttpSessionReadTask(std::string url, size_t offset, size_t length) :
url_(std::move(url)),
handle_(nullptr),
retry_count_(kDefaultRetryCount),
stopped_(false),
request_count_(0)
{
    request_start_ = offset;
    request_size_ = length;

    if (0 != request_size_) {
        std::ostringstream stringStream;
        stringStream << request_start_ << "-" <<request_start_ + request_size_ - 1;
        range_str_ = stringStream.str();
    } else {
        range_str_ = "0-";
    }


    head_data_ = NewSessionTaskData(nullptr, 0, 0, kReadDataTypeHeader);
    body_data_ = NewSessionTaskData(nullptr, 0, 0, kReadDataTypeBody);

    setupHandle();
}

HttpSessionReadTask::~HttpSessionReadTask() {
    printf("\n~HttpSessionReadTask:%p\n",this);
    cleanupHandle();

    if (head_data_) {
        delete head_data_;
        head_data_ = nullptr;
    }

    if (body_data_) {
        delete body_data_;
        body_data_ = nullptr;
    }
}

#pragma mark- Public

HttpConnectionCode HttpSessionReadTask::SyncRead() {
    if (nullptr == handle_) {
        setupHandle();
    }

    assert(handle_);

    result_code_ = SyncRead(retry_count_);

    ReadConnectionFinished(result_code_);

    return result_code_;
}

void HttpSessionReadTask::Cancel() {
    stopped_ = true;
}

void HttpSessionReadTask::ReadConnectionFinished(int finish_code) {
    HttpConnectionCode response_code = parseErrorReason(finish_code);

    //callback data finish
    if (nullptr != listener_) {
        listener_->OnDataFinish(this, response_code);
    }
}

#pragma mark- Internal CURL Callback

size_t HttpSessionReadTask::ReceiveData(char *data, size_t size, size_t nmemb) {
    size_t real_size = size * nmemb;
    return receiveData(data, real_size, kReadDataTypeBody);
}

size_t HttpSessionReadTask::ReceiveHeader(char *data, size_t size, size_t nmemb) {
    size_t real_size = size * nmemb;
    return receiveData(data, real_size, kReadDataTypeHeader);
}

int HttpSessionReadTask::ReceiveProgress(long long dltotal, long long dlnow) {
    return 0;
}

#pragma mark- Private

HttpConnectionCode HttpSessionReadTask::SyncRead(uint8_t retry_count) {
    if (retry_count == 0) {
        return CONN_DEFAULT;
    }

    int curl_code = curl_easy_perform(handle_);
    request_count_ ++;

    effective_url_ = parseEffectiveUrl();
    HttpConnectionCode response_code = parseErrorReason(curl_code);

    if (CONN_OK != response_code) {
        HttpConnectionCode res = SyncRead(--retry_count);
        response_code =  (CONN_DEFAULT == res) ? response_code : res;
    }

    return response_code;
}

size_t HttpSessionReadTask::receiveData(char *data, size_t size, int type) {

    HttpSessionTaskData *read_data = nullptr;

    if (kReadDataTypeHeader == type) {
        read_data = head_data_;
        read_data->offset += read_data->size;
    } else if (kReadDataTypeBody == type) {
        read_data = body_data_;
        read_data->offset = request_start_ + received_size_;
        received_size_ += size;
    }

    assert(read_data);

    read_data->data = data;
    read_data->size = size;

    if (nullptr != listener_ && !stopped_) {
        listener_->OnData(this, read_data);
    } else {
        //add a default implement write to file/console?
    }

    return size;
}

HttpConnectionCode HttpSessionReadTask::parseErrorReason(int code) {

    HttpConnectionCode result = CONN_OK;

    if (CURLE_OK != code) {
        switch(code) {
            case CURLE_URL_MALFORMAT:
                result = CONN_INVALID_URL;
                break;
            case CURLE_COULDNT_CONNECT:
                result = CONN_TCP_CONN_FAILED;
                break;
            case CURLE_OPERATION_TIMEDOUT:
                result = CONN_TIMEOUT;
                break;
            case CURLE_TOO_MANY_REDIRECTS:
                result = HTTP_OVER_REDIRECT;
                break;
            case CURLE_ABORTED_BY_CALLBACK:
                result = CONN_USER_CANCEL;
                break;
            default:
                result = CONN_OK;
                break;
        }

        return result;
    }

    long protocol;
    curl_easy_getinfo(handle_, CURLINFO_PROTOCOL, &protocol);

    if (CURLPROTO_HTTP != protocol &&  CURLPROTO_HTTPS != protocol) {
        result = CONN_UNSUPPORT_PROTOCOL;
    }

    long response_code = 0;
    curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, &response_code);

    switch(response_code) {
        case 400:
            result = HTTP_BAD_REQUEST;
            break;
        case 401:
            result = HTTP_UNAUTHORIZED;
            break;
        case 403:
            result = HTTP_FORBIDDEN;
            break;
        case 404:
            result = HTTP_NOT_FOUND;
            break;
        case 408:
            result = HTTP_TIMEOUT;
            break;
        case 416:
            result = HTTP_RANGE_ERROR;
            break;
        case 502:
            result = HTTP_SERVER_502;
            break;
        case 503:
            result = HTTP_SERVER_503;
            break;
        case 504:
            result = HTTP_SERVER_504;
            break;
        default: {
            if (response_code >= 400 && response_code < 500) {
                result = HTTP_OTHER_4XX;
                break;
            }
            if (response_code >= 500 && response_code < 600) {
                result = HTTP_SERVER_ERROR;
                break;
            }
            break;
        }
    }

    return result;
}

void HttpSessionReadTask::setupHandle() {
    if (nullptr != handle_) {
        return;
    }

    CURL *easy_handle = curl_easy_init();

    //basic setting
    curl_easy_setopt(easy_handle, CURLOPT_URL, url_.c_str());
    curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(easy_handle, CURLOPT_PRIVATE, this);

    //header setting
    curl_easy_setopt(easy_handle, CURLOPT_HEADER, 0L);
    curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(easy_handle, CURLOPT_HEADERDATA, this);

    //body data setting
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, this);

    //Allow redirect
    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_AUTOREFERER, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_MAXREDIRS, 30L);

    //SSL setting
#ifdef WITHOUT_SSL
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

    //progress setting
    curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, 0L);
#if LIBCURL_VERSION_NUM >= 0x072000
    curl_easy_setopt(easy_handle, CURLOPT_XFERINFOFUNCTION, xfer_callback);
    curl_easy_setopt(easy_handle, CURLOPT_XFERINFODATA, this);
#else
    curl_easy_setopt(easy_handle, CURLOPT_PROGRESSFUNCTION, progress_callback);
    curl_easy_setopt(easy_handle, CURLOPT_PROGRESSDATA, this);
#endif

    //time out 1 byte/s 10s
    curl_easy_setopt(easy_handle, CURLOPT_LOW_SPEED_LIMIT, 1);
    curl_easy_setopt(easy_handle, CURLOPT_LOW_SPEED_TIME, 10);

    curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, 20L);

    curl_easy_setopt(easy_handle, CURLOPT_RANGE, range_str_.c_str());

    handle_ = easy_handle;
}

void HttpSessionReadTask::cleanupHandle() {
    if (nullptr == handle_) {
        return;
    }

    curl_easy_cleanup(handle_);
    handle_ = nullptr;
}

std::string HttpSessionReadTask::parseEffectiveUrl() {
    assert(handle_);

    char *p = nullptr;
    curl_easy_getinfo(handle_, CURLINFO_EFFECTIVE_URL, &p);
    return p;
}

#pragma mark- Getters & Setters

std::string HttpSessionReadTask::url() {
    return url_;
}

CURL* HttpSessionReadTask::handle() {
    return handle_;
}

uint8_t HttpSessionReadTask::retry_count() {
    return retry_count_;
}

void HttpSessionReadTask::set_retry_count(uint8_t retry_count) {
    retry_count_ = retry_count;
}

bool HttpSessionReadTask::isStopped() {
    return stopped_;
}
