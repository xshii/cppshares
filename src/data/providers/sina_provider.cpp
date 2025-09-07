#include "cppshares/data/providers/sina_provider.hpp"

#include "cppshares/data/data_types.hpp"

namespace cppshares::data::providers {

SinaProvider::SinaProvider() : client_(BASE_URL), kline_client_(KLINE_URL) {
    // 设置基本配置
    client_.set_connection_timeout(5, 0);
    client_.set_read_timeout(10, 0);
    kline_client_.set_connection_timeout(5, 0);
    kline_client_.set_read_timeout(10, 0);
}

std::optional<MarketTick> SinaProvider::get_realtime_quote(const Symbol& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string sina_symbol = format_symbol_for_sina(symbol);
    std::string path = "/rn=xppzh&list=" + sina_symbol;

    auto result = client_.Get(path);
    if (result && result->status == 200) {
        return parse_realtime_response(result->body, symbol);
    }

    return std::nullopt;
}

std::vector<OHLCV> SinaProvider::get_kline_data(const Symbol& symbol,
                                                KlinePeriod period,
                                                int limit) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string sina_symbol = format_symbol_for_sina(symbol);
    std::string period_str = period_to_sina_format(period);
    std::string path = "/akdaily/cn/" + period_str + "/" + sina_symbol + ".js";

    auto result = kline_client_.Get(path);
    if (result && result->status == 200) {
        return parse_kline_response(result->body, symbol);
    }

    return {};
}

bool SinaProvider::health_check() {
    auto result = client_.Get("/");
    return result && result->status == 200;
}

std::string SinaProvider::format_symbol_for_sina(const Symbol& symbol) {
    return DataTypeUtils::to_sina_format(symbol);
}

std::optional<MarketTick> SinaProvider::parse_realtime_response(const std::string& response,
                                                                const Symbol& symbol) {
    // 简单的解析实现，实际中需要解析新浪的返回格式
    MarketTick tick;
    tick.symbol = symbol.to_string();
    tick.price = 0.0;
    tick.volume = 0;
    tick.timestamp = std::chrono::system_clock::now();

    return tick;
}

std::vector<OHLCV> SinaProvider::parse_kline_response(const std::string& response,
                                                      const Symbol& symbol) {
    // 简单的解析实现
    return {};
}

std::string SinaProvider::period_to_sina_format(KlinePeriod period) {
    static const std::array<const char*, 9> period_names = {
        "1min", "5min", "15min", "30min", "60min", "240min", "daily", "weekly", "monthly"};
    return period_names[static_cast<int>(period)];
}

}  // namespace cppshares::data::providers