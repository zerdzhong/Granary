//
// Created by zhongzhendong on 2018/8/11.
//

#ifndef GRANARY_HTTP_READ_CONNECTION_HPP
#define GRANARY_HTTP_READ_CONNECTION_HPP


#include "base_connection.hpp"


class HttpReadConnection : public BaseConnection {
public:
    explicit HttpReadConnection(const char *url, size_t offset, size_t length);

    const char *url();
private:
    const char *url_;
};


#endif //GRANARY_HTTP_READ_CONNECTION_HPP
