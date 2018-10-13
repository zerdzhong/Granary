#include <utility>

//
// Created by zhongzhendong on 2018/8/2.
//

#include "thread_base.hpp"
#if defined(__APPLE__)
#include <pthread.h>
#elif defined(__linux__)
#include <pthread.h>
#endif
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

void ThreadBase::Join() {
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
    thread_name_ = std::move(name);

#if defined(__APPLE__)
    pthread_setname_np(thread_name_.c_str());
#elif defined(__linux__)
    pthread_setname_np(pthread_self(), thread_name_);
#endif

}


ThreadBase::~ThreadBase() {
    if (isAlive()) {
        setIsAlive(false);
        Join();
    }
}

ThreadBase::ThreadBase() {

}
