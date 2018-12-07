//
// Created by zhongzhendong on 2018/8/11.
//

#ifndef GRANARY_BASE_CONNECTION_HPP
#define GRANARY_BASE_CONNECTION_HPP

#include <cstdio>
#include <memory>

typedef enum {
    kReadDataTypeBody = 0,
    kReadDataTypeHeader,
} ReadDataType;

typedef struct HttpSessionTaskData {
    char *data;
    size_t offset;
    size_t size;
    ReadDataType type;

    HttpSessionTaskData(char *data_para, size_t offset_para, size_t size_para, ReadDataType type_para):
    data(data_para),
    offset(offset_para),
    size(size_para),
    type(type_para)
    {

    }

} HttpSessionTaskData;

static std::unique_ptr<HttpSessionTaskData> SessionTaskData(char *data, size_t offset, size_t size, ReadDataType type)
{
    return std::make_unique<HttpSessionTaskData>(data, offset, size, type);
}

class HttpSessionTask;

class HttpSessionTaskListener {
public:
    virtual void OnReady(HttpSessionTask *session_task) = 0;
    virtual void OnData(HttpSessionTask *session_task, HttpSessionTaskData *read_data) = 0;
    virtual void OnDataFinish(HttpSessionTask *session_task, int err_code) = 0;
};

class HttpSessionTask {
public:
    HttpSessionTask();
    void setListener(HttpSessionTaskListener& listener);
    HttpSessionTaskListener* listener();

    virtual size_t request_start();
    virtual size_t request_size();
    virtual size_t received_size();

protected:
    virtual void OnData(char *data, size_t size, ReadDataType type) = 0;
    virtual void OnProgress(double progress) = 0;

protected:
    HttpSessionTaskListener* listener_;
    size_t request_start_;
    size_t request_size_;
    size_t received_size_;
};


#endif //GRANARY_BASE_CONNECTION_HPP
