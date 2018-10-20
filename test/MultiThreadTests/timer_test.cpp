//
// Created by zhongzhendong on 2018/10/20.
//

#include "gtest/gtest.h"

#define private public
#include "timer.hpp"
#undef private

using namespace std;

TEST(TimerTest, Create) {
    auto timer = make_unique<Timer>(0.3, false, nullptr);

    ASSERT_EQ(timer->interval_, 0.3);
    ASSERT_EQ(timer->is_repeat_, false);
    ASSERT_EQ(timer->callback_, nullptr);
}
