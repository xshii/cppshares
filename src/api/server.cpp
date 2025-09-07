#include "cppshares/api/server.hpp"

#include "cppshares/utils/logger.hpp"

namespace cppshares::api {

bool WebServer::start(const std::string& host, uint16_t port) {
    host_ = host;
    port_ = port;

    cppshares::utils::Logger::info("Starting web server on {}:{}", host_, port_);

    setup_routes();

    // TODO: Start httplib server
    running_ = true;
    return running_;
}

void WebServer::stop() {
    if (running_) {
        cppshares::utils::Logger::info("Stopping web server...");
        running_ = false;
        // TODO: Stop httplib server
    }
}

bool WebServer::is_running() const {
    return running_;
}

void WebServer::setup_routes() {
    // TODO: Setup HTTP routes with httplib
    cppshares::utils::Logger::info("Setting up API routes...");
}

}  // namespace cppshares::api