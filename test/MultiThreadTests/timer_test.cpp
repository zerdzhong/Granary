//
// Created by zhongzhendong on 2018/10/20.
//

#include "gtest/gtest.h"
#include "thread_loop.hpp"

#define private public
#include "timer.hpp"
#undef private

using namespace std;

TEST(TimerTest, Create) {
    auto timer = make_unique<Timer>(0.3, false, nullptr);

    ASSERT_EQ(timer->interval_, 0.3);
    ASSERT_EQ(timer->is_repeat_, false);
    ASSERT_EQ(timer->callback_, nullptr);

    auto timer1 = new Timer(0.1, true, nullptr);
    ASSERT_EQ(timer1->interval_, 0.1);
    ASSERT_EQ(timer1->is_repeat_, true);
    ASSERT_EQ(timer1->callback_, nullptr);
}


TEST(TimerTest, Fire) {
    auto timer = make_unique<Timer>(0.3, false, nullptr);
    auto fire_time = CurrentTime();

    timer->Fire();

    ASSERT_EQ(timer->isValid(), true);
    ASSERT_EQ(timer->fire_time_, fire_time + 0.3);
}

TEST(TimerTest, Handle) {
    auto timer = make_unique<Timer>(0.3, false, nullptr);
    bool is_handled = timer->handleTimer();
    ASSERT_EQ(is_handled, false);
}

