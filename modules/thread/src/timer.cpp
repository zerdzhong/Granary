//
// Created by zhongzhendong on 2018/10/15.
//

#include <sys/time.h>
#include <timer.hpp>

#include "timer.hpp"

const TimeInterval kAbsoluteTimeIntervalSince1970 = 978307200.0L;
const TimeInterval kAbsoluteTimeIntervalSince1904 = 3061152000.0L;

AbsoluteTime CurrentTime() {
    AbsoluteTime ret;
    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    ret = (TimeInterval)tv.tv_sec - kAbsoluteTimeIntervalSince1970;
    ret += (1.0E-6 * (TimeInterval)tv.tv_usec);
    return ret;
}

#pragma mark- Life cycle

Timer::Timer(TimeInterval interval, bool is_repeat, TimerCallBack callback) :
interval_{interval},
callback_{callback},
is_repeat_{is_repeat},
is_valid_{false},
thread_loop_{nullptr},
fire_time_{0}
{

}

Timer::~Timer() = default;

#pragma mark- Public Functions

void Timer::Fire() {
    is_valid_ = true;
    fire_time_ = CurrentTime() + interval_;
}

void Timer::SetThreadLoop(ThreadLoop *threadLoop) {
    if (thread_loop_) {
        return;
    }

    thread_loop_ = threadLoop;
}

bool Timer::isValid() {
    return is_valid_;
}

bool Timer::handleTimer() {
    bool timer_handled = false;

    if (nullptr == thread_loop_) {
        return timer_handled;
    }

    if (thread_loop_->getThreadId() != std::this_thread::get_id()) {
        return  timer_handled;
    }

    if (is_valid_ && fire_time_ < CurrentTime()) {
        callback_(this, nullptr);
        timer_handled = true;
    }

    return timer_handled;
}
