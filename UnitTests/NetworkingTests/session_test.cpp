//
// Created by zhongzhendong on 2018/8/23.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <pthread.h>

#define private public
#define protected public

#include "http_session.hpp"

#undef private
#undef protected

using ::testing::_;
using ::testing::Invoke;
using ::testing::AtLeast;

static const char *TEST_VALID_URLS[] = {
        "https://gensho.ftp.acc.umu.se/pub/gimp/gimp/v2.10/osx/gimp-2.10.4-x86_64.dmg",
        "http://www.baidu.com",
        "http://www.zhihu.com",
        "http://youku.com",
};
#define TEST_VALID_URLS_COUNT 4


class HttpSessionTestFixture : public ::testing::Test, public HttpSessionTaskListener{

public:
    HttpSessionTestFixture() {
        real_session_ = new HttpSession();
        real_session_->setListener(this);

        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&cond_, nullptr);
    }

    ~HttpSessionTestFixture() override {
        delete real_session_;

        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);
    }

    void waitUntilAllFinish(uint8_t request_count) {
        pthread_mutex_lock(&mutex_);

        request_count_ = request_count;

        while(!request_done_) {
            pthread_cond_wait(&cond_, &mutex_);
        }

        pthread_mutex_unlock(&mutex_);
    }

    void OnDataFinish(HttpSessionTask *session_task, int err_code) override {
        pthread_mutex_lock(&mutex_);
        if (++ request_done_ == request_count_) {
            pthread_cond_signal(&cond_);
        }

        pthread_mutex_unlock(&mutex_);
    }

    void OnReady(HttpSessionTask *session_task) override {}
    void OnData(HttpSessionTask *session_task, HttpSessionTaskData *data) override {}


protected:
    void TearDown() override {
    }

    void SetUp() override {
    }

protected:
    HttpSession *real_session_;

    pthread_cond_t cond_;
    pthread_mutex_t mutex_;

    uint8_t request_done_ = 0;
    uint8_t request_count_ = 0;
};


TEST_F(HttpSessionTestFixture, ReadTask) {

    real_session_->ReadTask("http://www.baidu.com");
    ASSERT_EQ(real_session_->pending_tasks_.size(), 1);
    real_session_->ReadTask("http://www.baidu.com");
    ASSERT_EQ(real_session_->pending_tasks_.size(), 2);

    real_session_->Start();
    waitUntilAllFinish(2);

    ASSERT_EQ(request_done_, 2);

}

void* add_task(void * pVoid) {
    auto *session = (HttpSession *) pVoid;
    int count = 10;
    while (count > 0) {
        session->ReadTask("http://www.baidu.com");
        count--;
        usleep(1000);
    }

    return nullptr;
}

TEST_F(HttpSessionTestFixture, Tasks) {

    real_session_->Start();

    pthread_t add_task_thread_1, add_task_thread_2;

    pthread_create(&add_task_thread_1, nullptr, add_task, real_session_);
    pthread_create(&add_task_thread_2, nullptr, add_task, real_session_);

    waitUntilAllFinish(20);

    ASSERT_EQ(request_done_, 20);
}