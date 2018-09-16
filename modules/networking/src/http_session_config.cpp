//
// Created by zhongzhendong on 2018/9/13.
//

#include <http_session_config.hpp>

#include "http_session_config.hpp"

http_session_config* http_session_config::DefaultSessionConfig() {
    http_session_config *defaultConfig = new http_session_config();
    return defaultConfig;
}

http_session_config::http_session_config() {

}

http_session_config::~http_session_config() {

}

#pragma mark- Setter
void http_session_config::SetAdditionalHeaders(std::vector<std::string> additional_headers) {
    additional_headers_ = additional_headers;
}

void http_session_config::SetRequestTimeout(time_interval request_timeout) {
    request_timeout_ = request_timeout;
}

void http_session_config::SetUrlResolve(std::map<std::string, std::set<std::string>> url_resolves) {
    url_resolves_ = url_resolves;
}

#pragma mark- Getter

time_interval http_session_config::RequestTimeout() {
    return request_timeout_;
}

std::vector<std::string> http_session_config::AdditionalHeaders() {
    return additional_headers_;
}

std::map<std::string, std::set<std::string>> http_session_config::UrlResolves() {
    return url_resolves_;
}

void http_session_config::AddResolve(std::string url, std::set<std::string> ip_addrs) {
    if (url.empty() || ip_addrs.size() <= 0) {
        return;
    }

    url_resolves_[url] = ip_addrs;
}
