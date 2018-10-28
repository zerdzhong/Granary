//
// Created by zhongzhendong on 2018/10/27.
//

#include "gtest/gtest.h"

#define private public

#include "thread_loop.hpp"

#undef private

using namespace std;


TEST(ThreadLoopTest, Setup) {
    auto thread_loop = make_unique<ThreadLoop>();
    ASSERT_EQ(thread_loop->CurrentThreadID(), nullptr);
}

TEST(ThreadLoopTest, RunUntil) {
    auto start = std::chrono::system_clock::now();

    auto thread_loop = make_unique<ThreadLoop>();
    thread_loop->RunUntil(start+chrono::milliseconds(50));

    auto end = std::chrono::system_clock::now();

    ASSERT_EQ(end-start, chrono::milliseconds(50));
}

