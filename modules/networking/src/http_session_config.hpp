//
// Created by zhongzhendong on 2018/9/13.
//

#ifndef GRANARY_HTTP_SESSION_CONFIG_HPP
#define GRANARY_HTTP_SESSION_CONFIG_HPP

#include <map>
#include <string>

typedef double time_interval;

class http_session_config {
public:
    http_session_config* DefaultSessionConfig();
    http_session_config();
    ~http_session_config();

    void setRequestTimeout(time_interval request_timeout);
    void requestTimeout(time_interval request_timeout);

private:
    time_interval request_timeout_;
    std::map<std::string, std::string> http_additional_headers_;
};


#endif //GRANARY_HTTP_SESSION_CONFIG_HPP
