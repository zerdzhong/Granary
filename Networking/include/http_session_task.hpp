//
// Created by zhongzhendong on 2018/8/11.
//

#ifndef GRANARY_BASE_CONNECTION_HPP
#define GRANARY_BASE_CONNECTION_HPP

#include <cstdio>

typedef enum {
    kReadDataTypeBody = 0,
    kReadDataTypeHeader,
} ReadDataType;

typedef struct {
    char *data;
    size_t offset;
    size_t size;
    ReadDataType type;
} HttpSessionTaskData;

static HttpSessionTaskData* NewSessionTaskData(char *data, size_t offset, size_t size, ReadDataType type)
{
    auto *read_data = new HttpSessionTaskData();
    read_data->data = data;
    read_data->offset = offset;
    read_data->size = size;
    read_data->type = type;

    return read_data;
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
    void setListener(HttpSessionTaskListener *listener);
    HttpSessionTaskListener* listener();

    virtual size_t request_start();
    virtual size_t request_size();
    virtual size_t received_size();

protected:
    HttpSessionTaskListener* listener_;
    size_t request_start_;
    size_t request_size_;
    size_t received_size_;
};


#endif //GRANARY_BASE_CONNECTION_HPP
