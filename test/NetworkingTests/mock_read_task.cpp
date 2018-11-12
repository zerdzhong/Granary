//
// Created by zhongzhendong on 2018/11/10.
//

#include "gmock/gmock.h"
#include "http_session_read_task.hpp"

class MockHttpReadTask : public HttpSessionReadTask {
public:
    MOCK_METHOD3(OnData, void(char *, size_t , ReadDataType));
};



