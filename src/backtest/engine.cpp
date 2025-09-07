#include "cppshares/backtest/engine.hpp"

#include "cppshares/utils/logger.hpp"

namespace cppshares::backtest {

void BacktestEngine::set_time_range(const std::chrono::system_clock::time_point& start,
                                    const std::chrono::system_clock::time_point& end) {
    start_time_ = start;
    end_time_ = end;
    configured_ = true;

    cppshares::utils::Logger::info("Backtest time range configured");
}

void BacktestEngine::run_backtest() {
    if (!configured_) {
        cppshares::utils::Logger::error("Backtest not configured");
        return;
    }

    cppshares::utils::Logger::info("Running backtest...");
    // TODO: Implement backtest logic
}

void BacktestEngine::generate_report() {
    cppshares::utils::Logger::info("Generating backtest report...");
    // TODO: Generate backtest report
}

}  // namespace cppshares::backtest