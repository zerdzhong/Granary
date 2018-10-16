//
// Created by zhongzhendong on 2018/10/15.
//

#ifndef GRANARY_TIMER_HPP
#define GRANARY_TIMER_HPP

#include <thread>

class Timer;

typedef double TimeInterval;
typedef void (*TimerCallBack)(Timer *timer, void *info);

class Timer {
public:
    Timer();
    ~Timer();

private:
    std::mutex mutex_;
    TimeInterval  interval_;
    TimerCallBack callout_;
};


#endif //GRANARY_TIMER_HPP
