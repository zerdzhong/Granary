//
// Created by zhongzhendong on 2018/10/15.
//

#ifndef GRANARY_TIMER_HPP
#define GRANARY_TIMER_HPP

#include <thread>

class Timer;

typedef double TimeInterval;
typedef TimeInterval AbsoluteTime;
typedef void (*TimerCallBack)(Timer *timer, void *info);

AbsoluteTime GetGurrentTime();

class Timer {
public:
    Timer(TimeInterval interval, bool is_repeat, TimerCallBack callback);
    ~Timer();

    void Fire();

private:
    std::mutex mutex_;
    TimeInterval  interval_;
    TimerCallBack callback_;
    bool is_repeat_;
};


#endif //GRANARY_TIMER_HPP
