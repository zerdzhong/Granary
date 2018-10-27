//
// Created by zhongzhendong on 2018/10/15.
//

#ifndef GRANARY_TIMER_HPP
#define GRANARY_TIMER_HPP

#include <thread>
#include "thread_loop.hpp"

class Timer;

typedef double TimeInterval;
typedef TimeInterval AbsoluteTime;
typedef void (*TimerCallBack)(Timer *timer, void *info);

AbsoluteTime CurrentTime();

class Timer {
public:
    Timer(TimeInterval interval, bool is_repeat, std::function<void(Timer*, void*)> lambda_callback);
    ~Timer();

    void SetThreadLoop(ThreadLoop *threadLoop);
    bool isValid();
    bool handleTimer();

    void Fire();

private:
    std::mutex mutex_;
    TimeInterval  interval_;
    bool is_repeat_;
    bool is_valid_;
    ThreadLoop *thread_loop_;
    AbsoluteTime fire_time_;

    std::function<void(Timer*, void*)> callback_;
};


#endif //GRANARY_TIMER_HPP
