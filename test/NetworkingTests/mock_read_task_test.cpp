//
// Created by zhongzhendong on 2018/11/12.
//

#include "mock_read_task.cpp"

using namespace std;

TEST(MockTest, OutOfRange) {
    auto mock_task = make_unique<MockHttpReadTask>();
    auto out_range_task = make_unique<RandomDataHttpReadTask>("http://www.baidu.com",0, 8);

    mock_task->DelegateTo(out_range_task.get());

    out_range_task->SyncRead();
}