//
// Created by zhongzhendong on 2018/8/11.
//

#ifndef GRANARY_HTTP_READ_CONNECTION_HPP
#define GRANARY_HTTP_READ_CONNECTION_HPP

#include <string>
#include "base_connection.hpp"

typedef void CURL;

typedef enum {
    CONN_DOWNLOAD_OK         = 0,        //download success
    CONN_INVAL_HANDLE     = 1000,     // invalid handle
    CONN_CURL_RUNNING,
    CONN_INVAL_PARAM      ,           //invalid parameter
    CONN_RT_ERR           ,           //runtime error
    CONN_OUT_OF_MEMORY    ,           //out of memory
    CONN_FAILED_INIT      ,           //initialization failed
    CONN_TIMEOUT          ,           //timeout
    CONN_USER_CANCEL      ,           //user canceled
    CONN_TCP_CONN_FAILED ,           //network connection failed
    CONN_INVALID_URL      ,           //invalid url format
    CONN_UNSUPPORT_PROTOCOL,

    /* http/https protocol */
    HTTP_OVER_REDIRECT = 2000,  //too many redirects
    HTTP_BAD_REQUEST   ,        //HTTP 400
    HTTP_UNAUTHORIZED  ,        //HTTP 401
    HTTP_FORBIDDEN     ,        //HTTP 403
    HTTP_NOT_FOUND     ,        //HTTP 404
    HTTP_TIMEOUT       ,        //HTTP 408
    HTTP_RANGE_ERROR   ,        //HTTP 416
    HTTP_OTHER_4XX     ,        //HTTP 4xx
    HTTP_SERVER_502    ,        //HTTP 502
    HTTP_SERVER_503    ,        //HTTP 503
    HTTP_SERVER_504    ,        //HTTP 504
    HTTP_SERVER_ERROR  ,        //HTTP 5xx
} HttpConnectionCode;

class HttpReadConnection : public BaseConnection {
public:
    explicit HttpReadConnection(std::string url, size_t offset, size_t length);
    ~HttpReadConnection();

    //curl callback
    size_t ReceiveData(char *data, size_t size, size_t nmemb);
    size_t ReceiveHeader(char *data, size_t size, size_t nmemb);
    int ReceiveProgress(long long dltotal, long long dlnow);

    //Read
    void SyncRead();
    void ReadConnectionFinished(int code);

    std::string url();
    CURL* handle();
    uint8_t retry_count();
    void set_retry_count(uint8_t retry_count);

private:
    void setupHandle();
    void cleanupHandle();
    size_t receiveData(char *data, size_t size, int type);
    HttpConnectionCode errorReason(int code);
private:
    std::string url_;
    std::string range_str_;
    CURL *handle_;
    ConnectionReadData *head_data_;
    ConnectionReadData *body_data_;

    uint8_t retry_count_;
};


#endif //GRANARY_HTTP_READ_CONNECTION_HPP
