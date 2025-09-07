#include <gtest/gtest.h>
#include "cppshares/data/providers/sina_provider.hpp"
#include "cppshares/data/providers/tencent_provider.hpp"
#include "cppshares/data/providers/netease_provider.hpp"
#include "cppshares/data/data_types.hpp"

namespace cppshares::data::providers::tests {

class ProvidersTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_symbols_ = {
            Symbol("000001", Market::SZ, SecurityType::STOCK),  // 平安银行
            Symbol("000002", Market::SZ, SecurityType::STOCK),  // 万科A
            Symbol("600000", Market::SH, SecurityType::STOCK),  // 浦发银行
            Symbol("000001", Market::SH, SecurityType::INDEX)   // 上证指数
        };
    }
    
    std::vector<Symbol> test_symbols_;
};

// 测试新浪提供者
TEST_F(ProvidersTest, SinaProviderBasics) {
    SinaProvider provider;
    
    EXPECT_EQ(provider.get_name(), "SinaFinance");
    EXPECT_EQ(provider.get_priority(), 10);
    EXPECT_EQ(provider.get_rate_limit(), 100);
    
    // 测试健康检查（模拟）
    // 注意：实际测试时可能需要网络连接
    // EXPECT_TRUE(provider.health_check());
}

// 测试腾讯提供者
TEST_F(ProvidersTest, TencentProviderBasics) {
    TencentProvider provider;
    
    EXPECT_EQ(provider.get_name(), "TencentFinance");
    EXPECT_EQ(provider.get_priority(), 15);
    EXPECT_EQ(provider.get_rate_limit(), 60);
}

// 测试网易提供者
TEST_F(ProvidersTest, NeteaseProviderBasics) {
    NeteaseProvider provider;
    
    EXPECT_EQ(provider.get_name(), "NeteaseFinance");
    EXPECT_EQ(provider.get_priority(), 5);
    EXPECT_EQ(provider.get_rate_limit(), 120);
}

// 测试提供者优先级排序
TEST_F(ProvidersTest, ProviderPriorities) {
    auto netease = std::make_unique<NeteaseProvider>();
    auto sina = std::make_unique<SinaProvider>();
    auto tencent = std::make_unique<TencentProvider>();
    
    std::vector<std::unique_ptr<DataProvider>> providers;
    providers.push_back(std::move(sina));
    providers.push_back(std::move(tencent));
    providers.push_back(std::move(netease));
    
    // 按优先级排序（数字越小优先级越高）
    std::sort(providers.begin(), providers.end(),
        [](const auto& a, const auto& b) {
            return a->get_priority() < b->get_priority();
        });
    
    // 验证排序结果
    EXPECT_EQ(providers[0]->get_name(), "NeteaseFinance");  // priority = 5
    EXPECT_EQ(providers[1]->get_name(), "SinaFinance");     // priority = 10
    EXPECT_EQ(providers[2]->get_name(), "TencentFinance");  // priority = 15
}

// 测试符号格式转换
TEST_F(ProvidersTest, SymbolFormatConversion) {
    for (const auto& symbol : test_symbols_) {
        // 测试各种格式转换
        std::string sina_format = DataTypeUtils::to_sina_format(symbol);
        std::string tencent_format = DataTypeUtils::to_tencent_format(symbol);
        std::string netease_format = DataTypeUtils::to_netease_format(symbol);
        
        // 基本验证：格式不应该为空
        EXPECT_FALSE(sina_format.empty());
        EXPECT_FALSE(tencent_format.empty());
        EXPECT_FALSE(netease_format.empty());
        
        // 验证包含原始代码
        EXPECT_TRUE(sina_format.find(symbol.code) != std::string::npos ||
                   tencent_format.find(symbol.code) != std::string::npos ||
                   netease_format.find(symbol.code) != std::string::npos);
    }
}


// 集成测试：多提供者协作
TEST_F(ProvidersTest, MultiProviderIntegration) {
    std::vector<std::unique_ptr<DataProvider>> providers;
    providers.push_back(std::make_unique<NeteaseProvider>());
    providers.push_back(std::make_unique<SinaProvider>());
    providers.push_back(std::make_unique<TencentProvider>());
    
    // 测试每个提供者的基本功能
    for (const auto& provider : providers) {
        EXPECT_FALSE(provider->get_name().empty());
        EXPECT_GT(provider->get_rate_limit(), 0);
        EXPECT_GE(provider->get_priority(), 0);
    }
    
    // 模拟负载均衡场景
    std::map<std::string, int> usage_count;
    const int total_requests = 100;
    
    for (int i = 0; i < total_requests; ++i) {
        // 模拟轮询选择提供者
        int provider_index = i % providers.size();
        std::string provider_name = providers[provider_index]->get_name();
        usage_count[provider_name]++;
    }
    
    // 验证负载分布
    for (const auto& [name, count] : usage_count) {
        EXPECT_GT(count, 0);
        std::cout << "Provider " << name << " used " << count << " times" << std::endl;
    }
}

// 性能基准测试
TEST_F(ProvidersTest, PerformanceBenchmark) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 测试符号转换性能
    for (int i = 0; i < iterations; ++i) {
        for (const auto& symbol : test_symbols_) {
            auto sina_format = DataTypeUtils::to_sina_format(symbol);
            auto tencent_format = DataTypeUtils::to_tencent_format(symbol);
            (void)sina_format; (void)tencent_format; // 避免编译器优化
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avg_time = static_cast<double>(duration.count()) / (iterations * test_symbols_.size() * 2);
    std::cout << "Average symbol conversion time: " << avg_time << " microseconds" << std::endl;
    
    // 性能应该在合理范围内
    EXPECT_LT(avg_time, 10.0); // 每次转换少于10微秒
}

// 错误处理测试
TEST_F(ProvidersTest, ErrorHandling) {
    // 测试无效符号
    Symbol invalid_symbol("INVALID", Market::SH, SecurityType::STOCK);
    
    // 格式转换应该能处理无效输入而不崩溃
    EXPECT_NO_THROW({
        auto sina_format = DataTypeUtils::to_sina_format(invalid_symbol);
        auto tencent_format = DataTypeUtils::to_tencent_format(invalid_symbol);
    });
    
}

} // namespace cppshares::data::providers::tests