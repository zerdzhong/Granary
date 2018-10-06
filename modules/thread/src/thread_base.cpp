//
// Created by zhongzhendong on 2018/8/2.
//

#include "thread_base.hpp"
#include <stdio.h>

void* ThreadBase::start_func(void *arg) {
    auto *ptr = (ThreadBase *)arg;
    ptr->run();
    return nullptr;
}

int ThreadBase::Start() {
    thread_ = std::thread(start_func, this);
    is_alive_ = true;
    return 0;
}

int ThreadBase::Join() {
    thread_.join();
}

int ThreadBase::Quit() {
    if (isAlive()) {
        setIsAlive(false);
        Join();
    }
    return 0;
}

bool ThreadBase::isAlive() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool is_alive = is_alive_;
    return is_alive;
}

void ThreadBase::setIsAlive(bool is_alive) {
    std::lock_guard<std::mutex> lock(mutex_);
    is_alive_ = is_alive;
}

void ThreadBase::setThreadName(std::string name) {
    thread_name_ = name;
}


ThreadBase::~ThreadBase() {
    if (isAlive()) {
        setIsAlive(false);
        Join();
    }
}

ThreadBase::ThreadBase() {

}
