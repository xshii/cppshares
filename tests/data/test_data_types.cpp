#include <gtest/gtest.h>
#include "cppshares/data/data_types.hpp"

namespace cppshares::data::tests {

class DataTypesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试数据准备
    }
};

// 测试符号解析和转换
TEST_F(DataTypesTest, SymbolParsing) {
    // 测试创建不同类型的符号
    Symbol ping_an("000001", Market::SZ, SecurityType::STOCK);
    Symbol shanghai_index("000001", Market::SH, SecurityType::INDEX);
    Symbol sh_etf("510050", Market::SH, SecurityType::ETF);
    
    EXPECT_EQ(ping_an.code, "000001");
    EXPECT_EQ(ping_an.market, Market::SZ);
    EXPECT_EQ(ping_an.type, SecurityType::STOCK);
    
    EXPECT_EQ(shanghai_index.market, Market::SH);
    EXPECT_EQ(shanghai_index.type, SecurityType::INDEX);
    
    // 测试符号相等性
    Symbol ping_an_copy("000001", Market::SZ, SecurityType::STOCK);
    EXPECT_EQ(ping_an, ping_an_copy);
    EXPECT_NE(ping_an, shanghai_index);  // 不同市场
}

// 测试枚举基本功能
TEST_F(DataTypesTest, EnumBasics) {
    // 测试枚举可以正常赋值和比较
    KlinePeriod period = KlinePeriod::MIN_1;
    EXPECT_EQ(period, KlinePeriod::MIN_1);
    EXPECT_NE(period, KlinePeriod::MIN_5);
    
    Market market = Market::SH;
    EXPECT_EQ(market, Market::SH);
    EXPECT_NE(market, Market::SZ);
    
    DataType dataType = DataType::REALTIME_QUOTE;
    EXPECT_EQ(dataType, DataType::REALTIME_QUOTE);
    EXPECT_NE(dataType, DataType::KLINE_DATA);
}

// 测试API格式转换
TEST_F(DataTypesTest, ApiFormatConversion) {
    Symbol sz_stock("000001", Market::SZ, SecurityType::STOCK);
    Symbol sh_stock("600000", Market::SH, SecurityType::STOCK);
    Symbol sh_index("000001", Market::SH, SecurityType::INDEX);
    
    // 测试新浪格式
    EXPECT_EQ(DataTypeUtils::to_sina_format(sz_stock), "sz000001");
    EXPECT_EQ(DataTypeUtils::to_sina_format(sh_stock), "sh600000");
    EXPECT_EQ(DataTypeUtils::to_sina_format(sh_index), "sh000001");
    
    // 测试腾讯格式
    EXPECT_EQ(DataTypeUtils::to_tencent_format(sz_stock), "sz000001");
    EXPECT_EQ(DataTypeUtils::to_tencent_format(sh_stock), "sh600000");
    
    // 测试网易格式
    EXPECT_EQ(DataTypeUtils::to_netease_format(sz_stock), "1000001");  // 深圳用前缀"1"
    EXPECT_EQ(DataTypeUtils::to_netease_format(sh_stock), "0600000");  // 上海用前缀"0"
}

// 测试枚举一致性
TEST_F(DataTypesTest, EnumConsistency) {
    // 确保枚举值不会意外改变
    EXPECT_EQ(static_cast<int>(KlinePeriod::MIN_5), 1);
    EXPECT_EQ(static_cast<int>(Market::HK), 3);
    EXPECT_EQ(static_cast<int>(SecurityType::ETF), 2);
    EXPECT_EQ(static_cast<int>(DataType::HISTORICAL_DATA), 2);
}

// 测试符号字符串转换
TEST_F(DataTypesTest, SymbolStringConversion) {
    Symbol symbol("000001", Market::SZ, SecurityType::STOCK);
    std::string symbol_str = symbol.to_string();
    
    // 应该包含所有必要信息
    EXPECT_TRUE(symbol_str.find("000001") != std::string::npos);
    EXPECT_TRUE(symbol_str.find("SZ") != std::string::npos);
    
    // 测试解析
    Symbol parsed = Symbol::parse(symbol_str);
    EXPECT_EQ(parsed, symbol);
}

// 性能测试
TEST_F(DataTypesTest, PerformanceTest) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        Symbol symbol("000001", Market::SZ, SecurityType::STOCK);
        std::string sina_format = DataTypeUtils::to_sina_format(symbol);
        std::string tencent_format = DataTypeUtils::to_tencent_format(symbol);
        (void)sina_format; (void)tencent_format; // 避免优化掉
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 应该在合理时间内完成
    EXPECT_LT(duration.count(), 100000); // 少于100ms
    
    double avg_time_per_conversion = static_cast<double>(duration.count()) / (iterations * 2);
    std::cout << "Average conversion time: " << avg_time_per_conversion << " microseconds" << std::endl;
}

} // namespace cppshares::data::tests