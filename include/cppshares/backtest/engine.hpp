#pragma once

#include <string>
#include <chrono>

namespace cppshares::backtest {

class BacktestEngine {
public:
    BacktestEngine() = default;
    ~BacktestEngine() = default;
    
    void set_time_range(const std::chrono::system_clock::time_point& start,
                       const std::chrono::system_clock::time_point& end);
    
    void run_backtest();
    void generate_report();
    
private:
    std::chrono::system_clock::time_point start_time_;
    std::chrono::system_clock::time_point end_time_;
    bool configured_ = false;
};

} // namespace cppshares::backtest