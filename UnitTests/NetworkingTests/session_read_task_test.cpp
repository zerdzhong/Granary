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
        HttpSessionReadTask *read_connection = new HttpSessionReadTask(TEST_VALID_URLS[i], i, i);

        ASSERT_NE(nullptr, read_connection);
        ASSERT_EQ(read_connection->url(), TEST_VALID_URLS[i]);
        ASSERT_EQ(read_connection->request_start(), i);
        ASSERT_EQ(read_connection->request_size(), i);
        ASSERT_NE(read_connection->handle(), nullptr);

        delete(read_connection);
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
    HttpSessionReadTask *read_connection1 = new HttpSessionReadTask("https://www.baidu.com", 0, 0);
    read_connection1->SyncRead();
    ASSERT_GT(read_connection1->received_size(), 0);

    HttpSessionReadTask *read_connection2 = new HttpSessionReadTask("https://www.baidu.com", 0, 0);
    read_connection2->SyncRead();
    ASSERT_GT(read_connection2->received_size(), 0);

    ASSERT_EQ(read_connection1->received_size(), read_connection2->received_size());

    delete(read_connection1);
    delete(read_connection2);
}

class MockConnectionListener : public ConnectionListener {
public:
    MockConnectionListener():received_size_(0){

    }
    MOCK_METHOD1(OnReady, void(BaseConnection *connection));
    MOCK_METHOD2(OnData, void(BaseConnection *connection, ConnectionReadData *data));
    MOCK_METHOD2(OnDataFinish, void(BaseConnection *connection, int err_code));

    void UpdateReceiveSize(BaseConnection *connection, ConnectionReadData *data) {
        if (data->type == kReadDataTypeBody) {
            received_size_ += data->size;
        }
    }

public:
    size_t received_size_;
};


TEST(SessionReadTaskTest, Listener) {

    MockConnectionListener *mock_listener = new MockConnectionListener();

    HttpSessionReadTask *read_connection1 = new HttpSessionReadTask("https://www.baidu.com", 0, 0);
    read_connection1->setListener(mock_listener);
    ASSERT_EQ(read_connection1->listener(), mock_listener);

    //call OnDataFinish 1 time
    EXPECT_CALL(*mock_listener, OnDataFinish(_, _)).Times(1);

    //call OnData when receive data
    ON_CALL(*mock_listener, OnData(_, _))
            .WillByDefault(Invoke(mock_listener,&MockConnectionListener::UpdateReceiveSize));
    ASSERT_EQ(mock_listener->received_size_, read_connection1->received_size());

    read_connection1->SyncRead();

    delete(mock_listener);
    delete(read_connection1);
}

TEST(SessionReadTaskTest, EffectiveUrl) {
    HttpSessionReadTask *read_connection = new HttpSessionReadTask("https://t.cn", 0, 0);
    read_connection->SyncRead();

    std::cout<< read_connection->effective_url_<< std::endl;
}

TEST(SessionReadTaskTest, Retry) {
    HttpSessionReadTask *read_connection = new HttpSessionReadTask("http://unavailable.zdzhong.com/", 0, 0);
    read_connection->set_retry_count(2);
    read_connection->SyncRead();

    ASSERT_EQ(2, read_connection->retry_count());
    ASSERT_EQ(read_connection->retry_count(), read_connection->request_count_);
}


