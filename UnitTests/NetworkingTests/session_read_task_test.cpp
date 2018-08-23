//
// Created by zhongzhendong on 2018/8/11.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define private public
#define protected public

#include "http_session_read_task.hpp"
#include <curl/curl.h>

#undef private
#undef protected

using ::testing::_;
using ::testing::Invoke;

static const char *TEST_VALID_URLS[] = {
        "https://gensho.ftp.acc.umu.se/pub/gimp/gimp/v2.10/osx/gimp-2.10.4-x86_64.dmg",
        "http://www.baidu.com",
        "http://www.zhihu.com",
        "http://youku.com",
};
#define TEST_VALID_URLS_COUNT 4

static const char *TEST_INVALID_URLS[] = {
        "https:???",
        "http:/www.com",
        "ftp://www.zhihu.com",
        "",
};
#define TEST_INVALID_URLS_COUNT 4


TEST(SessionReadTaskTest, initWithValidURLs) {

    for (int i = 0; i < TEST_VALID_URLS_COUNT; ++i) {
        HttpSessionReadTask *read_session_task = new HttpSessionReadTask(TEST_VALID_URLS[i], i, i);

        ASSERT_NE(nullptr, read_session_task);
        ASSERT_EQ(read_session_task->url(), TEST_VALID_URLS[i]);
        ASSERT_EQ(read_session_task->request_start(), i);
        ASSERT_EQ(read_session_task->request_size(), i);
        ASSERT_NE(read_session_task->handle(), nullptr);

        delete(read_session_task);
    }
}


TEST(SessionReadTaskTest, initWithInvalidURLs) {

    for (auto &url : TEST_INVALID_URLS) {
        HttpSessionReadTask *read_task = new HttpSessionReadTask(url, 0, 0);

        ASSERT_NE(nullptr, read_task);

        delete(read_task);
    }
}

TEST(SessionReadTaskTest, SyncRead) {
    HttpSessionReadTask *read_session_task1 = new HttpSessionReadTask("https://www.baidu.com", 0, 0);
    read_session_task1->SyncRead();
    ASSERT_GT(read_session_task1->received_size(), 0);

    HttpSessionReadTask *read_session_task2 = new HttpSessionReadTask("https://www.baidu.com", 0, 0);
    read_session_task2->SyncRead();
    ASSERT_GT(read_session_task2->received_size(), 0);

    ASSERT_EQ(read_session_task1->received_size(), read_session_task2->received_size());

    delete(read_session_task1);
    delete(read_session_task2);
}

class MockConnectionListener : public HttpSessionTaskListener {
public:
    MockConnectionListener():received_size_(0){

    }
    MOCK_METHOD1(OnReady, void(HttpSessionTask *session_task));
    MOCK_METHOD2(OnData, void(HttpSessionTask *session_task, HttpSessionTaskData *data));
    MOCK_METHOD2(OnDataFinish, void(HttpSessionTask *session_task, int err_code));

    void UpdateReceiveSize(HttpSessionTask *session_task, HttpSessionTaskData *data) {
        if (data->type == kReadDataTypeBody) {
            received_size_ += data->size;
        }
    }

public:
    size_t received_size_;
};


TEST(SessionReadTaskTest, Listener) {

    MockConnectionListener *mock_listener = new MockConnectionListener();

    HttpSessionReadTask *read_session_task1 = new HttpSessionReadTask("https://www.baidu.com", 0, 0);
    read_session_task1->setListener(mock_listener);
    ASSERT_EQ(read_session_task1->listener(), mock_listener);

    //call OnDataFinish 1 time
    EXPECT_CALL(*mock_listener, OnDataFinish(_, _)).Times(1);

    //call OnData when receive data
    ON_CALL(*mock_listener, OnData(_, _))
            .WillByDefault(Invoke(mock_listener,&MockConnectionListener::UpdateReceiveSize));
    ASSERT_EQ(mock_listener->received_size_, read_session_task1->received_size());

    read_session_task1->SyncRead();

    delete(mock_listener);
    delete(read_session_task1);
}

TEST(SessionReadTaskTest, EffectiveUrl) {
    HttpSessionReadTask *read_session_task = new HttpSessionReadTask("https://t.cn", 0, 0);
    read_session_task->SyncRead();

    std::cout<< read_session_task->effective_url_<< std::endl;
}

TEST(SessionReadTaskTest, Retry) {
    HttpSessionReadTask *read_session_task = new HttpSessionReadTask("http://unavailable.zdzhong.com/", 0, 0);
    read_session_task->set_retry_count(2);
    read_session_task->SyncRead();

    ASSERT_EQ(2, read_session_task->retry_count());
    ASSERT_EQ(read_session_task->retry_count(), read_session_task->request_count_);
}


