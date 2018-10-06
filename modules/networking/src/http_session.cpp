#include <utility>

//
// Created by zhongzhendong on 2018/8/4.
//

#include "http_session.hpp"
#include "http_session_read_task.hpp"
#include "thread_base.hpp"
#include <curl.h>
#include <sstream>
#include <http_session.hpp>
#include <algorithm>
#include <chrono>
#if defined(__APPLE__)
#include <pthread.h>
#elif defined(__linux__)
#include <pthread.h>
#endif

using namespace std::chrono_literals;

#pragma mark- HttpSessionThread

class HttpSession;

class HttpSessionThread: public ThreadBase {
public:
    explicit HttpSessionThread(HttpSession *session):session_(session) {}
    ~HttpSessionThread() {
        if (isAlive()) {
            setIsAlive(false);
            Join();
        }
    }
    void run() override;

private:
    HttpSession *session_;
};

void HttpSessionThread::run() {

#if defined(__APPLE__)
    pthread_setname_np("HttpSessionThread");
#elif defined(__linux__)
    pthread_setname_np(pthread_self(), "HttpSessionThread");
#endif

    while (isAlive() && session_) {
        session_->runInternal();
    }
}

#pragma mark- HttpSession

#pragma mark- Life cycle

HttpSession::HttpSession():
is_handle_running_(0),
listener_(nullptr),
task_auto_delete_(true),
session_config_(nullptr)
{

    thread_ = new HttpSessionThread(this);

}

HttpSession::~HttpSession() {

    if (thread_) {
        delete thread_;
        thread_ = nullptr;
    }

    if (!task_auto_delete_) {
        clearFinishedTask();
    }
}

void HttpSession::clearFinishedTask() {
    if (task_auto_delete_ && !finished_tasks_.empty() ) {
        return;
    }

    std::lock_guard<std::recursive_mutex> lock_guard(tasks_mutex_);

    for (auto task : finished_tasks_) {
        delete task;
    }
    finished_tasks_.clear();

}

#pragma mark- Public

void HttpSession::Start() {

    curl_global_init(CURL_GLOBAL_ALL);
    curl_multi_handle_ = curl_multi_init();

    thread_->Start();
}

HttpSessionReadTask* HttpSession::ReadTask(std::string url) {
    return ReadTaskWithInfo(std::move(url), 0, 0);
}

HttpSessionReadTask* HttpSession::ReadTaskWithInfo(std::string url, size_t offset, size_t length){
    std::lock_guard<std::recursive_mutex> lock_guard(tasks_mutex_);

    HttpSessionReadTask *read_task = new HttpSessionReadTask(std::move(url), offset, length);
    read_task->setListener(this);
    read_task->setSessionConfig(session_config_);
    pending_tasks_.push_back(read_task);

    task_cond_.notify_one();

    return read_task;
}

void HttpSession::CancelTask(HttpSessionTask *task) {
    if (nullptr != dynamic_cast<HttpSessionReadTask *>(task)) {
        auto read_task = dynamic_cast<HttpSessionReadTask *>(task);

        if(!finished_tasks_.empty() && std::find(finished_tasks_.begin(), finished_tasks_.end(),
                read_task) != finished_tasks_.end())
        {
            return;
        }

        read_task->Cancel();
    }
}

void HttpSession::setListener(HttpSessionTaskListener *listener) {
    listener_ = listener;
}

HttpSessionTaskListener* HttpSession::listener() {
    return listener_;
}

void HttpSession::setTaskAutoDelete(bool auto_delete) {
    task_auto_delete_ = auto_delete;
}

void HttpSession::setSessionConfig(HttpSessionConfig *session_config) {
    session_config_ = session_config;
}

HttpSessionConfig* HttpSession::sessionConfig() {
    return session_config_;
}

#pragma mark- private

void HttpSession::runInternal() {
    requestPendingTasks();

    curl_multi_perform(curl_multi_handle_, &is_handle_running_);

    handleCurlMessage();

    int fd_num = 0;
    int res = curl_multi_wait(curl_multi_handle_, nullptr, 0, 1000, &fd_num);

    if(res != CURLM_OK) {
        fprintf(stderr, "error: curl_multi_wait() returned %d\n", res);
    }
}

size_t HttpSession::requestPendingTasks() {
    std::lock_guard<std::recursive_mutex> lock_guard(tasks_mutex_);
    std::unique_lock<std::mutex> lock(task_cond_mutex_);

    size_t  request_count = 0;

    if (pending_tasks_.empty() && running_tasks_.empty()) {
        auto now = std::chrono::system_clock::now();
        task_cond_.wait_until(lock, now + 500ms);
    }

    while (!pending_tasks_.empty()) {
        HttpSessionReadTask *read_task = pending_tasks_.back();

        curl_multi_add_handle(curl_multi_handle_, read_task->handle());

        running_tasks_.push_back(read_task);
        pending_tasks_.pop_back();
        request_count ++;
    }

    for (auto iterator = running_tasks_.begin();
         iterator != running_tasks_.end();) {

        auto read_task = *iterator;

        if (read_task->isStopped()) {
            handleTaskFinish(read_task, CURLE_ABORTED_BY_CALLBACK);
        } else {
            iterator++;
        }
    }

    return request_count;
}


void HttpSession::handleCurlMessage() {
    CURLMsg *msg= nullptr;
    int msgs_left=0;

    while ((msg = curl_multi_info_read(curl_multi_handle_, &msgs_left))) {
        if (msg->msg == CURLMSG_DONE) {
            CURL *easy_handle = msg->easy_handle;
            //get task
            HttpSessionReadTask *read_task = nullptr;
            curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &read_task);

            //handle finish
            int curl_code = msg->data.result;
            handleTaskFinish(read_task, curl_code);
        } else {
            fprintf(stderr, "error: after curl_multi_info_read(), CURLMsg=%d\n", msg->msg);
        }
    }
}

void HttpSession::handleTaskFinish(HttpSessionReadTask *task, int curl_code) {
    std::lock_guard<std::recursive_mutex> lock_guard(tasks_mutex_);

    //remove from runing tasks
    curl_multi_remove_handle(curl_multi_handle_, task->handle());
    running_tasks_.erase(std::remove(running_tasks_.begin(), running_tasks_.end(), task), running_tasks_.end());

    if (task->ShouldRetry(curl_code)) {
        //retry
        task->RetryTask();
        pending_tasks_.push_back(task);
    } else {
        task->ReadConnectionFinished(curl_code);
        if (task_auto_delete_) {
            delete task;
        } else {
            finished_tasks_.push_back(task);
        }
    }
}

#pragma mark- HttpSessionTaskListener

void HttpSession::OnReady(HttpSessionTask *session_task) {
    if (nullptr != listener_) {
        listener_->OnReady(session_task);
    }
}

void HttpSession::OnData(HttpSessionTask *session_task, HttpSessionTaskData *read_data) {
    if (nullptr != listener_) {
        listener_->OnData(session_task, read_data);
    }
}

void HttpSession::OnDataFinish(HttpSessionTask *session_task, int err_code) {
    if (nullptr != listener_) {
        listener_->OnDataFinish(session_task, err_code);
    }
}

std::string HttpSession::CurlInfo() {

    curl_version_info_data *info_data = curl_version_info(CURLVERSION_NOW);

    std::ostringstream stringStream;
//    stringStream<< "libcurl version: "  << info_data->version
//                << "\tSSL version: "    << info_data->ssl_version
//                << "\tlibz version: "   << info_data->libz_version;

    return stringStream.str();
}

bool HttpSession::SupportSSL() {
    curl_version_info_data *info_data = curl_version_info(CURLVERSION_NOW);

    return (info_data->ssl_version != nullptr);
}

