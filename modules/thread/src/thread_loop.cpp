//
// Created by zhongzhendong on 2018/8/2.
//

#include <utility>
#include <thread_loop.hpp>
#include "timer.hpp"
#include "thread.hpp"


ThreadLoop::~ThreadLoop() {

}

ThreadLoop::ThreadLoop() {

}

void ThreadLoop::AddTimer(Timer* timer) {
    std::lock_guard<std::mutex> lock_guard(mutex_);

    timer->SetThreadLoop(this);
    timers_.insert(timer);
}

bool ThreadLoop::isAlive() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool is_alive = is_alive_;
    return is_alive;
}

void ThreadLoop::Run() {
    ThreadLoopRunResult result;
    do {
        result = RunSpecific();
    } while (kThreadLoopRunResultStopped != result && kThreadLoopRunResultFinished != result);
}

Thread *ThreadLoop::currentThread() {
    return thread_;
}

#pragma mark- Private function

ThreadLoopRunResult ThreadLoop::RunSpecific() {

    ThreadLoopRunResult result = kThreadLoopRunResultFinished;

    if (RunTimers()) {
        result = kThreadLoopRunResultHandledSource;
    }

    return result;
}


bool ThreadLoop::RunTimers() {
    bool res = false;
    for (auto timer : timers_) {
        if (timer->isValid()) {
            res = res || timer->handleTimer();
        }
    }

    return res;
}
