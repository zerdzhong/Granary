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

class ThreadLoop {
public:
    ThreadLoop();
    ~ThreadLoop();

    void AddTimer(Timer *timer);
    void Run();

    bool isAlive();
    std::thread::id getThreadId();

private:
    void setIsAlive(bool is_alive);
    void RunTimers();

private:
    std::mutex mutex_;
    std::thread thread_;
    bool is_alive_;
    std::set<Timer *> timers_;
};


#endif //GRANARY_THREAD_BASE_HPP
