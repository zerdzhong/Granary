#include <utility>

//
// Created by zhongzhendong on 2018/8/11.
//

#include "http_read_connection.hpp"
#include "base_connection.hpp"
#include "curl.h"
#include <sstream>

#pragma mark- CURL callback

static size_t header_callback(char *buffer, size_t size,
                              size_t nitems, void *userdata)
{
    auto * readTask = (HttpReadConnection *) userdata;
    return readTask->ReceiveHeader(buffer, size, nitems);
}

int xfer_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                  curl_off_t ultotal, curl_off_t ulnow)
{
    auto * readTask = (HttpReadConnection *) clientp;
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
    auto * readTask = (HttpReadConnection *) userdata;
    return readTask->ReceiveData(data, size, nmemb);
}

#pragma mark- Life cycle

HttpReadConnection::HttpReadConnection(std::string url, size_t offset, size_t length) :
url_(std::move(url)), handle_(nullptr)
{
    request_start_ = offset;
    request_size_ = length;

    if (0 != request_size_) {
        std::ostringstream stringStream;
        stringStream << request_start_ << "-" <<request_start_ + request_size_ - 1;
        range_str_ = stringStream.str();
    }

    head_data_ = NewConnectionReadData(nullptr, 0, 0, kReadDataTypeHeader);
    body_data_ = NewConnectionReadData(nullptr, 0, 0, kReadDataTypeBody);

    setupHandle();
}

HttpReadConnection::~HttpReadConnection() {
    cleanupHandle();
}

#pragma mark- Public

void HttpReadConnection::SyncRead() {
    if (nullptr == handle_) {
        setupHandle();
    }

    int res = curl_easy_perform(handle_);

    ReadConnectionFinished(res);
}

void HttpReadConnection::ReadConnectionFinished(int code) {

    if (nullptr != listener_) {
        listener_->OnDataFinish(this, code);
    }

    if (nullptr != handle_) {
        curl_easy_cleanup(handle_);
        handle_ = nullptr;
    }

}

#pragma mark- Internal CURL Callback

size_t HttpReadConnection::ReceiveData(char *data, size_t size, size_t nmemb) {
    size_t real_size = size * nmemb;
    return receiveData(data, real_size, kReadDataTypeBody);
}

size_t HttpReadConnection::ReceiveHeader(char *data, size_t size, size_t nmemb) {
    size_t real_size = size * nmemb;
    return receiveData(data, real_size, kReadDataTypeHeader);
}

int HttpReadConnection::ReceiveProgress(long long dltotal, long long dlnow) {
    return 0;
}

#pragma mark- Private

size_t HttpReadConnection::receiveData(char *data, size_t size, int type) {

    ConnectionReadData *read_data = nullptr;

    if (kReadDataTypeHeader == type) {
        read_data = head_data_;
        read_data->offset += read_data->size;
    } else if (kReadDataTypeBody == type) {
        read_data = body_data_;
        read_data->offset = request_start_ + received_size_;
        received_size_ += size;
    }

    read_data->data = data;
    read_data->size = size;

    if (nullptr != listener_) {
        listener_->OnData(this, read_data);
    }

    return size;
}

void HttpReadConnection::setupHandle() {
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

    if (0 != request_size_) {
        curl_easy_setopt(easy_handle, CURLOPT_RANGE, range_str_.c_str());
    }

    handle_ = easy_handle;
}

void HttpReadConnection::cleanupHandle() {
    if (nullptr == handle_) {
        return;
    }

    curl_easy_cleanup(handle_);
    handle_ = nullptr;
}

#pragma mark- Getters

std::string HttpReadConnection::url() {
    return url_;
}

CURL* HttpReadConnection::handle() {
    return handle_;
}

