//
// Created by zhongzhendong on 2018/8/11.
//

#ifndef GRANARY_HTTP_READ_CONNECTION_HPP
#define GRANARY_HTTP_READ_CONNECTION_HPP

#include <string>
#include "base_connection.hpp"

typedef void CURL;

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

private:
    void setupHandle();
    void cleanupHandle();
    size_t receiveData(char *data, size_t size, int type);

private:
    std::string url_;
    std::string range_str_;
    CURL *handle_;
    ConnectionReadData *head_data_;
    ConnectionReadData *body_data_;
};


#endif //GRANARY_HTTP_READ_CONNECTION_HPP
