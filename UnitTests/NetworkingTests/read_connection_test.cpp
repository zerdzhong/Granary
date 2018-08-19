//
// Created by zhongzhendong on 2018/8/11.
//

#include "gtest/gtest.h"

#define private public
#define protected public

#include "http_read_connection.hpp"
#include <curl/curl.h>

#undef private
#undef protected


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


TEST(ReadConnectionTest, initWithValidURLs) {

    for (int i = 0; i < TEST_VALID_URLS_COUNT; ++i) {
        HttpReadConnection *read_connection = new HttpReadConnection(TEST_VALID_URLS[i], i, i);

        ASSERT_NE(nullptr, read_connection);
        ASSERT_EQ(read_connection->url(), TEST_VALID_URLS[i]);
        ASSERT_EQ(read_connection->request_start(), i);
        ASSERT_EQ(read_connection->request_size(), i);
        ASSERT_NE(read_connection->handle(), nullptr);

        delete(read_connection);
    }
}


TEST(ReadConnectionTest, initWithInvalidURLs) {

    for (auto &url : TEST_INVALID_URLS) {
        HttpReadConnection *read_task = new HttpReadConnection(url, 0, 0);

        ASSERT_NE(nullptr, read_task);

        delete(read_task);
    }
}

TEST(ReadConnectionTest, SyncRead) {
    HttpReadConnection *read_connection1 = new HttpReadConnection("https://www.baidu.com", 0, 0);
    read_connection1->SyncRead();
    ASSERT_GT(read_connection1->received_size(), 0);

    HttpReadConnection *read_connection2 = new HttpReadConnection("https://www.baidu.com", 0, 0);
    read_connection2->SyncRead();
    ASSERT_GT(read_connection2->received_size(), 0);

    ASSERT_EQ(read_connection1->received_size(), read_connection2->received_size());

    delete(read_connection1);
    delete(read_connection2);
}

class TestConnectionListener : public ConnectionListener {
public:
    TestConnectionListener():received_size_(0), call_finish_(false){

    }
    void OnReady(BaseConnection *connection) override {

    }
    void OnData(BaseConnection *connection, ConnectionReadData *read_data) override {
        if (kReadDataTypeBody == read_data->type) {
            received_size_ += read_data->size;
        }
    }
    void OnDataFinish(BaseConnection *connection, int err_code) override {
        call_finish_ = true;
    }

public:
    size_t received_size_;
    bool call_finish_;
};

TEST(ReadConnectionTest, Listener) {

    TestConnectionListener *listener = new TestConnectionListener();

    HttpReadConnection *read_connection1 = new HttpReadConnection("https://www.baidu.com", 0, 0);
    read_connection1->setListener(listener);

    read_connection1->SyncRead();

    ASSERT_EQ(read_connection1->listener(), listener);
    ASSERT_EQ(listener->received_size_, read_connection1->received_size());
    ASSERT_EQ(true, listener->call_finish_);

    delete(listener);
    delete(read_connection1);
}

TEST(ReadConnectionTest, EffectiveUrl) {
    HttpReadConnection *read_connection = new HttpReadConnection("https://t.cn", 0, 0);
    read_connection->SyncRead();

    std::cout<< read_connection->effective_url_<< std::endl;
}

TEST(ReadConnectionTest, Retry) {
    HttpReadConnection *read_connection = new HttpReadConnection("https://www.google.com", 0, 0);
    read_connection->set_retry_count(2);
    read_connection->SyncRead();

    ASSERT_EQ(2, read_connection->retry_count());
    ASSERT_EQ(read_connection->retry_count(), read_connection->request_count_);
}


