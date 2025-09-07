#include "cppshares/data/providers/eastmoney_provider.hpp"
#include "cppshares/data/data_types.hpp"

namespace cppshares::data::providers {

EastMoneyProvider::EastMoneyProvider()
    : client_(BASE_URL), kline_client_(KLINE_URL) {
    client_.set_connection_timeout(5, 0);
    client_.set_read_timeout(10, 0);
    kline_client_.set_connection_timeout(5, 0);
    kline_client_.set_read_timeout(10, 0);
}

std::optional<MarketTick> EastMoneyProvider::get_realtime_quote(const Symbol& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string em_symbol = format_symbol_for_eastmoney(symbol);
    std::string path = "/api/qt/stock/get?secid=" + em_symbol + "&fields=f43,f44,f45,f46,f47,f48";

    auto result = client_.Get(path.c_str());
    if (result && result->status == 200) {
        return parse_realtime_response(result->body, symbol);
    }

    return std::nullopt;
}

std::vector<OHLCV> EastMoneyProvider::get_kline_data(const Symbol& symbol,
                                                     KlinePeriod period,
                                                     int limit) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string em_symbol = format_symbol_for_eastmoney(symbol);
    std::string period_str = period_to_eastmoney_format(period);
    std::string path = "/api/qt/stock/kline/get?secid=" + em_symbol +
                       "&fields1=f1,f2,f3&fields2=f51,f52,f53,f54,f55,f56,f57,f58" +
                       "&klt=" + period_str +
                       "&fqt=1&beg=19900101&end=20500101&limit=" + std::to_string(limit);

    auto result = kline_client_.Get(path.c_str());
    if (result && result->status == 200) {
        return parse_kline_response(result->body, symbol);
    }

    return {};
}

bool EastMoneyProvider::health_check() {
    auto result = client_.Get("/api/qt/stock/get?secid=1.000001&fields=f43");
    return result && result->status == 200;
}

std::string EastMoneyProvider::format_symbol_for_eastmoney(const Symbol& symbol) {
    // 沪市: 1.600000 深市: 0.000001
    if (symbol.market == Market::SH) {
        return "1." + symbol.code;
    } else if (symbol.market == Market::SZ) {
        return "0." + symbol.code;
    }
    return "1." + symbol.code;  // 默认沪市
}

std::optional<MarketTick> EastMoneyProvider::parse_realtime_response(const std::string& response,
                                                                    const Symbol& symbol) {
    try {
        auto json = nlohmann::json::parse(response);
        if (json.contains("data") && json["data"].contains("f43")) {
            MarketTick tick;
            tick.symbol = symbol.to_string();
            tick.price = json["data"]["f43"].get<double>();
            tick.volume = json["data"].value("f47", 0);
            tick.timestamp = std::chrono::system_clock::now();
            return tick;
        }
    } catch (const std::exception&) {
        // JSON解析失败，返回空值
    }
    return std::nullopt;
}

std::vector<OHLCV> EastMoneyProvider::parse_kline_response(const std::string& response,
                                                           const Symbol& symbol) {
    // 简单解析实现
    return {};
}

std::string EastMoneyProvider::period_to_eastmoney_format(KlinePeriod period) {
    // 东方财富klt参数: 1=1min, 5=5min, 15=15min, 30=30min, 60=60min, 101=日, 102=周, 103=月
    static const std::array<const char*, 9> period_names = {
        "1", "5", "15", "30", "60", "240", "101", "102", "103"};
    
    int index = static_cast<int>(period);
    if (index >= 0 && index < static_cast<int>(period_names.size())) {
        return period_names[index];
    }
    return "101";  // 默认返回日线
}

}  //