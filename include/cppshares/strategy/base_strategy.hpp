#pragma once

#include <string>
#include <vector>

namespace cppshares::strategy {

class BaseStrategy {
public:
    explicit BaseStrategy(const std::string& name);
    virtual ~BaseStrategy() = default;

    virtual void initialize() = 0;
    virtual void on_market_data() = 0;
    virtual void on_order_update() = 0;

    const std::string& name() const { return name_; }

protected:
    std::string name_;
    bool initialized_ = false;
};

}  // namespace cppshares::strategy