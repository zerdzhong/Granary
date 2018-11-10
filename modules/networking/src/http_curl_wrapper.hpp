//
// Created by zhongzhendong on 2018/11/6.
//

#ifndef GRANARY_HTTP_CURL_WRAPPER_HPP
#define GRANARY_HTTP_CURL_WRAPPER_HPP


#include <cstddef>

typedef void CURL;

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


#endif //GRANARY_HTTP_CURL_WRAPPER_HPP
