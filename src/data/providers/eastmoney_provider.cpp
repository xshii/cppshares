#include "cppshares/data/providers/eastmoney_provider.hpp"

#include <iomanip>
#include <sstream>

#include "cppshares/data/data_types.hpp"
#include "cppshares/utils/logger.hpp"

namespace cppshares::data::providers {

EastMoneyProvider::EastMoneyProvider() : client_(BASE_URL), kline_client_(KLINE_URL) {
    client_.set_connection_timeout(5, 0);
    client_.set_read_timeout(10, 0);
    kline_client_.set_connection_timeout(5, 0);
    kline_client_.set_read_timeout(10, 0);
}

std::optional<MarketTick> EastMoneyProvider::get_realtime_quote(const Symbol& symbol) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string em_symbol = format_symbol_for_eastmoney(symbol);
    std::string path = "/api/qt/stock/get?secid=" + em_symbol +
                       "&fields=" + EastMoneyFields::FieldSets::BASIC_QUOTE;

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
    auto result = client_.Get(("/api/qt/stock/get?secid=1.000001&fields=" +
                               std::string(EastMoneyFields::Realtime::LATEST_PRICE))
                                  .c_str());
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
    // 记录JSON响应到文件
    utils::Logger::log_json_response("EastMoney", "realtime", symbol.to_string(), response);

    try {
        auto json = nlohmann::json::parse(response);

        // 记录调试信息，显示字段含义
        utils::Logger::debug(
            "EastMoney realtime parsing - using field {}: {}",
            EastMoneyFields::Realtime::LATEST_PRICE,
            EastMoneyFields::get_field_description(EastMoneyFields::Realtime::LATEST_PRICE));
        if (json.contains("data") &&
            json["data"].contains(EastMoneyFields::Realtime::LATEST_PRICE)) {
            MarketTick tick;
            tick.symbol = symbol.to_string();
            tick.price = json["data"][EastMoneyFields::Realtime::LATEST_PRICE].get<double>();
            tick.volume = json["data"].value(EastMoneyFields::Realtime::VOLUME, 0);
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
    std::vector<OHLCV> klines;

    // 记录JSON响应到文件
    utils::Logger::log_json_response("EastMoney", "kline", symbol.to_string(), response);

    try {
        auto json = nlohmann::json::parse(response);

        // 检查响应是否包含数据
        if (!json.contains("data") || json["data"].is_null()) {
            utils::Logger::error("EastMoney: No data field in response for symbol {}",
                                 symbol.to_string());
            return klines;
        }

        auto data = json["data"];
        if (!data.contains("klines") || !data["klines"].is_array()) {
            utils::Logger::error("EastMoney: No klines array in response for symbol {}",
                                 symbol.to_string());
            return klines;
        }

        // 解析K线数据数组
        for (const auto& kline_str : data["klines"]) {
            if (!kline_str.is_string()) {
                continue;
            }

            try {
                // 东方财富K线数据格式:
                // "日期,开盘,收盘,最高,最低,成交量,成交额,振幅,涨跌幅,涨跌额,换手率"
                std::string kline_data = kline_str.get<std::string>();
                std::vector<std::string> fields;

                // 分割逗号分隔的字符串
                std::stringstream ss(kline_data);
                std::string field;
                while (std::getline(ss, field, ',')) {
                    fields.push_back(field);
                }

                if (fields.size() >= 7) {  // 至少需要日期、开盘、收盘、最高、最低、成交量、成交额
                    OHLCV ohlcv;
                    ohlcv.symbol = symbol.to_string();

                    // 解析日期 (格式: YYYY-MM-DD)
                    std::tm tm = {};
                    std::istringstream date_stream(fields[EastMoneyFields::Kline::DATE_INDEX]);
                    date_stream >> std::get_time(&tm, "%Y-%m-%d");
                    if (!date_stream.fail()) {
                        auto time_t_val = std::mktime(&tm);
                        ohlcv.timestamp = std::chrono::system_clock::from_time_t(time_t_val);
                    } else {
                        utils::Logger::error("EastMoney: Invalid date format '{}' for symbol {}",
                                             fields[EastMoneyFields::Kline::DATE_INDEX],
                                             symbol.to_string());
                        ohlcv.timestamp = std::chrono::system_clock::now();
                    }

                    // 解析OHLCV数据
                    ohlcv.open = std::stod(fields[EastMoneyFields::Kline::OPEN_INDEX]);
                    ohlcv.close = std::stod(fields[EastMoneyFields::Kline::CLOSE_INDEX]);
                    ohlcv.high = std::stod(fields[EastMoneyFields::Kline::HIGH_INDEX]);
                    ohlcv.low = std::stod(fields[EastMoneyFields::Kline::LOW_INDEX]);
                    ohlcv.volume = std::stoull(fields[EastMoneyFields::Kline::VOLUME_INDEX]);
                    ohlcv.amount = std::stod(fields[EastMoneyFields::Kline::TURNOVER_INDEX]);

                    // 数据验证
                    if (ohlcv.open > 0 && ohlcv.close > 0 && ohlcv.high > 0 && ohlcv.low > 0 &&
                        ohlcv.high >= ohlcv.low) {
                        klines.push_back(ohlcv);
                    } else {
                        utils::Logger::error(
                            "EastMoney: Invalid OHLCV data for symbol {}: O={}, H={}, L={}, C={}",
                            symbol.to_string(),
                            ohlcv.open,
                            ohlcv.high,
                            ohlcv.low,
                            ohlcv.close);
                    }
                } else {
                    utils::Logger::error(
                        "EastMoney: Insufficient fields in kline data for symbol {}: expected>=7, "
                        "got={}",
                        symbol.to_string(),
                        fields.size());
                }
            } catch (const std::exception& e) {
                utils::Logger::error("EastMoney: Failed to parse kline entry for symbol {}: {}",
                                     symbol.to_string(),
                                     e.what());
                continue;
            }
        }

        utils::Logger::info("EastMoney: Successfully parsed {} klines for symbol {}",
                            klines.size(),
                            symbol.to_string());

    } catch (const nlohmann::json::parse_error& e) {
        utils::Logger::error("EastMoney: JSON parse error for symbol {}: {} at byte {}",
                             symbol.to_string(),
                             e.what(),
                             e.byte);
    } catch (const std::exception& e) {
        utils::Logger::error("EastMoney: Unexpected error parsing klines for symbol {}: {}",
                             symbol.to_string(),
                             e.what());
    }

    return klines;
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

}  // namespace cppshares::data::providers