#include <utility>

//
// Created by zhongzhendong on 2018/8/2.
//

#include "thread_loop.hpp"
#if defined(__APPLE__)
#include <pthread.h>
#elif defined(__linux__)
#include <pthread.h>
#endif
#include <stdio.h>
#include <thread_loop.hpp>

#include "timer.hpp"


void* ThreadLoop::start_func(void *arg) {
    auto *ptr = (ThreadLoop *)arg;
    ptr->run();
    return nullptr;
}

int ThreadLoop::Start() {
    thread_ = std::thread(start_func, this);
    is_alive_ = true;
    return 0;
}

void ThreadLoop::Join() {
    thread_.join();
}

bool ThreadLoop::isAlive() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool is_alive = is_alive_;
    return is_alive;
}

void ThreadLoop::setIsAlive(bool is_alive) {
    std::lock_guard<std::mutex> lock(mutex_);
    is_alive_ = is_alive;
}

void ThreadLoop::setThreadName(std::string name) {
    thread_name_ = std::move(name);

#if defined(__APPLE__)
    pthread_setname_np(thread_name_.c_str());
#elif defined(__linux__)
    pthread_setname_np(pthread_self(), thread_name_);
#endif

}

ThreadLoop::~ThreadLoop() {
    if (isAlive()) {
        setIsAlive(false);
        Join();
    }
}

ThreadLoop::ThreadLoop() {

}

void ThreadLoop::AddTimer(Timer* timer) {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    timer_.insert(timer);
}
