//
// Created by zhongzhendong on 2018/9/2.
//

#include "gmock/gmock.h"
#include "http_session_read_task.hpp"

class MockSessionTaskListener : public HttpSessionTaskListener {
public:
    MOCK_METHOD1(OnReady, void(HttpSessionTask *session_task));
    MOCK_METHOD2(OnData, void(HttpSessionTask *session_task, HttpSessionTaskData *data));
    MOCK_METHOD2(OnDataFinish, void(HttpSessionTask *session_task, int err_code));
};

