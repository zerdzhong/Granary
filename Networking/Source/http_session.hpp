//
// Created by zhongzhendong on 2018/8/4.
//

#ifndef GRANARY_HTTP_CONNECTION_HPP
#define GRANARY_HTTP_CONNECTION_HPP

#include <string>
#include <vector>
#include <pthread.h>
#include "http_session_task.hpp"

class HttpSessionThread;
class HttpSessionReadTask;

typedef void CURLM;

class HttpSession : private HttpSessionTaskListener {
public:

    HttpSession();
    ~HttpSession();

    HttpSessionReadTask* ReadTask(std::string url);
    HttpSessionReadTask* ReadTaskWithInfo(std::string url, size_t offset, size_t length);

    void CancelTask(HttpSessionTask *task);

    void Start();
    void runInternal();

    void setListener(HttpSessionTaskListener *listener);
    HttpSessionTaskListener* listener();

    void setTaskAutoDelete(bool auto_delete);

private:
    //callback
    void OnReady(HttpSessionTask *session_task) override;
    void OnData(HttpSessionTask *session_task, HttpSessionTaskData *read_data) override;
    void OnDataFinish(HttpSessionTask *session_task, int err_code) override;

    size_t requestPendingTasks();
    void handleCurlMessage();
    void handleTaskFinish(HttpSessionReadTask *task, int curl_code);

private:
    HttpSessionThread *thread_;
    std::vector<HttpSessionReadTask *> pending_tasks_;
    std::vector<HttpSessionReadTask *> running_tasks_;
    std::vector<HttpSessionReadTask *> undelete_finish_tasks_;

    pthread_mutex_t tasks_mutex_;
    pthread_cond_t task_cond_;

    CURLM *curl_multi_handle_;
    int  is_handle_running_;
    bool task_auto_delete_;

    HttpSessionTaskListener* listener_;
};


#endif //GRANARY_HTTP_CONNECTION_HPP
