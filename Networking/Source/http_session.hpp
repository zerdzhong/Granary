//
// Created by zhongzhendong on 2018/8/4.
//

#ifndef GRANARY_HTTP_CONNECTION_HPP
#define GRANARY_HTTP_CONNECTION_HPP

#include "thread_base.hpp"
#include <string>

class HttpSessionThread;

class HttpSession {
public:
    void ReadTaskWithUrl(std::string url);
    void Start();
    void runInternal();

private:
    HttpSessionThread *thread_;
};


#endif //GRANARY_HTTP_CONNECTION_HPP
