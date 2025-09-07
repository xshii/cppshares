#pragma once

#include "../data_strategy.hpp"
#include "../market_data.hpp"
#include <httplib.h>

namespace cppshares::data::providers {

// 腾讯财经API提供者
class TencentProvider : public DataProvider {
public:
    TencentProvider();
    ~TencentProvider() override = default;
    
    // DataProvider 接口实现
    std::optional<MarketTick> get_realtime_quote(const Symbol& symbol) override;
    
    std::vector<OHLCV> get_kline_data(
        const Symbol& symbol,
        KlinePeriod period = KlinePeriod::DAY_1,
        int limit = 100
    ) override;
    
    std::string get_name() const override { return "TencentFinance"; }
    int get_priority() const override { return 15; }  // 较低优先级
    int get_rate_limit() const override { return 60; } // 每分钟60次
    
    bool health_check() override;
    
private:
    static constexpr const char* BASE_URL = "qt.gtimg.cn";
    
    httplib::Client client_;
    
    // 辅助方法
    std::optional<MarketTick> parse_realtime_response(const std::string& response, const Symbol& symbol);
    std::vector<OHLCV> parse_kline_response(const std::string& response, const Symbol& symbol);
    std::string convert_to_tencent_symbol(const Symbol& symbol);
};

} // namespace cppshares::data::providers