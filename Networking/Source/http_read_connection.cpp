//
// Created by zhongzhendong on 2018/8/11.
//

#include "http_read_connection.hpp"


HttpReadConnection::HttpReadConnection(const char *url, size_t offset, size_t length) :
url_(url)
{
    request_start_ = offset;
    request_size_ = length;
}

const char* HttpReadConnection::url() {
    return url_;
}