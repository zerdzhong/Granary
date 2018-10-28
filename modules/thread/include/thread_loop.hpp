//
// Created by zhongzhendong on 2018/8/2.
//

#ifndef GRANARY_THREAD_BASE_HPP
#define GRANARY_THREAD_BASE_HPP

#include <thread>
#include <mutex>
#include <string>
#include <set>
#include <chrono>

class Timer;
class Thread;

typedef enum {
    kThreadLoopRunResultFinished = 1,
    kThreadLoopRunResultStopped = 2,
    kThreadLoopRunResultTimedOut = 3,
    kThreadLoopRunResultHandledSource = 4
}ThreadLoopRunResult;

class ThreadLoop {
public:
    ThreadLoop();
    ~ThreadLoop();

    void AddTimer(Timer *timer);
    void Run();

    template <class _Clock, class _Duration>
    void RunUntil(const std::chrono::time_point<_Clock, _Duration>& limited_time);

    std::thread::id CurrentThreadID();

private:
    template <class _Clock, class _Duration> ThreadLoopRunResult RunSpecific(const std::chrono::time_point<_Clock, _Duration>& limited_time);
    bool RunTimers();

private:
    std::mutex mutex_;
    std::thread::id thread_id_;
    std::set<Timer *> timers_;
};


#endif //GRANARY_THREAD_BASE_HPP
