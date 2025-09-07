#include "cppshares/core/engine.hpp"
#include "cppshares/utils/logger.hpp"

namespace cppshares::core {

void TradingEngine::initialize() {
    cppshares::utils::Logger::info("TradingEngine initializing...");
    // TODO: Initialize trading engine
}

void TradingEngine::start() {
    if (is_running_) {
        return;
    }
    
    cppshares::utils::Logger::info("TradingEngine starting...");
    is_running_ = true;
    // TODO: Start trading engine
}

void TradingEngine::stop() {
    if (!is_running_) {
        return;
    }
    
    cppshares::utils::Logger::info("TradingEngine stopping...");
    is_running_ = false;
    // TODO: Stop trading engine
}

} // namespace cppshares::core