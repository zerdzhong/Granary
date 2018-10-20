//
// Created by zhongzhendong on 2018/10/15.
//

#include <sys/time.h>
#include <timer.hpp>

#include "timer.hpp"

const TimeInterval kAbsoluteTimeIntervalSince1970 = 978307200.0L;
const TimeInterval kAbsoluteTimeIntervalSince1904 = 3061152000.0L;

AbsoluteTime GetGurrentTime() {
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
is_repeat_{is_repeat}
{

}

Timer::~Timer() = default;

#pragma mark- Public Functions

void Timer::Fire() {

}
