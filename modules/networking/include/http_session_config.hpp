//
// Created by zhongzhendong on 2018/9/13.
//

#ifndef GRANARY_HTTP_SESSION_CONFIG_HPP
#define GRANARY_HTTP_SESSION_CONFIG_HPP

#include <map>
#include <string>
#include <vector>
#include <set>

typedef double time_interval;

class HttpSessionConfig {
public:
    static HttpSessionConfig* DefaultSessionConfig();
    HttpSessionConfig();
    ~HttpSessionConfig();

    void SetRequestTimeout(time_interval request_timeout);
    time_interval RequestTimeout();

    void SetAdditionalHeaders(std::vector<std::string> additional_headers);
    std::vector<std::string> AdditionalHeaders();

    void SetUrlResolve(std::map<std::string, std::set<std::string>> url_resolves);
    void AddResolve(std::string url, std::set<std::string> ips);
    std::map<std::string, std::set<std::string>> UrlResolves();

private:
    time_interval request_timeout_;
    std::vector<std::string> additional_headers_;
    std::map<std::string, std::set<std::string>> url_resolves_;
};


#endif //GRANARY_HTTP_SESSION_CONFIG_HPP
