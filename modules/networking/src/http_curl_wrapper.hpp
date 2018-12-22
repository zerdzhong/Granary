//
// Created by zhongzhendong on 2018/11/6.
//

#ifndef GRANARY_HTTP_CURL_WRAPPER_HPP
#define GRANARY_HTTP_CURL_WRAPPER_HPP

#include <cstddef>
#include <curl/multi.h>

typedef void CURL;
typedef void CURLM;

class HttpCurlCallback {
public:
    virtual size_t OnCurlWrite(char *data, size_t size) = 0;
    virtual size_t OnCurlHeader(char *data, size_t size) = 0;
    virtual int OnCurlProgress(long long dltotal, long long dlnow) {};
};

class HttpCurlWrapper {
public:

    explicit HttpCurlWrapper(HttpCurlCallback* callback);
    ~HttpCurlWrapper();

    //curl callback
    size_t ReceiveData(char *data, size_t size, size_t nmemb);
    size_t ReceiveHeader(char *data, size_t size, size_t nmemb);
    int ReceiveProgress(long long dltotal, long long dlnow);

    CURL* curl_handle();

    template <class T, class P> void setCurlOpt(T type, P parameter) {
        if (!handle_) { return; }
        curl_easy_setopt(handle_, type, parameter);
    }

    template <class T, class P> void getCurlInfo(T type, P parameter) {
        if (!handle_) { return; }
        curl_easy_getinfo(handle_, type, parameter);
    }

private:
    void DoInit();

private:
    CURL *handle_;
    HttpCurlCallback *callback_;
};


class HttpMultiCurlWrapper {
public:

    explicit HttpMultiCurlWrapper();
    ~HttpMultiCurlWrapper();

    CURLMcode perform(int *running_handles);
    CURLMcode wait(struct curl_waitfd extra_fds[],
                   unsigned int extra_nfds,
                   int timeout_ms,
                   int *ret);

    CURLMcode add_handle(CURL *curl_handle);
    CURLMcode remove_handle(CURL *curl_handle);

    CURLMsg *info_read(int *msgs_in_queue);

private:
    void DoInit();
    CURLM *multi_handle_;
};


#endif //GRANARY_HTTP_CURL_WRAPPER_HPP
