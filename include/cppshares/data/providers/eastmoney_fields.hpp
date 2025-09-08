#pragma once

#include <string>
#include <unordered_map>

namespace cppshares::data::providers {

/**
 * 东方财富API字段定义
 * 这些字段是通过逆向工程和社区总结得出的
 */
namespace EastMoneyFields {

// 实时行情字段
namespace Realtime {
constexpr const char* LATEST_PRICE = "f43";   // 最新价格
constexpr const char* CHANGE_PERCENT = "f3";  // 涨跌幅百分比
constexpr const char* CHANGE_AMOUNT = "f4";   // 涨跌额
constexpr const char* VOLUME = "f47";         // 成交量
constexpr const char* TURNOVER = "f48";       // 成交额
constexpr const char* HIGH_PRICE = "f44";     // 最高价
constexpr const char* LOW_PRICE = "f45";      // 最低价
constexpr const char* OPEN_PRICE = "f46";     // 开盘价
constexpr const char* PREV_CLOSE = "f60";     // 昨收价
constexpr const char* BID_PRICE1 = "f19";     // 买一价
constexpr const char* ASK_PRICE1 = "f20";     // 卖一价
constexpr const char* BID_VOLUME1 = "f21";    // 买一量
constexpr const char* ASK_VOLUME1 = "f22";    // 卖一量
}  // namespace Realtime

// K线数据字段 (用于字段说明，实际解析是CSV格式)
namespace Kline {
constexpr int DATE_INDEX = 0;            // 日期
constexpr int OPEN_INDEX = 1;            // 开盘价
constexpr int CLOSE_INDEX = 2;           // 收盘价
constexpr int HIGH_INDEX = 3;            // 最高价
constexpr int LOW_INDEX = 4;             // 最低价
constexpr int VOLUME_INDEX = 5;          // 成交量
constexpr int TURNOVER_INDEX = 6;        // 成交额
constexpr int AMPLITUDE_INDEX = 7;       // 振幅
constexpr int CHANGE_PERCENT_INDEX = 8;  // 涨跌幅
constexpr int CHANGE_AMOUNT_INDEX = 9;   // 涨跌额
constexpr int TURNOVER_RATE_INDEX = 10;  // 换手率
}  // namespace Kline

// 财务数据字段
namespace Financial {
constexpr const char* OPERATING_REVENUE = "f40";      // 营业收入
constexpr const char* REVENUE_YOY_CHANGE = "f41";     // 营业收入同比变化
constexpr const char* OPERATING_PROFIT = "f42";       // 营业利润
constexpr const char* INVESTMENT_INCOME = "f43";      // 投资收益(财务接口中)
constexpr const char* TOTAL_PROFIT = "f44";           // 利润总额
constexpr const char* NET_PROFIT = "f45";             // 净利润
constexpr const char* NET_PROFIT_YOY_CHANGE = "f46";  // 净利润同比变化
}  // namespace Financial

// 基本信息字段
namespace Basic {
constexpr const char* STOCK_CODE = "f12";         // 股票代码
constexpr const char* STOCK_NAME = "f14";         // 股票名称
constexpr const char* MARKET_VALUE = "f20";       // 总市值
constexpr const char* CIRCULATING_VALUE = "f21";  // 流通市值
constexpr const char* PE_RATIO = "f9";            // 市盈率
constexpr const char* PB_RATIO = "f23";           // 市净率
}  // namespace Basic

// 资金流向字段
namespace MoneyFlow {
constexpr const char* MAIN_NET_INFLOW = "f62";      // 主力资金净流入
constexpr const char* SUPER_LARGE_INFLOW = "f184";  // 超大单净流入
constexpr const char* LARGE_INFLOW = "f78";         // 大单净流入
constexpr const char* MEDIUM_INFLOW = "f82";        // 中单净流入
constexpr const char* SMALL_INFLOW = "f87";         // 小单净流入
}  // namespace MoneyFlow

// 字段描述映射表
inline const std::unordered_map<std::string, std::string> FIELD_DESCRIPTIONS = {
    // 实时行情
    {"f43", "最新价格/投资收益"},
    {"f3", "涨跌幅百分比"},
    {"f4", "涨跌额"},
    {"f47", "成交量"},
    {"f48", "成交额"},
    {"f44", "最高价/利润总额"},
    {"f45", "最低价/净利润"},
    {"f46", "开盘价/净利润同比变化"},
    {"f60", "昨收价"},
    {"f19", "买一价"},
    {"f20", "卖一价/总市值"},
    {"f21", "买一量/流通市值"},
    {"f22", "卖一量"},

    // 基本信息
    {"f12", "股票代码"},
    {"f14", "股票名称"},
    {"f9", "市盈率"},
    {"f23", "市净率"},

    // 财务数据
    {"f40", "营业收入"},
    {"f41", "营业收入同比变化"},
    {"f42", "营业利润"},

    // 资金流向
    {"f62", "主力资金净流入"},
    {"f184", "超大单净流入"},
    {"f78", "大单净流入"},
    {"f82", "中单净流入"},
    {"f87", "小单净流入"}};

/**
 * 获取字段描述
 * @param field_name 字段名称 (如 "f43")
 * @return 字段描述，如果未找到返回 "未知字段"
 */
inline std::string get_field_description(const std::string& field_name) {
    auto it = FIELD_DESCRIPTIONS.find(field_name);
    return it != FIELD_DESCRIPTIONS.end() ? it->second : "未知字段";
}

// 实时行情请求字段组合
namespace FieldSets {
// 基础行情数据
constexpr const char* BASIC_QUOTE = "f43,f44,f45,f46,f47,f48";

// 完整行情数据
constexpr const char* FULL_QUOTE = "f43,f44,f45,f46,f47,f48,f3,f4,f60,f19,f20,f21,f22";

// 五档行情数据
constexpr const char* LEVEL5_QUOTE =
    "f43,f44,f45,f46,f47,f48,f19,f20,f21,f22,f17,f18,f23,f24,f25,f26,f27,f28";
}  // namespace FieldSets

}  // namespace EastMoneyFields

}  // namespace cppshares::data::providers