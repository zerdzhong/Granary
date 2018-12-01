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

using namespace std;

TEST(SessionConfigTest, Default)
{
    std::shared_ptr<HttpSessionConfig> config = HttpSessionConfig::DefaultSessionConfig();
    ASSERT_NE(nullptr, config);
}

TEST(SessionConfigTest, RequestTimeout)
{
    std::shared_ptr<HttpSessionConfig> config = HttpSessionConfig::DefaultSessionConfig();
    config->SetRequestTimeout(1);
    ASSERT_EQ(config->RequestTimeout(), 1);
}

TEST(SessionConfigTest, Configs)
{
    std::shared_ptr<HttpSessionConfig> config = HttpSessionConfig::DefaultSessionConfig();
    config->SetRequestTimeout(1);
    ASSERT_EQ(config->RequestTimeout(), 1);

    vector<string> headers = {"header1:1","header2:2"};
    config->SetAdditionalHeaders(headers);
    ASSERT_EQ(config->AdditionalHeaders(), headers);
    ASSERT_EQ(config->AdditionalHeaders().size(), 2);

    map<string, set<string>> url_resolve = {{"www.baidu.com", {"8.8.8.8", "1.1.1.1"}}};
    config->SetUrlResolve(url_resolve);
    ASSERT_EQ(config->UrlResolves(), url_resolve);
    ASSERT_EQ(config->UrlResolves().size(), 1);

    config->AddResolve("www.google.com", {"8.8.8.8", "1.1.1.1"});
    ASSERT_EQ(config->UrlResolves().size(), 2);

}
