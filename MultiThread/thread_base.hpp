//
// Created by zhongzhendong on 2018/8/2.
//

#ifndef GRANARY_THREAD_BASE_HPP
#define GRANARY_THREAD_BASE_HPP

#include <pthread.h>

class ThreadBase {
public:
    int Start();
    int Join();
    int Quit();

    pthread_t getTid();
    bool isAlive();

    virtual void run() = 0;
    virtual ~ThreadBase();

private:
    static void* start_func(void* arg);
    void setIsAlive(bool is_alive);

private:
    pthread_rwlock_t rwlock_;
    pthread_t tid_;
    bool is_alive_;
};


#endif //GRANARY_THREAD_BASE_HPP
