#pragma once

#include <httplib.h>

#include "../data_strategy.hpp"
#include "../market_data.hpp"

namespace cppshares::data::providers {

// 网易财经API提供者
class NeteaseProvider : public DataProvider {
public:
    NeteaseProvider();
    ~NeteaseProvider() override = default;

    // DataProvider 接口实现
    std::optional<MarketTick> get_realtime_quote(const Symbol& symbol) override;

    std::vector<OHLCV> get_kline_data(const Symbol& symbol,
                                      KlinePeriod period = KlinePeriod::DAY_1,
                                      int limit = 100) override;

    std::string get_name() const override { return "NeteaseFinance"; }
    int get_priority() const override { return 5; }      // 高优先级
    int get_rate_limit() const override { return 120; }  // 每分钟120次

    bool health_check() override;

private:
    static constexpr const char* BASE_URL = "api.money.126.net";

    httplib::Client client_;

    // 辅助方法
    std::optional<MarketTick> parse_realtime_response(const std::string& response,
                                                      const Symbol& symbol);
    std::vector<OHLCV> parse_kline_response(const std::string& response, const Symbol& symbol);
    std::string convert_to_netease_symbol(const Symbol& symbol);
};

}  // namespace cppshares::data::providers