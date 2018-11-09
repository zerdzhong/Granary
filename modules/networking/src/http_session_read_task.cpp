#include <utility>

//
// Created by zhongzhendong on 2018/8/11.
//

#include "http_session_read_task.hpp"
#include "http_session_task.hpp"
#include <curl/curl.h>
#include <sstream>
#include <assert.h>
#include <http_session_read_task.hpp>

#include "http_session_config.hpp"
#include "http_curl_wrapper.hpp"

#define kDefaultRetryCount 3

class HttpCurlAdapter : public HttpCurlCallback {
public:
    explicit HttpCurlAdapter(HttpSessionReadTask *read_task) :
    read_task_{read_task}
    {
        curl_wrapper_ = std::make_unique<HttpCurlWrapper>(this);
    }

    size_t OnCurlWrite(char *data, size_t size) override {
        read_task_->OnData(data, size, kReadDataTypeBody);
        return size;
    }
    size_t OnCurlHeader(char *data, size_t size) override {
        read_task_->OnData(data, size, kReadDataTypeHeader);
        return size;
    }
    int OnCurlProgress(long long dltotal, long long dlnow) override {
        double progress = 0;

        if (dltotal > 0) {
            progress = dlnow / dltotal;
        }
        read_task_->OnProgress(progress);

        return 0;
    }

    CURL * handle() {
        return curl_wrapper_->curl_handle();
    }

private:
    HttpSessionReadTask *read_task_;
    std::unique_ptr<HttpCurlWrapper> curl_wrapper_;
};

#pragma mark- Life cycle

HttpSessionReadTask::HttpSessionReadTask(std::string url, size_t offset, size_t length) :
url_(std::move(url)),
retry_count_(kDefaultRetryCount),
stopped_(false),
request_count_(0),
session_config_(nullptr)
{
    request_start_ = offset;
    request_size_ = length;

    std::ostringstream stringStream;
    if (0 != request_size_) {
        stringStream << request_start_ << "-" <<request_start_ + request_size_ - 1;
    } else {
        stringStream << request_start_ << "-";
    }
    range_str_ = stringStream.str();

    head_data_ = NewSessionTaskData(nullptr, 0, 0, kReadDataTypeHeader);
    body_data_ = NewSessionTaskData(nullptr, 0, 0, kReadDataTypeBody);

    curl_adapter = std::make_unique<HttpCurlAdapter>(this);
    setupHandle();
}

HttpSessionReadTask::~HttpSessionReadTask() {

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

    result_code_ = SyncRead(retry_count_);

    ReadConnectionFinished(result_code_);

    return result_code_;
}

void HttpSessionReadTask::Cancel() {
    stopped_ = true;
}

bool HttpSessionReadTask::ShouldRetry(int curl_code) {
    return  CONN_OK != curl_code &&
            CURLE_ABORTED_BY_CALLBACK != curl_code &&
            request_count_ < retry_count_;
}

void HttpSessionReadTask::RetryTask() {
    request_count_ ++;
    received_size_ = 0;
}

void HttpSessionReadTask::ReadConnectionFinished(int finish_code) {
    HttpConnectionCode response_code = parseErrorReason(finish_code);

    //callback data finish
    if (nullptr != listener_) {
        listener_->OnDataFinish(this, response_code);
    }
}

#pragma mark- Private

HttpConnectionCode HttpSessionReadTask::SyncRead(uint8_t retry_count) {
    if (retry_count == 0) {
        return CONN_DEFAULT;
    }

    CURL *easy_handle = curl_adapter->handle();

    int curl_code = curl_easy_perform(easy_handle);
    request_count_ ++;

    effective_url_ = parseEffectiveUrl();
    HttpConnectionCode response_code = parseErrorReason(curl_code);

    if (CONN_OK != response_code) {
        HttpConnectionCode res = SyncRead(--retry_count);
        response_code =  (CONN_DEFAULT == res) ? response_code : res;
    }

    return response_code;
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
                result = CONN_UNKNOW_ERROR;
                break;
        }

        return result;
    }

    long protocol;
    curl_easy_getinfo(curl_adapter->handle(), CURLINFO_PROTOCOL, &protocol);

    if (CURLPROTO_HTTP != protocol &&  CURLPROTO_HTTPS != protocol) {
        result = CONN_UNSUPPORT_PROTOCOL;
    }

    long response_code = 0;
    curl_easy_getinfo(curl_adapter->handle(), CURLINFO_RESPONSE_CODE, &response_code);

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
    CURL *easy_handle = curl_adapter->handle();

    //basic setting
    curl_easy_setopt(easy_handle, CURLOPT_URL, url_.c_str());
    curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(easy_handle, CURLOPT_PRIVATE, this);

    //Allow redirect
    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_AUTOREFERER, 1L);
    curl_easy_setopt(easy_handle, CURLOPT_MAXREDIRS, 30L);

    //time out 1 byte/s 10s
    curl_easy_setopt(easy_handle, CURLOPT_LOW_SPEED_LIMIT, 1);
    curl_easy_setopt(easy_handle, CURLOPT_LOW_SPEED_TIME, 10);

    curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, 3L);

    curl_easy_setopt(easy_handle, CURLOPT_RANGE, range_str_.c_str());
}

std::string HttpSessionReadTask::parseEffectiveUrl() {
    assert(curl_adapter);

    char *p = nullptr;
    curl_easy_getinfo(curl_adapter->handle(), CURLINFO_EFFECTIVE_URL, &p);
    return p;
}

#pragma mark- Getters & Setters

std::string HttpSessionReadTask::url() {
    return url_;
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

void HttpSessionReadTask::setSessionConfig(HttpSessionConfig *session_config) {
    session_config_ = session_config;
}

HttpSessionConfig* HttpSessionReadTask::sessionConfig() {
    return session_config_;
}

uint8_t HttpSessionReadTask::request_count() {
    return request_count_;
}

CURL *HttpSessionReadTask::handle() {
    if (curl_adapter) {
        return curl_adapter->handle();
    }
    return nullptr;
}

void HttpSessionReadTask::OnData(char *data, size_t size, ReadDataType type) {
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
}

void HttpSessionReadTask::OnProgress(double progress) {

}
