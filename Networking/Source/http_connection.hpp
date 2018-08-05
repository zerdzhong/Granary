//
// Created by zhongzhendong on 2018/8/4.
//

#ifndef GRANARY_HTTP_CONNECTION_HPP
#define GRANARY_HTTP_CONNECTION_HPP

#include "thread_base.hpp"

class HttpConnection;

class HttpConnectionThread: public ThreadBase {
public:
    void run() override;

private:
    HttpConnection *connection_;
};

class HttpConnection {
public:
    void Start();
    void runInternal();

private:
    HttpConnectionThread *thread_;

};


#endif //GRANARY_HTTP_CONNECTION_HPP
