//
// Created by zhongzhendong on 2018/8/2.
//

#include "thread_base.hpp"
#include <stdio.h>

void* ThreadBase::start_func(void *arg) {
    ThreadBase *ptr = (ThreadBase *)arg;
    ptr->run();
    return nullptr;
}

int ThreadBase::Start() {

    if (pthread_create(&tid_, NULL, start_func, this) != 0) {
        printf("create a new thread failed!\n");
        return -1;
    }

    is_alive_ = true;
    return 0;
}

int ThreadBase::Join() {
    int ret = -1;

    printf("Join thread tid=%d\n", tid_);

    ret = pthread_join(tid_, NULL);

    if (0 != ret) {
        printf("Join thread fail, tid =%d\n", tid_);
    }

    return  ret;
}

int ThreadBase::Quit() {
    setIsAlive(false);
    return 0;
}

pthread_t ThreadBase::getTid() {
    return tid_;
}

bool ThreadBase::isAlive() {
    pthread_rwlock_rdlock(&rwlock_);
    bool is_alive = is_alive_;
    pthread_rwlock_unlock(&rwlock_);
    return is_alive;
}

void ThreadBase::setIsAlive(bool is_alive) {
    pthread_rwlock_wrlock(&rwlock_);
    is_alive_ = is_alive;
    pthread_rwlock_unlock(&rwlock_);
}

void ThreadBase::setThreadName(std::string name) {
    thread_name_ = name;
}


ThreadBase::~ThreadBase() {
    printf("Release ThreadBase, %p\n", this);

    if (isAlive()) {
        setIsAlive(false);
        pthread_join(tid_, nullptr);
    }

    pthread_rwlock_destroy(&rwlock_);
}

ThreadBase::ThreadBase() {
    pthread_rwlock_init(&rwlock_, nullptr);
}
