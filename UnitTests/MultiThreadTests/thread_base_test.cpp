//
// Created by zhongzhendong on 2018/8/2.
//

#include "gtest/gtest.h"
#include <iostream>
#include "thread_base.hpp"
#include <pthread.h>

using namespace std;

class Test {
public:
    Test():count_(0) {

    }

    void AddCount() {
        count_ ++;
    }

    int count_;
};


class ThreadTest : public ThreadBase
{
public:
    ThreadTest();
    ~ThreadTest() override;
    void run() override;

private:
    int count_;
    Test *test_;
    pthread_mutex_t mutex_;
};

ThreadTest::ThreadTest() {
    mutex_ = PTHREAD_MUTEX_INITIALIZER;
    test_ = new Test();
}

void ThreadTest::run()
{
    while (isAlive())
    {
        usleep(50);
        pthread_mutex_lock(&mutex_);
        cout<< this << ", count: " << test_->count_ <<endl;
        pthread_mutex_unlock(&mutex_);

        test_->AddCount();
    }
}

ThreadTest::~ThreadTest() {
    cout << "Release ThreadTest, " << this << endl;
}

TEST(ThreadBaseTest, start) {
    printf("\n");
    ThreadBase *test1 = new ThreadTest();
    ThreadBase *test2 = new ThreadTest();
    ThreadBase *test3 = new ThreadTest();

    test1->Start();
    test2->Start();
    test3->Start();

    sleep(1);

    test1->Quit();
    test2->Quit();

    delete test1;
    delete test2;
    delete test3;

    sleep(2);
}

