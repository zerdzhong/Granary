//
// Created by zhongzhendong on 2018/8/4.
//

#include "http_session.hpp"

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

void HttpSession::Start() {
    if (thread_ == nullptr) {
        thread_ = new HttpSessionThread();
    }
}

void HttpSession::runInternal() {

}
