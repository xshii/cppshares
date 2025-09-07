#include "cppshares/data/data_types.hpp"

#include <array>
#include <sstream>

namespace cppshares::data {

// Symbol 实现
Symbol Symbol::parse(const std::string& symbol_str) {
    // 简单实现：格式为 "CODE.MARKET.TYPE"
    std::istringstream iss(symbol_str);
    std::string code, market_str, type_str;

    if (std::getline(iss, code, '.') && std::getline(iss, market_str, '.') &&
        std::getline(iss, type_str)) {
        static const std::unordered_map<std::string, Market> market_map = {{"SH", Market::SH},
                                                                           {"SZ", Market::SZ},
                                                                           {"BJ", Market::BJ},
                                                                           {"HK", Market::HK},
                                                                           {"US", Market::US}};

        static const std::unordered_map<std::string, SecurityType> type_map = {
            {"STOCK", SecurityType::STOCK},
            {"INDEX", SecurityType::INDEX},
            {"ETF", SecurityType::ETF},
            {"CONVERTIBLE", SecurityType::CONVERTIBLE},
            {"BOND", SecurityType::BOND},
            {"FUTURE", SecurityType::FUTURE},
            {"OPTION", SecurityType::OPTION}};

        auto market_it = market_map.find(market_str);
        auto type_it = type_map.find(type_str);

        return Symbol(code,
                      market_it != market_map.end() ? market_it->second : Market::SH,
                      type_it != type_map.end() ? type_it->second : SecurityType::STOCK);
    }

    return Symbol(symbol_str, Market::SH, SecurityType::STOCK);
}

std::string Symbol::to_string() const {
    static const std::array<const char*, 5> market_names = {"SH", "SZ", "BJ", "HK", "US"};
    static const std::array<const char*, 7> type_names = {
        "STOCK", "INDEX", "ETF", "CONVERTIBLE", "BOND", "FUTURE", "OPTION"};

    return code + "." + market_names[static_cast<int>(market)] + "." +
           type_names[static_cast<int>(type)];
}

// DataTypeUtils 实现
std::string DataTypeUtils::to_sina_format(const Symbol& symbol) {
    static const std::array<const char*, 5> prefixes = {"sh", "sz", "bj", "hk", "us"};
    return std::string(prefixes[static_cast<int>(symbol.market)]) + symbol.code;
}

std::string DataTypeUtils::to_tencent_format(const Symbol& symbol) {
    static const std::array<const char*, 5> prefixes = {"sh", "sz", "bj", "hk", "us"};
    return std::string(prefixes[static_cast<int>(symbol.market)]) + symbol.code;
}

std::string DataTypeUtils::to_netease_format(const Symbol& symbol) {
    static const std::array<const char*, 5> prefixes = {"0", "1", "2", "3", "4"};
    return std::string(prefixes[static_cast<int>(symbol.market)]) + symbol.code;
}

}  // namespace cppshares::data