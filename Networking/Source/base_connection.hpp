//
// Created by zhongzhendong on 2018/8/11.
//

#ifndef GRANARY_BASE_CONNECTION_HPP
#define GRANARY_BASE_CONNECTION_HPP

#include <stdio.h>

typedef enum {
    kReadDataTypeBody = 0,
    kReadDataTypeHeader,
} ReadDataType;

typedef struct {
    char *data;
    size_t offset;
    size_t size;
    ReadDataType type;
} ConnectionReadData;

static ConnectionReadData* NewConnectionReadData(char* data, size_t offset, size_t size, ReadDataType type)
{
    auto *read_data = new ConnectionReadData();
    read_data->data = data;
    read_data->offset = offset;
    read_data->size = size;
    read_data->type = type;

    return read_data;
}

class BaseConnection;

class ConnectionListener {
public:
    virtual void OnReady(BaseConnection *connection);
    virtual void OnData(BaseConnection *connection, ConnectionReadData *read_data);
    virtual void OnDataFinish(BaseConnection *connection, int err_code);
};

class BaseConnection {
public:
    BaseConnection();
    void registListener(ConnectionListener *listener);
    ConnectionListener* listener();

    size_t request_start();
    size_t request_size();
    size_t received_size();

protected:
    ConnectionListener* listener_;
    size_t request_start_;
    size_t request_size_;
    size_t received_size_;
};


#endif //GRANARY_BASE_CONNECTION_HPP
