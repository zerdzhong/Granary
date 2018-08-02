//
// Created by zhongzhendong on 2018/8/2.
//

#include "thread_base.hpp"
#include <stdio.h>

void* ThreadBase::start_func(void *arg) {
    ThreadBase *ptr = (ThreadBase *)arg;
    ptr->run();
}

int ThreadBase::Start() {

    if (pthread_create(&tid_, NULL, start_func, this) != 0) {
        printf("create a new thread failed!\n");
        return -1;
    }

    is_alive = true;
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
    is_alive = false;
    return 0;
}

pthread_t ThreadBase::getTid() {
    return tid_;
}

bool ThreadBase::isAlive() {
    return is_alive;
}

ThreadBase::~ThreadBase() {
    if (isAlive()) {
        pthread_kill(tid_, 0);
    }

}