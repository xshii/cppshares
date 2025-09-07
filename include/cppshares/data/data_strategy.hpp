#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include "data_types.hpp"
#include "market_data.hpp"

namespace cppshares::data {

// API提供者状态
enum class ProviderStatus {
    UNKNOWN,
    HEALTHY,
    DEGRADED,     // 部分功能可用
    FAILED,       // 完全不可用
    RATE_LIMITED  // 触发限流
};

// API提供者健康状态
struct ProviderHealth {
    ProviderStatus status = ProviderStatus::UNKNOWN;
    std::chrono::system_clock::time_point last_success;
    std::chrono::system_clock::time_point last_failure;
    int consecutive_failures = 0;
    double success_rate = 0.0;  // 最近成功率
    int64_t avg_response_time_ms = 0;
};

// 数据提供者基类
class DataProvider {
public:
    virtual ~DataProvider() = default;

    // 获取实时行情
    virtual std::optional<MarketTick> get_realtime_quote(const Symbol& symbol) = 0;

    // 获取K线数据
    virtual std::vector<OHLCV> get_kline_data(const Symbol& symbol,
                                              KlinePeriod period = KlinePeriod::DAY_1,
                                              int limit = 100) = 0;

    // 获取提供者信息
    virtual std::string get_name() const = 0;
    virtual int get_priority() const = 0;    // 优先级，数字越小优先级越高
    virtual int get_rate_limit() const = 0;  // 每分钟请求限制

    // 健康检查
    virtual bool health_check() = 0;

protected:
    mutable std::mutex mutex_;
    ProviderHealth health_;
};

// 数据获取策略基类
class DataStrategy {
public:
    virtual ~DataStrategy() = default;

    // 选择最佳的数据提供者
    virtual std::vector<std::shared_ptr<DataProvider>> select_providers(
        DataType data_type,
        const std::vector<std::shared_ptr<DataProvider>>& available_providers) = 0;

    virtual std::string get_strategy_name() const = 0;
};

// 故障转移策略
class FailoverStrategy : public DataStrategy {
public:
    std::vector<std::shared_ptr<DataProvider>> select_providers(
        DataType data_type,
        const std::vector<std::shared_ptr<DataProvider>>& available_providers) override;

    std::string get_strategy_name() const override { return "Failover"; }
};

// 轮询负载均衡策略
class RoundRobinStrategy : public DataStrategy {
public:
    std::vector<std::shared_ptr<DataProvider>> select_providers(
        DataType data_type,
        const std::vector<std::shared_ptr<DataProvider>>& available_providers) override;

    std::string get_strategy_name() const override { return "RoundRobin"; }

private:
    mutable std::mutex mutex_;
    mutable std::map<std::string, size_t> next_provider_index_;
};

// 加权随机策略（根据成功率选择）
class WeightedRandomStrategy : public DataStrategy {
public:
    std::vector<std::shared_ptr<DataProvider>> select_providers(
        DataType data_type,
        const std::vector<std::shared_ptr<DataProvider>>& available_providers) override;

    std::string get_strategy_name() const override { return "WeightedRandom"; }
};

// 聚合数据管理器
class DataAggregator {
public:
    DataAggregator() = default;
    ~DataAggregator() = default;

    // 注册数据提供者
    void register_provider(std::shared_ptr<DataProvider> provider);
    void unregister_provider(const std::string& provider_name);

    // 设置获取策略
    void set_strategy(std::unique_ptr<DataStrategy> strategy);

    // 数据获取方法（带故障转移）
    std::optional<MarketTick> get_realtime_quote(const Symbol& symbol);
    std::vector<OHLCV> get_kline_data(const Symbol& symbol,
                                      KlinePeriod period = KlinePeriod::DAY_1,
                                      int limit = 100);

    // 健康状态管理
    void update_provider_health();
    std::map<std::string, ProviderHealth> get_provider_health() const;

    // 统计信息
    struct Statistics {
        size_t total_requests = 0;
        size_t successful_requests = 0;
        size_t failed_requests = 0;
        std::map<std::string, size_t> provider_usage_count;
    };
    Statistics get_statistics() const;

private:
    mutable std::mutex mutex_;
    std::vector<std::shared_ptr<DataProvider>> providers_;
    std::unique_ptr<DataStrategy> strategy_;
    Statistics stats_;

    // 辅助方法
    template <typename T>
    std::optional<T> try_get_data(
        DataType data_type,
        std::function<std::optional<T>(std::shared_ptr<DataProvider>)> getter);

    void record_success(const std::string& provider_name);
    void record_failure(const std::string& provider_name);
    void update_provider_metrics(std::shared_ptr<DataProvider> provider,
                                 bool success,
                                 int64_t response_time_ms);
};

}  // namespace cppshares::data