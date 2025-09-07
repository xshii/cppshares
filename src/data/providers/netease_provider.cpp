#include "cppshares/data/providers/netease_provider.hpp"

#include "cppshares/data/data_types.hpp"

namespace cppshares::data::providers {

NeteaseProvider::NeteaseProvider() : client_(BASE_URL) {
    client_.set_connection_timeout(5, 0);
    client_.set_read_timeout(10, 0);
}

std::optional<MarketTick> NeteaseProvider::get_realtime_quote(const Symbol& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string netease_symbol = convert_to_netease_symbol(symbol);
    std::string path = "/data/feed/" + netease_symbol;

    auto result = client_.Get(path);
    if (result && result->status == 200) {
        return parse_realtime_response(result->body, symbol);
    }

    return std::nullopt;
}

std::vector<OHLCV> NeteaseProvider::get_kline_data(const Symbol& symbol,
                                                   KlinePeriod period,
                                                   int limit) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string netease_symbol = convert_to_netease_symbol(symbol);
    std::string path = "/data/hs/kline/" + netease_symbol;

    auto result = client_.Get(path);
    if (result && result->status == 200) {
        return parse_kline_response(result->body, symbol);
    }

    return {};
}

bool NeteaseProvider::health_check() {
    auto result = client_.Get("/");
    return result && result->status == 200;
}

std::optional<MarketTick> NeteaseProvider::parse_realtime_response(const std::string& response,
                                                                   const Symbol& symbol) {
    // 简单的解析实现
    MarketTick tick;
    tick.symbol = symbol.to_string();
    tick.price = 0.0;
    tick.volume = 0;
    tick.timestamp = std::chrono::system_clock::now();

    return tick;
}

std::vector<OHLCV> NeteaseProvider::parse_kline_response(const std::string& response,
                                                         const Symbol& symbol) {
    return {};
}

std::string NeteaseProvider::convert_to_netease_symbol(const Symbol& symbol) {
    // 网易格式: 0600000(沪市), 1000001(深市) 
    static const std::array<const char*, 5> prefixes = {"0", "1", "2", "3", "4"};
    int market_index = static_cast<int>(symbol.market);
    if (market_index >= 0 && market_index < static_cast<int>(prefixes.size())) {
        return std::string(prefixes[market_index]) + symbol.code;
    }
    return "0" + symbol.code;  // 默认沪市
}

}  // namespace cppshares::data::providers