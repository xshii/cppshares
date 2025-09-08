#include <gtest/gtest.h>

#include "cppshares/data/providers/eastmoney_fields.hpp"

using namespace cppshares::data::providers;

class EastMoneyFieldsTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(EastMoneyFieldsTest, RealtimeFieldsTest) {
    // 测试实时行情字段常量
    EXPECT_STREQ(EastMoneyFields::Realtime::LATEST_PRICE, "f2");
    EXPECT_STREQ(EastMoneyFields::Realtime::VOLUME, "f5");
    EXPECT_STREQ(EastMoneyFields::Realtime::HIGH_PRICE, "f15");
    EXPECT_STREQ(EastMoneyFields::Realtime::LOW_PRICE, "f16");
    EXPECT_STREQ(EastMoneyFields::Realtime::STOCK_CODE, "f12");
    EXPECT_STREQ(EastMoneyFields::Realtime::STOCK_NAME, "f14");
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
    EXPECT_TRUE(basic_quote.find("f2") != std::string::npos);  // 最新价
    EXPECT_TRUE(basic_quote.find("f5") != std::string::npos);  // 总手(成交量)
    EXPECT_TRUE(basic_quote.find("f17") != std::string::npos); // 开盘价
}

TEST_F(EastMoneyFieldsTest, FieldDescriptionTest) {
    // 测试字段描述功能
    EXPECT_EQ(EastMoneyFields::get_field_description("f2"), "最新价");
    EXPECT_EQ(EastMoneyFields::get_field_description("f12"), "股票代码");
    EXPECT_EQ(EastMoneyFields::get_field_description("f43"), "投资收益");
    EXPECT_EQ(EastMoneyFields::get_field_description("f999"), "未知字段");
}

TEST_F(EastMoneyFieldsTest, ExtendedFieldsTest) {
    // 测试扩展信息字段
    EXPECT_STREQ(EastMoneyFields::Extended::CHANGE_60D, "f24");
    EXPECT_STREQ(EastMoneyFields::Extended::LISTING_DATE, "f26");
    EXPECT_STREQ(EastMoneyFields::Extended::INDUSTRY, "f100");
}

TEST_F(EastMoneyFieldsTest, FinancialFieldsTest) {
    // 测试财务数据字段
    EXPECT_STREQ(EastMoneyFields::Financial::OPERATING_REVENUE, "f40");
    EXPECT_STREQ(EastMoneyFields::Financial::NET_PROFIT, "f45");
    EXPECT_STREQ(EastMoneyFields::Financial::INVESTMENT_INCOME, "f43");
}