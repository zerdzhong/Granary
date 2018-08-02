//
// Created by zhongzhendong on 2018/8/2.
//

#include "gtest/gtest.h"

#include "thread_base.hpp"
#include "unistd.h"
#include <iostream>

using namespace std;

class ThreadTest1 : public ThreadBase
{
public:
    void run() override;
};


void ThreadTest1::run()
{
    while (isAlive())
    {
        cout << "Thread test 1 tid="<< getTid() << endl;
        sleep(1);
    }
}


class ThreadTest2 : public ThreadBase
{
public:
    void run() override;
};


void ThreadTest2::run()
{
    while (isAlive())
    {
        cout << "Thread test 2 tid="<< getTid() << endl;
        sleep(1);
    }
}


class ThreadTest3 : public ThreadBase
{
public:
    void run() override;

private:
    int count_;
};


void ThreadTest3::run()
{
    while (isAlive())
    {
        cout << "Thread test 3 tid="<< getTid() << endl;
        count_ ++;
    }
}

TEST(ThreadBaseTest, start) {
    ThreadBase *test1 = new ThreadTest1();
    ThreadBase *test2 = new ThreadTest2();
    ThreadBase *test3 = new ThreadTest3();

    test1->Start();
    test2->Start();
    test3->Start();

    sleep(2);

//    test1->Start();
//    test2->Start();

//    test1->Join();
//    test2->Join();

    delete test1;
    delete test2;
    delete test3;

    sleep(2);
}

