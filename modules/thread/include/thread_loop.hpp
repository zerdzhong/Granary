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
    int Start();
    void Join();

    bool isAlive();

    virtual void run() = 0;
    virtual ~ThreadLoop();
    ThreadLoop();

    void setThreadName(std::string name);
    void AddTimer(Timer *timer);

    std::thread::id getThreadId();

protected:
    static void* start_func(void* arg);
    void setIsAlive(bool is_alive);

private:
    void RunTimers();

private:
    std::mutex mutex_;
    std::thread thread_;
    bool is_alive_;
    std::string thread_name_;

    std::set<Timer *> timers_;
};


#endif //GRANARY_THREAD_BASE_HPP
