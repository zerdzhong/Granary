//
// Created by zhongzhendong on 2018/9/14.
//


#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define private public
#define protected public

#include "http_session_config.hpp"

#undef private
#undef protected

TEST(SessionConfigTest, Default)
{
    http_session_config config = http_session_config::DefaultSessionConfig();
    ASSERT_NE(nullptr, &config);
}
