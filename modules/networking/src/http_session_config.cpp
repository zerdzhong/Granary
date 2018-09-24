//
// Created by zhongzhendong on 2018/9/13.
//

#include <http_session_config.hpp>

#include "http_session_config.hpp"

HttpSessionConfig* HttpSessionConfig::DefaultSessionConfig() {
    HttpSessionConfig *defaultConfig = new HttpSessionConfig();
    return defaultConfig;
}

HttpSessionConfig::HttpSessionConfig() {

}

HttpSessionConfig::~HttpSessionConfig() {

}

#pragma mark- Setter
void HttpSessionConfig::SetAdditionalHeaders(std::vector<std::string> additional_headers) {
    additional_headers_ = additional_headers;
}

void HttpSessionConfig::SetRequestTimeout(time_interval request_timeout) {
    request_timeout_ = request_timeout;
}

void HttpSessionConfig::SetUrlResolve(std::map<std::string, std::set<std::string>> url_resolves) {
    url_resolves_ = std::move(url_resolves);
}

#pragma mark- Getter

time_interval HttpSessionConfig::RequestTimeout() {
    return request_timeout_;
}

std::vector<std::string> HttpSessionConfig::AdditionalHeaders() {
    return additional_headers_;
}

std::map<std::string, std::set<std::string>> HttpSessionConfig::UrlResolves() {
    return url_resolves_;
}

void HttpSessionConfig::AddResolve(std::string url, std::set<std::string> ips) {
    if (url.empty() || ips.empty()) {
        return;
    }

    url_resolves_[url] = ips;
}
