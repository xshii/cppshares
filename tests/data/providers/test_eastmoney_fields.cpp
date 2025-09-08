#include <gtest/gtest.h>

#include "cppshares/data/providers/eastmoney_fields.hpp"

using namespace cppshares::data::providers;

class EastMoneyFieldsTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(EastMoneyFieldsTest, RealtimeFieldsTest) {
    // 测试实时行情字段常量
    EXPECT_STREQ(EastMoneyFields::Realtime::LATEST_PRICE, "f43");
    EXPECT_STREQ(EastMoneyFields::Realtime::VOLUME, "f47");
    EXPECT_STREQ(EastMoneyFields::Realtime::HIGH_PRICE, "f44");
    EXPECT_STREQ(EastMoneyFields::Realtime::LOW_PRICE, "f45");
}

TEST_F(EastMoneyFieldsTest, KlineIndicesTest) {
    // 测试K线数据索引
    EXPECT_EQ(EastMoneyFields::Kline::DATE_INDEX, 0);
    EXPECT_EQ(EastMoneyFields::Kline::OPEN_INDEX, 1);
    EXPECT_EQ(EastMoneyFields::Kline::CLOSE_INDEX, 2);
    EXPECT_EQ(EastMoneyFields::Kline::HIGH_INDEX, 3);
    EXPECT_EQ(EastMoneyFields::Kline::LOW_INDEX, 4);
    EXPECT_EQ(EastMoneyFields::Kline::VOLUME_INDEX, 5);
}

TEST_F(EastMoneyFieldsTest, FieldSetsTest) {
    // 测试字段组合
    std::string basic_quote = EastMoneyFields::FieldSets::BASIC_QUOTE;
    EXPECT_TRUE(basic_quote.find("f43") != std::string::npos);
    EXPECT_TRUE(basic_quote.find("f47") != std::string::npos);
}

TEST_F(EastMoneyFieldsTest, FieldDescriptionTest) {
    // 测试字段描述功能
    EXPECT_EQ(EastMoneyFields::get_field_description("f43"), "最新价格/投资收益");
    EXPECT_EQ(EastMoneyFields::get_field_description("f12"), "股票代码");
    EXPECT_EQ(EastMoneyFields::get_field_description("f999"), "未知字段");
}

TEST_F(EastMoneyFieldsTest, BasicFieldsTest) {
    // 测试基本信息字段
    EXPECT_STREQ(EastMoneyFields::Basic::STOCK_CODE, "f12");
    EXPECT_STREQ(EastMoneyFields::Basic::STOCK_NAME, "f14");
    EXPECT_STREQ(EastMoneyFields::Basic::MARKET_VALUE, "f20");
}

TEST_F(EastMoneyFieldsTest, FinancialFieldsTest) {
    // 测试财务数据字段
    EXPECT_STREQ(EastMoneyFields::Financial::OPERATING_REVENUE, "f40");
    EXPECT_STREQ(EastMoneyFields::Financial::NET_PROFIT, "f45");
    EXPECT_STREQ(EastMoneyFields::Financial::INVESTMENT_INCOME, "f43");
}