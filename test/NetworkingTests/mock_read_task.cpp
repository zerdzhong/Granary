//
// Created by zhongzhendong on 2018/11/10.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "http_session_read_task.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::AtLeast;
using ::testing::NiceMock;

class RandomDataHttpReadTask : public HttpSessionReadTask {
public:
    RandomDataHttpReadTask(std::string url, size_t offset, size_t length)
            : HttpSessionReadTask(url, offset, length)
    {

    }

    virtual void OnData(char *data, size_t size, ReadDataType type) override {
        std::cout<<"RandomDataHttpReadTask Mocked"<<std::endl;
        HttpSessionReadTask::OnData(data, size, type);
    }
};

class MockHttpReadTask : public HttpSessionReadTask {
public:
    MOCK_METHOD3(OnData, void(char *, size_t , ReadDataType));

    void DelegateTo(RandomDataHttpReadTask *mocked_task) {
        ON_CALL(*this, OnData(_, _, _)).
        WillByDefault(Invoke(mocked_task, &RandomDataHttpReadTask::OnData));
    }
};
