#include <utility>
#include <thread_loop.hpp>


//
// Created by zhongzhendong on 2018/8/2.
//

#include "thread_loop.hpp"
#include "timer.hpp"


ThreadLoop::~ThreadLoop() {

}

ThreadLoop::ThreadLoop() {

}

void ThreadLoop::AddTimer(Timer* timer) {
    std::lock_guard<std::mutex> lock_guard(mutex_);

    timer->SetThreadLoop(this);
    timers_.insert(timer);
}

void ThreadLoop::RunTimers() {
    for (auto timer : timers_) {
        if (timer->isValid()) {
            timer->handleTimer();
        }
    }
}

bool ThreadLoop::isAlive() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool is_alive = is_alive_;
    return is_alive;
}

std::thread::id ThreadLoop::getThreadId() {
    return thread_.get_id();
}

void ThreadLoop::Run() {

}
