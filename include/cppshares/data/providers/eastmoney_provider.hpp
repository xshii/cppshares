#pragma once

#include <httplib.h>

#include <nlohmann/json.hpp>

#include "../data_strategy.hpp"
#include "../data_types.hpp"
#include "../market_data.hpp"
#include "eastmoney_fields.hpp"

namespace cppshares::data::providers {

// 东方财富API提供者
class EastMoneyProvider : public DataProvider {
public:
    EastMoneyProvider();
    ~EastMoneyProvider() override = default;

    // DataProvider 接口实现
    std::optional<MarketTick> get_realtime_quote(const Symbol& symbol) override;

    std::vector<OHLCV> get_kline_data(const Symbol& symbol,
                                      KlinePeriod period = KlinePeriod::DAY_1,
                                      int limit = 100) override;

    std::string get_name() const override { return "EastMoney"; }
    int get_priority() const override { return 1; }      // 最高优先级
    int get_rate_limit() const override { return 100; }  // 每分钟100次

    bool health_check() override;

private:
    static constexpr const char* BASE_URL = "push2.eastmoney.com";
    static constexpr const char* KLINE_URL = "push2his.eastmoney.com";

    httplib::Client client_;
    httplib::Client kline_client_;

    // 辅助方法
    std::string format_symbol_for_eastmoney(const Symbol& symbol);
    std::optional<MarketTick> parse_realtime_response(const std::string& response,
                                                      const Symbol& symbol);
    std::vector<OHLCV> parse_kline_response(const std::string& response, const Symbol& symbol);
    std::string period_to_eastmoney_format(KlinePeriod period);
};

}  // namespace cppshares::data::providers