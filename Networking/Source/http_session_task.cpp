//
// Created by zhongzhendong on 2018/8/11.
//

#include "http_session_task.hpp"

HttpSessionTask::HttpSessionTask():
request_size_(0),
request_start_(0),
received_size_(0),
listener_(nullptr)
{

}

void HttpSessionTask::setListener(HttpSessionTaskListener *listener) {
    listener_ = listener;
}

HttpSessionTaskListener* HttpSessionTask::listener() {
    return listener_;
}

size_t HttpSessionTask::request_size() {
    return request_size_;
}

size_t HttpSessionTask::request_start() {
    return request_start_;
}

size_t HttpSessionTask::received_size() {
    return received_size_;
}