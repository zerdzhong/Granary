//
// Created by zhongzhendong on 2018/10/23.
//

#ifndef GRANARY_THREAD_HPP
#define GRANARY_THREAD_HPP

#include <string>
#include <mutex>
#include <thread>

class Thread {
public:
    int Start();
    void Join();

    bool isAlive();
    void Quit();

    virtual void run() {};
    virtual ~Thread();
    Thread();

    void setThreadName(std::string name);
    std::thread::id getThreadId();

protected:
    static void* start_func(void* arg);
    void setIsAlive(bool is_alive);

private:
    std::mutex mutex_;
    std::thread thread_;
    bool is_alive_;
    std::string thread_name_;
};


#endif //GRANARY_THREAD_HPP
