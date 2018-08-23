//
// Created by zhongzhendong on 2018/8/4.
//

#include "http_session.hpp"

#pragma mark- HttpSessionThread

class HttpSessionThread: public ThreadBase {
public:
    void run() override;

private:
    HttpSession *connection_;
};


void HttpSessionThread::run() {
    while (isAlive()) {
        connection_->runInternal();
    }
}

#pragma mark- HttpSession

void HttpSession::Start() {
    if (thread_ == nullptr) {
        thread_ = new HttpSessionThread();
    }
}

void HttpSession::runInternal() {

}

#pragma mark- Public

void HttpSession::ReadTask(std::string url) {
    ReadTask(url, 0, 0);
}

void HttpSession(std::string url, size_t offset, size_t length){

}
