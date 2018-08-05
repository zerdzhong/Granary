//
// Created by zhongzhendong on 2018/8/4.
//

#include "http_connection.hpp"

void HttpConnectionThread::run() {
    while (isAlive()) {
        connection_->runInternal();
    }
}


void HttpConnection::Start() {
    if (thread_ == nullptr) {
        thread_ = new HttpConnectionThread();
    }
}

void HttpConnection::runInternal() {

}
