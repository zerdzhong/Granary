//
// Created by zhongzhendong on 2018/8/2.
//

#include <utility>
#include <thread_loop.hpp>
#include "timer.hpp"
#include "thread.hpp"
#include <chrono>

using namespace std::chrono;

ThreadLoop::~ThreadLoop() {

}

ThreadLoop::ThreadLoop()
{

}

void ThreadLoop::AddTimer(Timer* timer) {
    std::lock_guard<std::mutex> lock_guard(mutex_);

    timer->SetThreadLoop(this);
    timers_.insert(timer);
}

void ThreadLoop::Run() {
    auto distant_future = system_clock::now() + seconds::max();
    RunUntil(distant_future);
}

template <class _Clock, class _Duration>
void ThreadLoop::RunUntil(const time_point<_Clock, _Duration>& limited_time) {
    thread_id_ = std::this_thread::get_id();
    while (system_clock::now() < limited_time) {
        RunSpecific(limited_time);
    }
}

std::thread::id ThreadLoop::CurrentThreadID() {
    return thread_id_;
}

#pragma mark- Private function

template <class _Clock, class _Duration>
ThreadLoopRunResult ThreadLoop::RunSpecific(const std::chrono::time_point<_Clock, _Duration>& limited_time)
{

    ThreadLoopRunResult result = kThreadLoopRunResultFinished;

    do {
        if (RunTimers()) {
            result = kThreadLoopRunResultHandledSource;
        }

        if (system_clock::now() > limited_time) {
            result = kThreadLoopRunResultFinished;
        }
        

    } while (kThreadLoopRunResultStopped != result && kThreadLoopRunResultFinished != result);

    return result;
}


bool ThreadLoop::RunTimers() {
    bool res = false;
    for (auto timer : timers_) {
        if (timer->isValid()) {
            res = timer->handleTimer() || res;
        }
    }

    return res;
}


