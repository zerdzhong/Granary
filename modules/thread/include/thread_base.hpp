//
// Created by zhongzhendong on 2018/8/2.
//

#ifndef GRANARY_THREAD_BASE_HPP
#define GRANARY_THREAD_BASE_HPP

#include <thread>
#include <mutex>
#include <string>

class ThreadBase {
public:
    int Start();
    int Join();
    int Quit();

    bool isAlive();

    virtual void run() = 0;
    virtual ~ThreadBase();
    ThreadBase();

    void setThreadName(std::string name);

protected:
    static void* start_func(void* arg);
    void setIsAlive(bool is_alive);

private:
    std::mutex mutex_;
    std::thread thread_;
    bool is_alive_;
    std::string thread_name_;
};


#endif //GRANARY_THREAD_BASE_HPP
