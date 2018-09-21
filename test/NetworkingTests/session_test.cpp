//
// Created by zhongzhendong on 2018/8/23.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include <pthread.h>
#include "http_session_read_task.hpp"
#include "http_session_config.hpp"

#define private public
#define protected public

#include "http_session.hpp"

#undef private
#undef protected

#include "session_mock_listener.cpp"

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


class HttpSessionTestFixture : public ::testing::Test {

public:
    HttpSessionTestFixture() {
        mock_listener_ = new MockSessionTaskListener();
        test_session_ = new HttpSession();
        test_session_->setListener(mock_listener_);
        test_session_->setTaskAutoDelete(false);

        ON_CALL(*mock_listener_, OnDataFinish(_, _)).
                WillByDefault(Invoke(this, &HttpSessionTestFixture::OnDataFinish));

        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&cond_, nullptr);
    }

    ~HttpSessionTestFixture() override {
        delete test_session_;

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

    void OnDataFinish(HttpSessionTask *session_task, int err_code) {
        pthread_mutex_lock(&mutex_);
        if (++ request_done_ == request_count_) {
            pthread_cond_signal(&cond_);
        }

        pthread_mutex_unlock(&mutex_);
    }

    void CancelOnceReceiveData(HttpSessionTask *session_task, HttpSessionTaskData *read_data) {
        HttpSessionReadTask *read_task = dynamic_cast<HttpSessionReadTask *>(session_task);
        if (read_task && kReadDataTypeBody == read_data->type) {
            test_session_->CancelTask(read_task);
        }
    }

protected:
    void TearDown() override {
    }

    void SetUp() override {
        std::cout<< "session curl info: " << test_session_->CurlInfo() << std::endl;
    }

protected:
    HttpSession *test_session_;
    MockSessionTaskListener *mock_listener_;

    pthread_cond_t cond_{};
    pthread_mutex_t mutex_{};

    uint8_t request_done_ = 0;
    uint8_t request_count_ = 0;
};


TEST_F(HttpSessionTestFixture, ReadTask) {

    test_session_->ReadTask("http://www.baidu.com");
    ASSERT_EQ(test_session_->pending_tasks_.size(), 1);
    test_session_->ReadTask("http://www.baidu.com");
    ASSERT_EQ(test_session_->pending_tasks_.size(), 2);

    test_session_->Start();
    waitUntilAllFinish(2);

    ASSERT_EQ(request_done_, 2);

}

TEST_F(HttpSessionTestFixture, ReadTaskWithRange) {
    HttpSessionReadTask *task1 = test_session_->ReadTaskWithInfo("https://www.baidu.com", 0, 8);
    ASSERT_EQ(test_session_->pending_tasks_.size(), 1);
    HttpSessionReadTask *task2 = test_session_->ReadTaskWithInfo("https://www.baidu.com",0, 10);
    ASSERT_EQ(test_session_->pending_tasks_.size(), 2);

    test_session_->Start();
    waitUntilAllFinish(2);

    ASSERT_EQ(request_done_, 2);
    ASSERT_EQ(task1->received_size(), 8);
    ASSERT_EQ(task2->received_size(), 10);
}

TEST_F(HttpSessionTestFixture, CancelTask) {

    ON_CALL(*mock_listener_, OnData(_, _)).WillByDefault(Invoke(this, &HttpSessionTestFixture::CancelOnceReceiveData));
    EXPECT_CALL(*mock_listener_, OnDataFinish(_, CONN_USER_CANCEL)).Times(1);

    HttpSessionReadTask *task = test_session_->ReadTaskWithInfo(TEST_VALID_URLS[0], 0, 0);
    test_session_->Start();

    waitUntilAllFinish(1);
    size_t received_size1 = task->received_size();

    ASSERT_GT(received_size1, 0);
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

    test_session_->Start();

    pthread_t add_task_thread_1, add_task_thread_2;

    pthread_create(&add_task_thread_1, nullptr, add_task, test_session_);
    pthread_create(&add_task_thread_2, nullptr, add_task, test_session_);

    waitUntilAllFinish(20);

    ASSERT_EQ(request_done_, 20);
}

TEST_F(HttpSessionTestFixture, SetConfig) {
    HttpSessionConfig *config = HttpSessionConfig::DefaultSessionConfig();

    test_session_->setSessionConfig(config);

    ASSERT_EQ(test_session_->sessionConfig(), config);

    delete config;
}

