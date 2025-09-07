#pragma once

#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>

namespace cppshares::utils {

// 计算简单收益率
static inline double calculate_returns(double current_price, double previous_price) noexcept {
    if (previous_price == 0.0) [[unlikely]] {
        return 0.0;
    }
    return (current_price - previous_price) / previous_price;
}

// 计算对数收益率
static inline double calculate_log_returns(double current_price, double previous_price) noexcept {
    if (previous_price <= 0.0 || current_price <= 0.0) [[unlikely]] {
        return 0.0;
    }
    return std::log(current_price / previous_price);
}

// 计算波动率
static inline double calculate_volatility(const std::vector<double>& returns) noexcept {
    if (returns.empty()) [[unlikely]] {
        return 0.0;
    }
    
    const double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    
    double variance = 0.0;
    for (const double ret : returns) {
        const double diff = ret - mean;
        variance += diff * diff;
    }
    variance /= returns.size();
    
    return std::sqrt(variance);
}

// 计算移动平均
template<typename T>
static inline double moving_average(const std::vector<T>& data, size_t window) noexcept {
    if (data.empty() || window == 0 || window > data.size()) [[unlikely]] {
        return 0.0;
    }
    
    const auto start = data.end() - window;
    return std::accumulate(start, data.end(), 0.0) / window;
}

// 计算标准差
static inline double standard_deviation(const std::vector<double>& data) noexcept {
    return calculate_volatility(data);
}

// 计算最大回撤
static inline double max_drawdown(const std::vector<double>& prices) noexcept {
    if (prices.size() < 2) [[unlikely]] {
        return 0.0;
    }
    
    double max_price = prices[0];
    double max_dd = 0.0;
    
    for (size_t i = 1; i < prices.size(); ++i) {
        max_price = std::max(max_price, prices[i]);
        const double drawdown = (max_price - prices[i]) / max_price;
        max_dd = std::max(max_dd, drawdown);
    }
    
    return max_dd;
}

// 计算夏普比率
static inline double sharpe_ratio(const std::vector<double>& returns, double risk_free_rate = 0.0) noexcept {
    if (returns.empty()) [[unlikely]] {
        return 0.0;
    }
    
    const double mean_return = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    const double excess_return = mean_return - risk_free_rate;
    const double volatility = calculate_volatility(returns);
    
    if (volatility == 0.0) [[unlikely]] {
        return 0.0;
    }
    
    return excess_return / volatility;
}

// RSI计算
static inline double calculate_rsi(const std::vector<double>& prices, size_t period = 14) noexcept {
    if (prices.size() <= period) [[unlikely]] {
        return 50.0; // 中性值
    }
    
    double gains = 0.0;
    double losses = 0.0;
    
    // 计算初始平均涨跌幅
    for (size_t i = prices.size() - period; i < prices.size() - 1; ++i) {
        const double change = prices[i + 1] - prices[i];
        if (change > 0) {
            gains += change;
        } else {
            losses -= change; // 转为正值
        }
    }
    
    gains /= period;
    losses /= period;
    
    if (losses == 0.0) [[unlikely]] {
        return 100.0;
    }
    
    const double rs = gains / losses;
    return 100.0 - (100.0 / (1.0 + rs));
}

// MACD计算结果
struct MACDResult {
    double macd;
    double signal;
    double histogram;
};

static inline MACDResult calculate_macd(const std::vector<double>& prices, 
                                       size_t fast_period = 12, 
                                       size_t slow_period = 26, 
                                       size_t signal_period = 9) noexcept {
    if (prices.size() < slow_period) [[unlikely]] {
        return {0.0, 0.0, 0.0};
    }
    
    // 简化版MACD计算（实际应该使用EMA）
    const double fast_ma = moving_average(prices, fast_period);
    const double slow_ma = moving_average(prices, slow_period);
    const double macd = fast_ma - slow_ma;
    
    // 这里简化signal线计算，实际应该是MACD的EMA
    const double signal = macd * 0.9; // 简化
    const double histogram = macd - signal;
    
    return {macd, signal, histogram};
}

// 布林带结果
struct BollingerBands {
    double upper;
    double middle;
    double lower;
};

static inline BollingerBands calculate_bollinger_bands(const std::vector<double>& prices, 
                                                      size_t period = 20, 
                                                      double std_dev_multiplier = 2.0) noexcept {
    if (prices.size() < period) [[unlikely]] {
        const double price = prices.empty() ? 0.0 : prices.back();
        return {price, price, price};
    }
    
    const double middle = moving_average(prices, period);
    
    // 计算标准差
    const auto start = prices.end() - period;
    double variance = 0.0;
    for (auto it = start; it != prices.end(); ++it) {
        const double diff = *it - middle;
        variance += diff * diff;
    }
    variance /= period;
    const double std_dev = std::sqrt(variance);
    
    const double band_width = std_dev_multiplier * std_dev;
    return {
        middle + band_width,  // upper
        middle,               // middle
        middle - band_width   // lower
    };
}

} // namespace cppshares::utils