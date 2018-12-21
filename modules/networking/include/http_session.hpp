//
// Created by zhongzhendong on 2018/8/4.
//

#ifndef GRANARY_HTTP_CONNECTION_HPP
#define GRANARY_HTTP_CONNECTION_HPP

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "http_session_task.hpp"

class HttpSessionThread;
class HttpSessionReadTask;
class HttpSessionConfig;
class HttpMultiCurlWrapper;

class HttpSession : private HttpSessionTaskListener {
public:

    HttpSession();
    ~HttpSession();

    HttpSessionReadTask* ReadTask(std::string url);
    HttpSessionReadTask* ReadTaskWithInfo(std::string url, size_t offset, size_t length);

    void CancelTask(HttpSessionTask *task);

    void Start();
    void runInternal();

    void setListener(HttpSessionTaskListener& listener);
    HttpSessionTaskListener* listener();

    void setTaskAutoDelete(bool auto_delete);
    void setSessionConfig(std::shared_ptr<HttpSessionConfig> session_config);
    std::shared_ptr<HttpSessionConfig> sessionConfig();

    static std::string CurlInfo();
    static bool SupportSSL();

private:
    //callback
    void OnReady(HttpSessionTask *session_task) override;
    void OnData(HttpSessionTask *session_task, HttpSessionTaskData *read_data) override;
    void OnDataFinish(HttpSessionTask *session_task, int err_code) override;

    size_t requestPendingTasks();
    void handleCurlMessage();
    void handleTaskFinish(HttpSessionReadTask *task, int curl_code);

    void clearFinishedTask();

private:
    std::unique_ptr<HttpSessionThread> thread_loop_;
    std::vector<HttpSessionReadTask *> pending_tasks_;
    std::vector<HttpSessionReadTask *> running_tasks_;
    std::vector<HttpSessionReadTask *> finished_tasks_;

    std::recursive_mutex tasks_mutex_;
    std::mutex task_cond_mutex_;
    std::condition_variable task_cond_;

    std::unique_ptr<HttpMultiCurlWrapper> multi_curl_;
    int  is_handle_running_;
    bool task_auto_delete_;

    HttpSessionTaskListener* listener_;
    std::shared_ptr<HttpSessionConfig> session_config_;
};


#endif //GRANARY_HTTP_CONNECTION_HPP
