//
// Created by zhongzhendong on 2018/8/2.
//

#include "gtest/gtest.h"
#include <iostream>

#define private public

#include "thread.hpp"

#undef private

#include <mutex>
#include <thread>
#include <chrono>

using namespace std;

class TestModel {
public:
    TestModel():count_(0) {
        data = (char *)malloc(4);
        memset(data,0, sizeof(int));
    }

    void AddCount() {
        count_ ++;
        memset(data, count_, sizeof(count_));
    }

    int ReadData() {
        return (int)data[0];
    }

    int count_;
private:
    char *data;
};


class ThreadTest : public Thread
{
public:
    ThreadTest();
    ~ThreadTest() override;
    void run() override;

private:
    TestModel *test_;
    std::mutex mutex_;
};

ThreadTest::ThreadTest() {
    test_ = new TestModel();
}

void ThreadTest::run()
{
    while (isAlive()) {

        std::this_thread::sleep_for (std::chrono::milliseconds(10));

        std::lock_guard<std::mutex> lock(mutex_);
        auto test_count =  test_->count_ ;
        test_->AddCount();
    }
}

ThreadTest::~ThreadTest() {

    if (isAlive()) {
        setIsAlive(false);
        Join();
    }

    delete test_;
    test_ = nullptr;
}

TEST(ThreadBaseTest, start) {

    printf("\n");
    auto test1 = new ThreadTest();
    auto test2 = new ThreadTest();
    auto test3 = new ThreadTest();

    test1->Start();
    test2->Start();
    test3->Start();

    std::this_thread::sleep_for (std::chrono::seconds(1));

    delete test1;
    delete test2;
    delete test3;

    std::this_thread::sleep_for (std::chrono::seconds(2));

}

TEST(ThreadBaseTest, setName) {
    auto test1 = new ThreadTest();
    test1->setThreadName("test_1");

    ASSERT_EQ(test1->thread_name_, "test_1");
}

