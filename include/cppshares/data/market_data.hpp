#pragma once

#include <string>
#include <chrono>
#include <vector>

namespace cppshares::data {

// 市场数据结构
struct MarketTick {
    std::string symbol;
    double price;
    uint64_t volume;
    double bid_price;
    double ask_price;
    uint64_t bid_volume;
    uint64_t ask_volume;
    double change_rate;  // 涨跌幅
    double change_amount; // 涨跌额
    std::chrono::system_clock::time_point timestamp;
};

// K线数据结构
struct OHLCV {
    std::string symbol;
    std::chrono::system_clock::time_point timestamp;
    double open;
    double high;
    double low;
    double close;
    uint64_t volume;
    double amount;  // 成交金额
};

} // namespace cppshares::data