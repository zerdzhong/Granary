//
// Created by zhongzhendong on 2018/8/2.
//

#ifndef GRANARY_THREAD_BASE_HPP
#define GRANARY_THREAD_BASE_HPP

#include <thread>
#include <mutex>
#include <string>
#include <set>

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

    bool isAlive();
    Thread* currentThread();

private:
    void setIsAlive(bool is_alive);
    ThreadLoopRunResult RunSpecific();

    bool RunTimers();
private:
    std::mutex mutex_;
    Thread* thread_;
    bool is_alive_;
    std::set<Timer *> timers_;
};


#endif //GRANARY_THREAD_BASE_HPP
