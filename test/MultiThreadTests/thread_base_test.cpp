//
// Created by zhongzhendong on 2018/8/2.
//

#include "gtest/gtest.h"
#include <iostream>
#include "thread_base.hpp"
#include <pthread.h>

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


class ThreadTest : public ThreadBase
{
public:
    ThreadTest();
    ~ThreadTest() override;
    void run() override;

private:
    TestModel *test_;
    pthread_mutex_t mutex_;
};

ThreadTest::ThreadTest() {
    mutex_ = PTHREAD_MUTEX_INITIALIZER;
    test_ = new TestModel();
}

void ThreadTest::run()
{
    while (isAlive())
    {
        usleep(50);
        pthread_mutex_lock(&mutex_);
        auto test_count =  test_->count_ ;
        pthread_mutex_unlock(&mutex_);

        test_->AddCount();
    }
}

ThreadTest::~ThreadTest() {

    if (isAlive()) {
        setIsAlive(false);
        pthread_join(getTid(), NULL);
    }

    delete test_;
    test_ = nullptr;
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

    delete test1;
    delete test2;
    delete test3;

    sleep(2);
}

