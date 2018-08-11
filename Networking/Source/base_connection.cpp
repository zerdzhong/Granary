//
// Created by zhongzhendong on 2018/8/11.
//

#include "base_connection.hpp"

BaseConnection::BaseConnection():
request_size_(0),
request_start_(0),
received_size_(0),
listener_(nullptr)
{

}

void BaseConnection::registListener(ConnectionListener *listener) {
    listener_ = listener;
}

ConnectionListener* BaseConnection::listener() {
    return listener_;
}

size_t BaseConnection::request_size() {
    return request_size_;
}

size_t BaseConnection::request_start() {
    return request_start_;
}

size_t BaseConnection::received_size() {
    return received_size_;
}