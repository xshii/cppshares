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
constexpr const char* LATEST_PRICE = "f2";    // 最新价
constexpr const char* CHANGE_PERCENT = "f3";  // 涨跌幅
constexpr const char* CHANGE_AMOUNT = "f4";   // 涨跌额
constexpr const char* VOLUME = "f5";          // 总手（成交量）
constexpr const char* TURNOVER = "f6";        // 成交额
constexpr const char* AMPLITUDE = "f7";       // 振幅
constexpr const char* TURNOVER_RATE = "f8";   // 换手率
constexpr const char* PE_RATIO = "f9";        // 市盈率
constexpr const char* VOLUME_RATIO = "f10";   // 量比
constexpr const char* STOCK_CODE = "f12";     // 股票代码
constexpr const char* MARKET = "f13";         // 市场
constexpr const char* STOCK_NAME = "f14";     // 股票名称
constexpr const char* HIGH_PRICE = "f15";     // 最高价
constexpr const char* LOW_PRICE = "f16";      // 最低价
constexpr const char* OPEN_PRICE = "f17";     // 开盘价
constexpr const char* PREV_CLOSE = "f18";     // 昨收价
constexpr const char* MARKET_VALUE = "f20";   // 总市值
constexpr const char* CIRCULATING_VALUE = "f21"; // 流通市值
constexpr const char* PRICE_SPEED = "f22";    // 涨速
constexpr const char* PB_RATIO = "f23";       // 市净率
constexpr const char* CURRENT_VOLUME = "f30"; // 现手
constexpr const char* BID_PRICE = "f31";      // 买入价
constexpr const char* ASK_PRICE = "f32";      // 卖出价
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
constexpr const char* REVENUE_YOY_CHANGE = "f41";     // 营业收入同比
constexpr const char* OPERATING_PROFIT = "f42";       // 营业利润
constexpr const char* INVESTMENT_INCOME = "f43";      // 投资收益
constexpr const char* TOTAL_PROFIT = "f44";           // 利润总额
constexpr const char* NET_PROFIT = "f45";             // 净利润
constexpr const char* NET_PROFIT_YOY_CHANGE = "f46";  // 净利润同比
constexpr const char* UNDISTRIBUTED_PROFIT = "f47";   // 未分配利润
constexpr const char* UNDISTRIBUTED_PER_SHARE = "f48"; // 每股未分配利润
constexpr const char* GROSS_MARGIN = "f49";           // 毛利率
constexpr const char* TOTAL_ASSETS = "f50";           // 总资产
constexpr const char* CURRENT_ASSETS = "f51";         // 流动资产
constexpr const char* FIXED_ASSETS = "f52";           // 固定资产
constexpr const char* INTANGIBLE_ASSETS = "f53";      // 无形资产
constexpr const char* TOTAL_LIABILITIES = "f54";      // 总负债
constexpr const char* CURRENT_LIABILITIES = "f55";    // 流动负债
constexpr const char* LONG_TERM_LIABILITIES = "f56";  // 长期负债
constexpr const char* DEBT_RATIO = "f57";             // 资产负债比率
constexpr const char* SHAREHOLDERS_EQUITY = "f58";     // 股东权益
constexpr const char* EQUITY_RATIO = "f59";           // 股东权益比
constexpr const char* RESERVE_FUND = "f60";           // 公积金
constexpr const char* RESERVE_PER_SHARE = "f61";      // 每股公积金
constexpr const char* EPS = "f112";                   // 每股收益
constexpr const char* BOOK_VALUE_PER_SHARE = "f113";  // 每股净资产
constexpr const char* PE_STATIC = "f114";             // 市盈率（静）
constexpr const char* PE_TTM = "f115";               // 市盈率（TTM）
}  // namespace Financial

// 扩展信息字段
namespace Extended {
constexpr const char* CHANGE_60D = "f24";            // 60日涨跌幅
constexpr const char* CHANGE_YTD = "f25";            // 年初至今涨跌幅
constexpr const char* LISTING_DATE = "f26";          // 上市日期
constexpr const char* SETTLEMENT_PRICE = "f28";      // 昨日结算价
constexpr const char* COMMISSION_RATIO = "f33";      // 委比
constexpr const char* OUTER_VOLUME = "f34";          // 外盘
constexpr const char* INNER_VOLUME = "f35";          // 内盘
constexpr const char* SHARES_PER_CAPITA = "f36";     // 人均持股数
constexpr const char* ROE_WEIGHTED = "f37";          // 净资产收益率(加权)
constexpr const char* TOTAL_SHARE_CAPITAL = "f38";   // 总股本
constexpr const char* CIRCULATING_SHARES = "f39";    // 流通股
constexpr const char* INDUSTRY = "f100";             // 行业
constexpr const char* REGION = "f102";               // 地区板块
constexpr const char* TRADING_TIME = "f124";         // 当前交易时间
constexpr const char* PS_TTM = "f130";               // 市销率TTM
constexpr const char* PCF_TTM = "f131";              // 市现率TTM
constexpr const char* REVENUE_TTM = "f132";          // 总营业收入TTM
constexpr const char* DIVIDEND_YIELD = "f133";       // 股息率
constexpr const char* NET_ASSETS = "f135";           // 净资产
constexpr const char* NET_PROFIT_TTM = "f138";       // 净利润TTM
}  // namespace Extended

// 资金流向字段
namespace MoneyFlow {
constexpr const char* MAIN_NET_INFLOW = "f62";       // 主力净流入
constexpr const char* CALL_AUCTION = "f63";          // 集合竞价
constexpr const char* SUPER_LARGE_INFLOW = "f64";    // 超大单流入
constexpr const char* SUPER_LARGE_OUTFLOW = "f65";   // 超大单流出
constexpr const char* SUPER_LARGE_NET = "f66";       // 超大单净额
constexpr const char* SUPER_LARGE_NET_RATIO = "f69"; // 超大单净占比
constexpr const char* LARGE_INFLOW = "f70";          // 大单流入
constexpr const char* LARGE_OUTFLOW = "f71";         // 大单流出
constexpr const char* LARGE_NET = "f72";             // 大单净额
constexpr const char* LARGE_NET_RATIO = "f75";       // 大单净占比
constexpr const char* MEDIUM_INFLOW = "f76";         // 中单流入
constexpr const char* MEDIUM_OUTFLOW = "f77";        // 中单流出
constexpr const char* MEDIUM_NET = "f78";            // 中单净额
constexpr const char* MEDIUM_NET_RATIO = "f81";      // 中单净占比
constexpr const char* SMALL_INFLOW = "f82";          // 小单流入
constexpr const char* SMALL_OUTFLOW = "f83";         // 小单流出
constexpr const char* SMALL_NET = "f84";             // 小单净额
constexpr const char* SMALL_NET_RATIO = "f87";       // 小单净占比
// DDX/DDY/DDZ指标
constexpr const char* DDX_TODAY = "f88";             // 当日DDX
constexpr const char* DDY_TODAY = "f89";             // 当日DDY
constexpr const char* DDZ_TODAY = "f90";             // 当日DDZ
constexpr const char* DDX_5D = "f91";                // 5日DDX
constexpr const char* DDY_5D = "f92";                // 5日DDY
constexpr const char* DDX_10D = "f94";               // 10日DDX
constexpr const char* DDY_10D = "f95";               // 10日DDY
}  // namespace MoneyFlow

// 字段描述映射表
inline const std::unordered_map<std::string, std::string> FIELD_DESCRIPTIONS = {
    // 实时行情基础字段
    {"f2", "最新价"},
    {"f3", "涨跌幅"},
    {"f4", "涨跌额"},
    {"f5", "总手"},
    {"f6", "成交额"},
    {"f7", "振幅"},
    {"f8", "换手率"},
    {"f9", "市盈率"},
    {"f10", "量比"},
    {"f11", "5分钟涨跌幅"},
    {"f12", "股票代码"},
    {"f13", "市场"},
    {"f14", "股票名称"},
    {"f15", "最高价"},
    {"f16", "最低价"},
    {"f17", "开盘价"},
    {"f18", "昨收"},
    {"f20", "总市值"},
    {"f21", "流通市值"},
    {"f22", "涨速"},
    {"f23", "市净率"},
    {"f24", "60日涨跌幅"},
    {"f25", "年初至今涨跌幅"},
    {"f26", "上市日期"},
    {"f28", "昨日结算价"},
    {"f30", "现手"},
    {"f31", "买入价"},
    {"f32", "卖出价"},
    {"f33", "委比"},
    {"f34", "外盘"},
    {"f35", "内盘"},
    {"f36", "人均持股数"},
    {"f37", "净资产收益率(加权)"},
    {"f38", "总股本"},
    {"f39", "流通股"},
    
    // 财务数据
    {"f40", "营业收入"},
    {"f41", "营业收入同比"},
    {"f42", "营业利润"},
    {"f43", "投资收益"},
    {"f44", "利润总额"},
    {"f45", "净利润"},
    {"f46", "净利润同比"},
    {"f47", "未分配利润"},
    {"f48", "每股未分配利润"},
    {"f49", "毛利率"},
    {"f50", "总资产"},
    {"f51", "流动资产"},
    {"f52", "固定资产"},
    {"f53", "无形资产"},
    {"f54", "总负债"},
    {"f55", "流动负债"},
    {"f56", "长期负债"},
    {"f57", "资产负债比率"},
    {"f58", "股东权益"},
    {"f59", "股东权益比"},
    {"f60", "公积金"},
    {"f61", "每股公积金"},
    
    // 资金流向
    {"f62", "主力净流入"},
    {"f63", "集合竞价"},
    {"f64", "超大单流入"},
    {"f65", "超大单流出"},
    {"f66", "超大单净额"},
    {"f69", "超大单净占比"},
    {"f70", "大单流入"},
    {"f71", "大单流出"},
    {"f72", "大单净额"},
    {"f75", "大单净占比"},
    {"f76", "中单流入"},
    {"f77", "中单流出"},
    {"f78", "中单净额"},
    {"f81", "中单净占比"},
    {"f82", "小单流入"},
    {"f83", "小单流出"},
    {"f84", "小单净额"},
    {"f87", "小单净占比"},
    
    // DDX指标
    {"f88", "当日DDX"},
    {"f89", "当日DDY"},
    {"f90", "当日DDZ"},
    {"f91", "5日DDX"},
    {"f92", "5日DDY"},
    {"f94", "10日DDX"},
    {"f95", "10日DDY"},
    {"f97", "DDX飘红天数(连续)"},
    {"f98", "DDX飘红天数(5日)"},
    {"f99", "DDX飘红天数(10日)"},
    
    // 其他字段
    {"f100", "行业"},
    {"f102", "地区板块"},
    {"f112", "每股收益"},
    {"f113", "每股净资产"},
    {"f114", "市盈率（静）"},
    {"f115", "市盈率（TTM）"},
    {"f124", "当前交易时间"},
    {"f130", "市销率TTM"},
    {"f131", "市现率TTM"},
    {"f132", "总营业收入TTM"},
    {"f133", "股息率"},
    {"f135", "净资产"},
    {"f138", "净利润TTM"}};

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
// 基础行情数据 - 价格、涨跌、成交量
constexpr const char* BASIC_QUOTE = "f2,f3,f4,f5,f6,f15,f16,f17,f18";

// 完整行情数据 - 包含市值、比率等
constexpr const char* FULL_QUOTE = "f2,f3,f4,f5,f6,f7,f8,f9,f10,f12,f14,f15,f16,f17,f18,f20,f21,f22,f23";

// 五档行情数据 - 包含买卖盘信息
constexpr const char* LEVEL5_QUOTE = 
    "f2,f3,f4,f5,f6,f15,f16,f17,f18,f31,f32,f30,f33,f34,f35";

// 财务分析数据
constexpr const char* FINANCIAL_DATA = 
    "f40,f41,f42,f43,f44,f45,f46,f112,f113,f114,f115,f130,f131,f133";

// 资金流向数据
constexpr const char* MONEY_FLOW = 
    "f62,f64,f65,f66,f70,f71,f72,f76,f77,f78,f82,f83,f84";
}  // namespace FieldSets

}  // namespace EastMoneyFields

}  // namespace cppshares::data::providers