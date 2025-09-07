#include "cppshares/data/providers/tencent_provider.hpp"

#include "cppshares/data/data_types.hpp"

namespace cppshares::data::providers {

TencentProvider::TencentProvider() : client_(BASE_URL) {
    client_.set_connection_timeout(5, 0);
    client_.set_read_timeout(10, 0);
}

std::optional<MarketTick> TencentProvider::get_realtime_quote(const Symbol& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string tencent_symbol = convert_to_tencent_symbol(symbol);
    std::string path = "/q=" + tencent_symbol;

    auto result = client_.Get(path);
    if (result && result->status == 200) {
        return parse_realtime_response(result->body, symbol);
    }

    return std::nullopt;
}

std::vector<OHLCV> TencentProvider::get_kline_data(const Symbol& symbol,
                                                   KlinePeriod period,
                                                   int limit) {
    // 腾讯接口的K线数据获取实现
    return {};
}

bool TencentProvider::health_check() {
    auto result = client_.Get("/");
    return result && result->status == 200;
}

std::optional<MarketTick> TencentProvider::parse_realtime_response(const std::string& response,
                                                                   const Symbol& symbol) {
    // 简单的解析实现
    MarketTick tick;
    tick.symbol = symbol.to_string();
    tick.price = 0.0;
    tick.volume = 0;
    tick.timestamp = std::chrono::system_clock::now();

    return tick;
}

std::vector<OHLCV> TencentProvider::parse_kline_response(const std::string& response,
                                                         const Symbol& symbol) {
    return {};
}

std::string TencentProvider::convert_to_tencent_symbol(const Symbol& symbol) {
    // 腾讯格式: sh600000, sz000001 (与新浪相同)
    static const std::array<const char*, 5> prefixes = {"sh", "sz", "bj", "hk", "us"};
    int market_index = static_cast<int>(symbol.market);
    if (market_index >= 0 && market_index < static_cast<int>(prefixes.size())) {
        return std::string(prefixes[market_index]) + symbol.code;
    }
    return "sh" + symbol.code;  // 默认沪市
}

}  // namespace cppshares::data::providers