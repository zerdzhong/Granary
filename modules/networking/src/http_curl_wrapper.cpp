//
// Created by zhongzhendong on 2018/11/6.
//

#include "http_curl_wrapper.hpp"
#include <curl/curl.h>
#include <assert.h>

#pragma mark- CURL callback

static size_t header_callback(char *buffer, size_t size,
                              size_t nitems, void *userdata)
{
    auto * readTask = (HttpCurlWrapper *) userdata;
    return readTask->ReceiveHeader(buffer, size, nitems);
}

int xfer_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                  curl_off_t , curl_off_t )
{
    auto * readTask = (HttpCurlWrapper *) clientp;
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
    auto * readTask = (HttpCurlWrapper *) userdata;
    return readTask->ReceiveData(data, size, nmemb);
}

#pragma mark- Internal CURL Callback

size_t HttpCurlWrapper::ReceiveData(char *data, size_t size, size_t nmemb) {
    size_t real_size = size * nmemb;
    if (callback_) {
        return callback_->OnCurlWrite(data, real_size);
    }
    return real_size;
}

size_t HttpCurlWrapper::ReceiveHeader(char *data, size_t size, size_t nmemb) {
    size_t real_size = size * nmemb;
    if (callback_) {
        return callback_->OnCurlHeader(data, real_size);
    }
    return real_size;
}

int HttpCurlWrapper::ReceiveProgress(long long dltotal, long long dlnow) {
    if (callback_) {
        return callback_->OnCurlProgress(dltotal, dlnow);
    }
    return 0;
}

HttpCurlWrapper::HttpCurlWrapper(HttpCurlCallback *callback)
: callback_(callback)
{
    DoInit();
}

HttpCurlWrapper::~HttpCurlWrapper() {
    if (handle_) {
        curl_easy_cleanup(handle_);
        handle_ = nullptr;
    }
}

void HttpCurlWrapper::DoInit() {

    handle_ = curl_easy_init();

    setCurlOpt(CURLOPT_HEADER, 0L);

    //header setting
    setCurlOpt(CURLOPT_HEADER, 0L);
    setCurlOpt(CURLOPT_HEADERFUNCTION, header_callback);
    setCurlOpt(CURLOPT_HEADERDATA, this);

    //body data setting
    setCurlOpt(CURLOPT_WRITEFUNCTION, write_callback);
    setCurlOpt(CURLOPT_WRITEDATA, this);

    //progress setting
    setCurlOpt(CURLOPT_NOPROGRESS, 0L);
#if LIBCURL_VERSION_NUM >= 0x072000
    setCurlOpt(CURLOPT_XFERINFOFUNCTION, xfer_callback);
    setCurlOpt(CURLOPT_XFERINFODATA, this);
#else
    curl_easy_setopt(easy_handle, CURLOPT_PROGRESSFUNCTION, progress_callback);
    curl_easy_setopt(easy_handle, CURLOPT_PROGRESSDATA, this);
#endif

}

CURL *HttpCurlWrapper::curl_handle() {
    return handle_;
}

#pragma mark- HttpMultiCurlWrapper

HttpMultiCurlWrapper::HttpMultiCurlWrapper() {
    DoInit();
}

void HttpMultiCurlWrapper::DoInit() {
    multi_handle_= curl_multi_init();
}

HttpMultiCurlWrapper::~HttpMultiCurlWrapper() {
    curl_multi_cleanup(multi_handle_);
}

CURLMcode HttpMultiCurlWrapper::perform(int *running_handles) {
    return curl_multi_perform(multi_handle_, running_handles);
}

CURLMcode HttpMultiCurlWrapper::wait(curl_waitfd *extra_fds, unsigned int extra_nfds,
                                                int timeout_ms, int *ret) {
    return curl_multi_wait(multi_handle_, extra_fds, extra_nfds, timeout_ms, ret);
}

CURLMcode HttpMultiCurlWrapper::add_handle(CURL *curl_handle) {
    return curl_multi_add_handle(multi_handle_, curl_handle);
}

CURLMcode HttpMultiCurlWrapper::remove_handle(CURL *curl_handle) {
    return curl_multi_remove_handle(multi_handle_, curl_handle);
}

CURLMsg *HttpMultiCurlWrapper::info_read(int *msgs_in_queue) {
    return curl_multi_info_read(multi_handle_, msgs_in_queue);
}
