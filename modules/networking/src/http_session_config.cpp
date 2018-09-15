//
// Created by zhongzhendong on 2018/9/13.
//

#include "http_session_config.hpp"

http_session_config& http_session_config::DefaultSessionConfig() {
    static http_session_config defaultConfig;
    return defaultConfig;
}

void http_session_config::SetRequestTimeout(time_interval request_timeout) {
    request_timeout_ = request_timeout;
}

time_interval http_session_config::RequestTimeout() {
    return request_timeout_;
}

http_session_config::http_session_config() {

}

http_session_config::~http_session_config() {

}
