//
// Created by zhongzhendong on 2018/8/11.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define private public
#define protected public

#include "http_session_read_task.hpp"

#undef private
#undef protected

#include "session_mock_listener.cpp"
#include <curl/curl.h>

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

static const char *TEST_INVALID_URLS[] = {
        "https:???",
        "http:/www.com",
        "ftp://www.zhihu.com",
        "",
};

static auto UNAVAILABLE_URL = "http://unavailable.zdzhong.com/a";

bool SupportSSL() {
    curl_version_info_data *info_data = curl_version_info(CURLVERSION_NOW);

    return (info_data->ssl_version != nullptr);
}

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

#if defined(__APPLE__)
    if (SupportSSL()) {
        HttpSessionReadTask *read_session_task1 = new HttpSessionReadTask("https://gensho.ftp.acc.umu.se/pub/gimp/gimp/v2.10/osx/gimp-2.10.4-x86_64.dmg", 0, 1024);
        read_session_task1->SyncRead();
        ASSERT_EQ(read_session_task1->received_size(), 1024);

        delete(read_session_task1);
    }
#endif

    HttpSessionReadTask *read_session_task2 = new HttpSessionReadTask("https://www.baidu.com", 0, 0);
    read_session_task2->SyncRead();
    ASSERT_GT(read_session_task2->received_size(), 0);
    delete(read_session_task2);
}

#if defined(__APPLE__)

TEST(SessionReadTaskTest, ReadRange) {
    HttpSessionReadTask *read_session_task = new HttpSessionReadTask("http://www.baidu.com", 0, 8);
    read_session_task->SyncRead();
    ASSERT_EQ(read_session_task->received_size(), 8);

    delete(read_session_task);
}

#endif

TEST(SessionReadTaskTest, ResultCode) {
    HttpSessionReadTask *read_session_task = new HttpSessionReadTask("http://www.baidu.com", 0, 0);
    HttpConnectionCode result = read_session_task->SyncRead();

    ASSERT_EQ(result, read_session_task->result_code_);
#if defined(__APPLE__)
    ASSERT_EQ(CONN_OK, read_session_task->result_code_);
#endif

    HttpSessionReadTask *read_session_task1 = new HttpSessionReadTask(UNAVAILABLE_URL, 0, 0);
    read_session_task1->SyncRead();

    ASSERT_EQ(HTTP_NOT_FOUND, read_session_task1->result_code_);

    delete(read_session_task);
}

class Helper {
public:
    Helper():received_size_(0){

    }

    void UpdateReceiveSize(HttpSessionTask *session_task, HttpSessionTaskData *data) {
        if (data->type == kReadDataTypeBody) {
            received_size_ += data->size;
        }
    }

public:
    size_t received_size_;
};


TEST(SessionReadTaskTest, Listener) {

    auto *mock_listener = new MockSessionTaskListener();
    auto *helper = new Helper();

    HttpSessionReadTask *read_session_task1 = new HttpSessionReadTask("https://www.baidu.com", 0, 0);
    read_session_task1->setListener(mock_listener);
    ASSERT_EQ(read_session_task1->listener(), mock_listener);

    //call OnDataFinish 1 time
    EXPECT_CALL(*mock_listener, OnDataFinish(_, _)).Times(1);
    //call OnDataFinish at least 1 time
    EXPECT_CALL(*mock_listener, OnData(_, _)).Times(AtLeast(1));

    //call OnData when receive data
    ON_CALL(*mock_listener, OnData(_, _))
            .WillByDefault(Invoke(helper, &Helper::UpdateReceiveSize));
    ASSERT_EQ(helper->received_size_, read_session_task1->received_size());

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
    HttpSessionReadTask *read_session_task = new HttpSessionReadTask(UNAVAILABLE_URL, 0, 0);
    read_session_task->set_retry_count(2);
    read_session_task->SyncRead();

    ASSERT_EQ(2, read_session_task->retry_count());
    ASSERT_EQ(read_session_task->retry_count(), 2);
    ASSERT_EQ(read_session_task->request_count_, 2);
}


