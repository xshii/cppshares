#include <iostream>

#include "cppshares/utils/logger.hpp"
#include "cppshares/utils/math.hpp"

int main() {
    // 初始化混合日志系统
    cppshares::utils::Logger::initialize();

    // 文本日志示例
    cppshares::utils::Logger::info("CppShares Quantitative Trading System Starting...");
    cppshares::utils::Logger::info("C++ Standard: {}", __cplusplus);

    // 注册一些测试符号
    auto& logger = cppshares::utils::Logger::instance();
    logger.register_symbol(1001, "AAPL");
    logger.register_symbol(1002, "GOOGL");
    logger.register_symbol(1003, "MSFT");

    // 二进制日志示例 - 模拟市场数据
    logger.log_market_data(1001, 150.25, 1000, true);   // AAPL买单
    logger.log_market_data(1002, 2800.50, 500, false);  // GOOGL卖单
    logger.log_market_data(1003, 320.75, 2000, true);   // MSFT买单

    // 订单日志示例
    logger.log_order(12345, 1001, 150.30, 100, true, 0);  // 新订单
    logger.log_order(12345, 1001, 150.30, 100, true, 1);  // 成交

    // 策略信号示例
    logger.log_strategy_signal(101, 1001, 0, 85, 155.0, 200);  // 买入信号

    // 数学工具测试
    std::vector<double> prices = {150.0, 151.5, 149.8, 152.3, 150.9, 153.1};
    std::vector<double> returns;

    for (size_t i = 1; i < prices.size(); ++i) {
        returns.push_back(cppshares::utils::calculate_returns(prices[i], prices[i - 1]));
    }

    double volatility = cppshares::utils::calculate_volatility(returns);
    double rsi = cppshares::utils::calculate_rsi(prices);
    auto bollinger = cppshares::utils::calculate_bollinger_bands(prices, 5);

    cppshares::utils::Logger::info("Calculated volatility: {:.6f}", volatility);
    cppshares::utils::Logger::info("RSI: {:.2f}", rsi);
    cppshares::utils::Logger::info("Bollinger Bands - Upper: {:.2f}, Middle: {:.2f}, Lower: {:.2f}",
                                   bollinger.upper,
                                   bollinger.middle,
                                   bollinger.lower);

    // 刷新所有日志缓冲区
    logger.flush();

    cppshares::utils::Logger::info("System initialized successfully");
    return 0;
}