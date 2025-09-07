#pragma once

namespace cppshares::core {

class TradingEngine {
public:
    TradingEngine() = default;
    ~TradingEngine() = default;
    
    void initialize();
    void start();
    void stop();
    
private:
    bool is_running_ = false;
};

} // namespace cppshares::core