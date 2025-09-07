#include "cppshares/data/data_strategy.hpp"

#include <algorithm>
#include <random>

namespace cppshares::data {

// FailoverStrategy 实现
std::vector<std::shared_ptr<DataProvider>> FailoverStrategy::select_providers(
    DataType data_type,
    const std::vector<std::shared_ptr<DataProvider>>& available_providers) {
    auto sorted_providers = available_providers;
    std::sort(sorted_providers.begin(), sorted_providers.end(), [](const auto& a, const auto& b) {
        return a->get_priority() < b->get_priority();
    });

    return sorted_providers;
}

// RoundRobinStrategy 实现
std::vector<std::shared_ptr<DataProvider>> RoundRobinStrategy::select_providers(
    DataType data_type,
    const std::vector<std::shared_ptr<DataProvider>>& available_providers) {
    if (available_providers.empty())
        return {};

    std::lock_guard<std::mutex> lock(mutex_);

    std::string key = std::to_string(static_cast<int>(data_type));
    size_t& index = next_provider_index_[key];

    if (index >= available_providers.size()) {
        index = 0;
    }

    std::vector<std::shared_ptr<DataProvider>> result;
    result.push_back(available_providers[index]);
    index = (index + 1) % available_providers.size();

    return result;
}

// WeightedRandomStrategy 实现
std::vector<std::shared_ptr<DataProvider>> WeightedRandomStrategy::select_providers(
    DataType data_type,
    const std::vector<std::shared_ptr<DataProvider>>& available_providers) {
    if (available_providers.empty())
        return {};

    // 简单的随机选择，可以根据成功率加权
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, available_providers.size() - 1);

    std::vector<std::shared_ptr<DataProvider>> result;
    result.push_back(available_providers[dis(gen)]);

    return result;
}

// DataAggregator 实现
void DataAggregator::register_provider(std::shared_ptr<DataProvider> provider) {
    std::lock_guard<std::mutex> lock(mutex_);
    providers_.push_back(provider);
}

void DataAggregator::unregister_provider(const std::string& provider_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    providers_.erase(std::remove_if(providers_.begin(),
                                    providers_.end(),
                                    [&provider_name](const auto& provider) {
                                        return provider->get_name() == provider_name;
                                    }),
                     providers_.end());
}

void DataAggregator::set_strategy(std::unique_ptr<DataStrategy> strategy) {
    std::lock_guard<std::mutex> lock(mutex_);
    strategy_ = std::move(strategy);
}

std::optional<MarketTick> DataAggregator::get_realtime_quote(const Symbol& symbol) {
    return try_get_data<MarketTick>(
        DataType::REALTIME_QUOTE,
        [&symbol](std::shared_ptr<DataProvider> provider) -> std::optional<MarketTick> {
            return provider->get_realtime_quote(symbol);
        });
}

std::vector<OHLCV> DataAggregator::get_kline_data(const Symbol& symbol,
                                                  KlinePeriod period,
                                                  int limit) {
    auto result = try_get_data<std::vector<OHLCV>>(
        DataType::KLINE_DATA,
        [&](std::shared_ptr<DataProvider> provider) -> std::optional<std::vector<OHLCV>> {
            auto data = provider->get_kline_data(symbol, period, limit);
            return data.empty() ? std::nullopt : std::make_optional(data);
        });

    return result.value_or(std::vector<OHLCV>{});
}

void DataAggregator::update_provider_health() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& provider : providers_) {
        bool is_healthy = provider->health_check();
        // 更新健康状态逻辑可以在这里实现
    }
}

std::map<std::string, ProviderHealth> DataAggregator::get_provider_health() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<std::string, ProviderHealth> health_map;

    for (const auto& provider : providers_) {
        ProviderHealth health;
        health.status = ProviderStatus::UNKNOWN;
        health_map[provider->get_name()] = health;
    }

    return health_map;
}

DataAggregator::Statistics DataAggregator::get_statistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

template <typename T>
std::optional<T> DataAggregator::try_get_data(
    DataType data_type,
    std::function<std::optional<T>(std::shared_ptr<DataProvider>)> getter) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!strategy_ || providers_.empty()) {
        return std::nullopt;
    }

    auto selected_providers = strategy_->select_providers(data_type, providers_);

    for (auto& provider : selected_providers) {
        stats_.total_requests++;

        try {
            auto result = getter(provider);
            if (result) {
                stats_.successful_requests++;
                record_success(provider->get_name());
                return result;
            }
        } catch (...) {
            record_failure(provider->get_name());
        }
    }

    stats_.failed_requests++;
    return std::nullopt;
}

void DataAggregator::record_success(const std::string& provider_name) {
    stats_.provider_usage_count[provider_name]++;
}

void DataAggregator::record_failure(const std::string& provider_name) {
    // 记录失败统计
}

void DataAggregator::update_provider_metrics(std::shared_ptr<DataProvider> provider,
                                             bool success,
                                             int64_t response_time_ms) {
    // 更新提供者指标
}

}  // namespace cppshares::data