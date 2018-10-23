//
// Created by zhongzhendong on 2018/10/23.
//

#include "thread.hpp"

#if defined(__APPLE__)
#include <pthread.h>
#elif defined(__linux__)
#include <pthread.h>
#endif
#include <stdio.h>
#include <thread.hpp>


void* Thread::start_func(void *arg) {
    auto *ptr = (Thread *)arg;
    ptr->run();
    return nullptr;
}

int Thread::Start() {
    thread_ = std::thread(start_func, this);
    is_alive_ = true;
    return 0;
}

void Thread::Join() {
    thread_.join();
}

bool Thread::isAlive() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool is_alive = is_alive_;
    return is_alive;
}

void Thread::setIsAlive(bool is_alive) {
    std::lock_guard<std::mutex> lock(mutex_);
    is_alive_ = is_alive;
}

void Thread::setThreadName(std::string name) {
    thread_name_ = std::move(name);

#if defined(__APPLE__)
    pthread_setname_np(thread_name_.c_str());
#elif defined(__linux__)
    pthread_setname_np(pthread_self(), thread_name_);
#endif

}

std::thread::id Thread::getThreadId() {
    return thread_.get_id();
}

Thread::~Thread() {
    if (isAlive()) {
        setIsAlive(false);
        Join();
    }
}

Thread::Thread() : is_alive_{false} {

}
