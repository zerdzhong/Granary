//
// Created by zhongzhendong on 2018/8/11.
//

#ifndef GRANARY_HTTP_READ_CONNECTION_HPP
#define GRANARY_HTTP_READ_CONNECTION_HPP

#include <string>
#include "http_session_task.hpp"

typedef void CURL;

typedef enum {
    CONN_OK      = 0,        //download success
    CONN_RT_ERR = 1000      ,         //invalid parameter
    CONN_INVALID_PARAM      ,           //runtime error
    CONN_OUT_OF_MEMORY      ,           //out of memory
    CONN_FAILED_INIT        ,           //initialization failed
    CONN_TIMEOUT            ,           //timeout
    CONN_USER_CANCEL        ,           //user canceled
    CONN_TCP_CONN_FAILED    ,           //network connection failed
    CONN_INVALID_URL        ,           //invalid url format
    CONN_UNSUPPORT_PROTOCOL ,

    /* http/https protocol */
    HTTP_OVER_REDIRECT = 2000   ,         //too many redirects
    HTTP_BAD_REQUEST            ,        //HTTP 400
    HTTP_UNAUTHORIZED           ,        //HTTP 401
    HTTP_FORBIDDEN              ,        //HTTP 403
    HTTP_NOT_FOUND              ,        //HTTP 404
    HTTP_TIMEOUT                ,        //HTTP 408
    HTTP_RANGE_ERROR            ,        //HTTP 416
    HTTP_OTHER_4XX              ,        //HTTP 4xx
    HTTP_SERVER_502             ,        //HTTP 502
    HTTP_SERVER_503             ,        //HTTP 503
    HTTP_SERVER_504             ,        //HTTP 504
    HTTP_SERVER_ERROR           ,        //HTTP 5xx
} HttpConnectionCode;

class HttpSessionReadTask : public HttpSessionTask {
public:
    explicit HttpSessionReadTask(std::string url, size_t offset, size_t length);
    ~HttpSessionReadTask();

    //curl callback
    size_t ReceiveData(char *data, size_t size, size_t nmemb);
    size_t ReceiveHeader(char *data, size_t size, size_t nmemb);
    int ReceiveProgress(long long dltotal, long long dlnow);

    //Read
    void SyncRead();
    void Cancel();
    void ReadConnectionFinished(int code);

    std::string url();
    CURL* handle();
    uint8_t retry_count();
    void set_retry_count(uint8_t retry_count);

private:
    void setupHandle();
    void cleanupHandle();
    void SyncRead(uint8_t retry_count);
    size_t receiveData(char *data, size_t size, int type);
    HttpConnectionCode parseErrorReason(int code);
    std::string parseEffectiveUrl();

private:
    std::string url_;
    std::string range_str_;
    CURL *handle_;
    HttpSessionTaskData *head_data_;
    HttpSessionTaskData *body_data_;

    std::string effective_url_;
    HttpConnectionCode result_code_;

    uint8_t request_count_;
    uint8_t retry_count_;
    bool stopped_;
};


#endif //GRANARY_HTTP_READ_CONNECTION_HPP
