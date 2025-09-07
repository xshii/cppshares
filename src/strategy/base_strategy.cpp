#include "cppshares/strategy/base_strategy.hpp"

#include "cppshares/utils/logger.hpp"

namespace cppshares::strategy {

BaseStrategy::BaseStrategy(const std::string& name) : name_(name) {
    cppshares::utils::Logger::info("Strategy '{}' created", name_);
}

}  // namespace cppshares::strategy