#pragma once

#include "../data_strategy.hpp"
#include "../market_data.hpp"
#include "../data_types.hpp"
#include <httplib.h>
#include <nlohmann/json.hpp>

namespace cppshares::data::providers {

// 新浪财经API提供者
class SinaProvider : public DataProvider {
public:
    SinaProvider();
    ~SinaProvider() override = default;
    
    // DataProvider 接口实现
    std::optional<MarketTick> get_realtime_quote(const Symbol& symbol) override;
    
    std::vector<OHLCV> get_kline_data(
        const Symbol& symbol,
        KlinePeriod period = KlinePeriod::DAY_1,
        int limit = 100
    ) override;
    
    std::string get_name() const override { return "SinaFinance"; }
    int get_priority() const override { return 10; }  // 中等优先级
    int get_rate_limit() const override { return 100; } // 每分钟100次
    
    bool health_check() override;
    
private:
    static constexpr const char* BASE_URL = "hq.sinajs.cn";
    static constexpr const char* KLINE_URL = "money.finance.sina.com.cn";
    
    httplib::Client client_;
    httplib::Client kline_client_;
    
    // 辅助方法
    std::string format_symbol_for_sina(const Symbol& symbol);
    std::optional<MarketTick> parse_realtime_response(const std::string& response, const Symbol& symbol);
    std::vector<OHLCV> parse_kline_response(const std::string& response, const Symbol& symbol);
    std::string period_to_sina_format(KlinePeriod period);
};

} // namespace cppshares::data::providers