#include <gtest/gtest.h>

#include "cppshares/core/engine.hpp"

namespace cppshares::core::tests {

class TradingEngineTest : public ::testing::Test {
protected:
    void SetUp() override { engine_ = std::make_unique<TradingEngine>(); }

    void TearDown() override {
        if (engine_) {
            engine_->stop();
        }
    }

    std::unique_ptr<TradingEngine> engine_;
};

TEST_F(TradingEngineTest, Initialize) {
    ASSERT_NO_THROW(engine_->initialize());
}

TEST_F(TradingEngineTest, StartStop) {
    engine_->initialize();
    ASSERT_NO_THROW(engine_->start());
    ASSERT_NO_THROW(engine_->stop());
}

}  // namespace cppshares::core::tests