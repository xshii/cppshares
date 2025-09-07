#pragma once

#include <string>
#include <unordered_map>

namespace cppshares::data {

// 数据类型枚举
enum class DataType { REALTIME_QUOTE, KLINE_DATA, HISTORICAL_DATA, MARKET_DEPTH, TRADE_DETAIL };

// K线周期枚举
enum class KlinePeriod {
    MIN_1,   // 1分钟
    MIN_5,   // 5分钟
    MIN_15,  // 15分钟
    MIN_30,  // 30分钟
    HOUR_1,  // 1小时
    HOUR_4,  // 4小时
    DAY_1,   // 日K
    WEEK_1,  // 周K
    MONTH_1  // 月K
};

// 市场类型枚举
enum class Market {
    SH,  // 上海证券交易所
    SZ,  // 深圳证券交易所
    BJ,  // 北京证券交易所
    HK,  // 香港交易所
    US   // 美国市场
};

// 证券类型枚举
enum class SecurityType {
    STOCK,        // 股票
    INDEX,        // 指数
    ETF,          // ETF
    CONVERTIBLE,  // 可转债
    BOND,         // 债券
    FUTURE,       // 期货
    OPTION        // 期权
};

// 符号结构
struct Symbol {
    std::string code;   // 证券代码
    Market market;      // 市场
    SecurityType type;  // 证券类型

    // 构造函数
    Symbol(const std::string& code, Market market, SecurityType type = SecurityType::STOCK)
        : code(code), market(market), type(type) {}

    // 从字符串解析符号
    static Symbol parse(const std::string& symbol_str);

    // 转换为字符串
    std::string to_string() const;

    // 比较运算符
    bool operator==(const Symbol& other) const {
        return code == other.code && market == other.market && type == other.type;
    }
};

}  // namespace cppshares::data

// 为Symbol提供hash支持，用于unordered_map
namespace std {
template <>
struct hash<cppshares::data::Symbol> {
    size_t operator()(const cppshares::data::Symbol& symbol) const {
        return hash<string>()(symbol.code) ^ (hash<int>()(static_cast<int>(symbol.market)) << 1) ^
               (hash<int>()(static_cast<int>(symbol.type)) << 2);
    }
};
}  // namespace std