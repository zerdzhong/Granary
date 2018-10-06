//
// Created by zhongzhendong on 2018/8/5.
//

#include "gtest/gtest.h"
#include "thread_base.hpp"

#include <stdlib.h>
#include <vector>
#include <pthread.h>

class TestBuffer {
public:
    void Add();     //生产
    int Reduce();  //消费
    TestBuffer():count_(0){
        pthread_mutex_init(&mutex_, NULL);
    };

private:
    int count_;
    std::vector<int> data_;
    pthread_mutex_t mutex_;
};

void TestBuffer::Add() {
    pthread_mutex_lock(&mutex_);
    count_++;
    data_.push_back(count_);
    pthread_mutex_unlock(&mutex_);
}

int TestBuffer::Reduce() {
    pthread_mutex_lock(&mutex_);
    if (data_.empty()) {
        return -1;
    }
    int element = data_.back();
    data_.pop_back();
    pthread_mutex_unlock(&mutex_);
    return element;
}


class TestThread: public ThreadBase {
public:
    TestThread() {
        buffer_ = new TestBuffer();
    }
    ~TestThread() {
        if (isAlive()) {
            setIsAlive(false);
            Join();
        }
        delete buffer_;
    }
    void run() {
        while (isAlive()) {
            int count = buffer_->Reduce();
//            printf("reduce count:%d\n", count);
        }
    }

    void productBuffer() {
        buffer_->Add();
    }
private:
    TestBuffer *buffer_;
};



TEST(ProducerConsumer,test) {
    printf("\n");
    auto *testThread = new TestThread();
    testThread->Start();

    size_t count  = 0;

    while(count < 1000) {
        testThread->productBuffer();
        count++;
    }

    testThread->Quit();
    delete testThread;
}
