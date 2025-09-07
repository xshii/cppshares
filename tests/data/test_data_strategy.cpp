#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "cppshares/data/data_strategy.hpp"
#include "cppshares/data/data_types.hpp"

namespace cppshares::data::tests {

// Mock数据提供者
class MockDataProvider : public DataProvider {
public:
    MOCK_METHOD(std::optional<MarketTick>, get_realtime_quote, (const Symbol& symbol), (override));
    MOCK_METHOD(std::vector<OHLCV>, get_kline_data, 
                (const Symbol& symbol, KlinePeriod period, int limit), (override));
    MOCK_METHOD(std::string, get_name, (), (const, override));
    MOCK_METHOD(int, get_priority, (), (const, override));
    MOCK_METHOD(int, get_rate_limit, (), (const, override));
    MOCK_METHOD(bool, health_check, (), (override));
};

class DataStrategyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建mock提供者
        provider1_ = std::make_shared<MockDataProvider>();
        provider2_ = std::make_shared<MockDataProvider>();
        provider3_ = std::make_shared<MockDataProvider>();
        
        // 设置基本属性
        ON_CALL(*provider1_, get_name()).WillByDefault(testing::Return("Provider1"));
        ON_CALL(*provider1_, get_priority()).WillByDefault(testing::Return(1));
        ON_CALL(*provider1_, get_rate_limit()).WillByDefault(testing::Return(100));
        
        ON_CALL(*provider2_, get_name()).WillByDefault(testing::Return("Provider2"));
        ON_CALL(*provider2_, get_priority()).WillByDefault(testing::Return(2));
        ON_CALL(*provider2_, get_rate_limit()).WillByDefault(testing::Return(80));
        
        ON_CALL(*provider3_, get_name()).WillByDefault(testing::Return("Provider3"));
        ON_CALL(*provider3_, get_priority()).WillByDefault(testing::Return(3));
        ON_CALL(*provider3_, get_rate_limit()).WillByDefault(testing::Return(60));
        
        providers_ = {provider1_, provider2_, provider3_};
    }
    
    std::shared_ptr<MockDataProvider> provider1_;
    std::shared_ptr<MockDataProvider> provider2_;
    std::shared_ptr<MockDataProvider> provider3_;
    std::vector<std::shared_ptr<DataProvider>> providers_;
};

// 测试故障转移策略
TEST_F(DataStrategyTest, FailoverStrategy) {
    FailoverStrategy strategy;
    
    auto selected = strategy.select_providers(DataType::REALTIME_QUOTE, providers_);
    
    // 应该按优先级排序
    ASSERT_EQ(selected.size(), 3);
    EXPECT_EQ(selected[0]->get_priority(), 1);
    EXPECT_EQ(selected[1]->get_priority(), 2);
    EXPECT_EQ(selected[2]->get_priority(), 3);
    
    EXPECT_EQ(strategy.get_strategy_name(), "Failover");
}

// 测试轮询策略
TEST_F(DataStrategyTest, RoundRobinStrategy) {
    RoundRobinStrategy strategy;
    
    // 第一次调用
    auto selected1 = strategy.select_providers(DataType::REALTIME_QUOTE, providers_);
    ASSERT_FALSE(selected1.empty());
    
    // 第二次调用应该选择不同的提供者
    auto selected2 = strategy.select_providers(DataType::REALTIME_QUOTE, providers_);
    ASSERT_FALSE(selected2.empty());
    
    // 对于不同的数据类型，轮询是独立的
    auto selected3 = strategy.select_providers(DataType::KLINE_DATA, providers_);
    ASSERT_FALSE(selected3.empty());
    
    EXPECT_EQ(strategy.get_strategy_name(), "RoundRobin");
}

// 测试加权随机策略
TEST_F(DataStrategyTest, WeightedRandomStrategy) {
    WeightedRandomStrategy strategy;
    
    auto selected = strategy.select_providers(DataType::REALTIME_QUOTE, providers_);
    
    // 应该返回可用的提供者
    ASSERT_FALSE(selected.empty());
    EXPECT_EQ(strategy.get_strategy_name(), "WeightedRandom");
}

// 测试数据聚合器
class DataAggregatorTest : public DataStrategyTest {
protected:
    void SetUp() override {
        DataStrategyTest::SetUp();
        
        // 创建聚合器并注册提供者
        for (auto& provider : providers_) {
            aggregator_.register_provider(provider);
        }
        
        // 设置默认策略
        aggregator_.set_strategy(std::make_unique<FailoverStrategy>());
    }
    
    DataAggregator aggregator_;
};

TEST_F(DataAggregatorTest, BasicFunctionality) {
    Symbol test_symbol("000001", Market::SZ, SecurityType::STOCK);
    
    // 准备测试数据
    MarketTick expected_tick;
    expected_tick.symbol = test_symbol.to_string();
    expected_tick.price = 10.5;
    expected_tick.volume = 1000;
    expected_tick.timestamp = std::chrono::system_clock::now();
    
    // 设置期望：第一个提供者成功
    EXPECT_CALL(*provider1_, get_realtime_quote(testing::_))
        .WillOnce(testing::Return(expected_tick));
    
    // 测试获取实时行情
    auto result = aggregator_.get_realtime_quote(test_symbol);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->symbol, expected_tick.symbol);
    EXPECT_EQ(result->price, expected_tick.price);
}

TEST_F(DataAggregatorTest, FailoverBehavior) {
    Symbol test_symbol("000001", Market::SZ, SecurityType::STOCK);
    
    MarketTick expected_tick;
    expected_tick.symbol = test_symbol.to_string();
    expected_tick.price = 10.5;
    
    // 设置期望：第一个提供者失败，第二个成功
    EXPECT_CALL(*provider1_, get_realtime_quote(testing::_))
        .WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(*provider2_, get_realtime_quote(testing::_))
        .WillOnce(testing::Return(expected_tick));
    
    // 测试故障转移
    auto result = aggregator_.get_realtime_quote(test_symbol);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->price, expected_tick.price);
}

TEST_F(DataAggregatorTest, AllProvidersFail) {
    Symbol test_symbol("000001", Market::SZ, SecurityType::STOCK);
    
    // 设置期望：所有提供者都失败
    EXPECT_CALL(*provider1_, get_realtime_quote(testing::_))
        .WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(*provider2_, get_realtime_quote(testing::_))
        .WillOnce(testing::Return(std::nullopt));
    EXPECT_CALL(*provider3_, get_realtime_quote(testing::_))
        .WillOnce(testing::Return(std::nullopt));
    
    // 测试全部失败的情况
    auto result = aggregator_.get_realtime_quote(test_symbol);
    EXPECT_FALSE(result.has_value());
}

TEST_F(DataAggregatorTest, HealthMonitoring) {
    // 测试健康状态监控
    aggregator_.update_provider_health();
    
    auto health_status = aggregator_.get_provider_health();
    EXPECT_EQ(health_status.size(), 3);
    
    for (const auto& [name, health] : health_status) {
        EXPECT_TRUE(!name.empty());
        // 初始状态应该是UNKNOWN
        EXPECT_EQ(health.status, ProviderStatus::UNKNOWN);
    }
}

TEST_F(DataAggregatorTest, Statistics) {
    auto stats = aggregator_.get_statistics();
    
    // 初始统计应该为0
    EXPECT_EQ(stats.total_requests, 0);
    EXPECT_EQ(stats.successful_requests, 0);
    EXPECT_EQ(stats.failed_requests, 0);
}

// 性能测试
TEST_F(DataAggregatorTest, PerformanceTest) {
    Symbol test_symbol("000001", Market::SZ, SecurityType::STOCK);
    
    MarketTick test_tick;
    test_tick.symbol = test_symbol.to_string();
    test_tick.price = 10.5;
    
    // 设置提供者总是成功
    EXPECT_CALL(*provider1_, get_realtime_quote(testing::_))
        .WillRepeatedly(testing::Return(test_tick));
    
    const int iterations = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto result = aggregator_.get_realtime_quote(test_symbol);
        ASSERT_TRUE(result.has_value());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time_per_request = static_cast<double>(duration.count()) / iterations;
    std::cout << "Average request time: " << avg_time_per_request << " microseconds" << std::endl;
    
    // 性能应该在合理范围内
    EXPECT_LT(avg_time_per_request, 1000.0); // 少于1ms每次请求
}

} // namespace cppshares::data::tests