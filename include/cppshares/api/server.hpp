#pragma once

#include <string>
#include <cstdint>

namespace cppshares::api {

class WebServer {
public:
    WebServer() = default;
    ~WebServer() = default;
    
    bool start(const std::string& host = "localhost", uint16_t port = 8080);
    void stop();
    bool is_running() const;
    
private:
    void setup_routes();
    
    bool running_ = false;
    std::string host_;
    uint16_t port_ = 0;
};

} // namespace cppshares::api