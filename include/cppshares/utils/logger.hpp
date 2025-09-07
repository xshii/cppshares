#pragma once

#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace cppshares::utils {

// 二进制日志记录头部
struct BinaryLogEntry {
    uint64_t timestamp_us;  // 8字节 - 微秒级时间戳
    uint32_t event_type;    // 4字节 - 事件类型ID
    uint32_t data_size;     // 4字节 - 数据部分大小
    // 后续跟随变长数据部分
} __attribute__((packed));

// 市场数据记录
struct MarketDataRecord {
    static constexpr uint32_t TYPE_ID = 0x1001;

    uint32_t symbol_id;  // 4字节 - 符号ID
    double price;        // 8字节 - 价格
    uint64_t volume;     // 8字节 - 成交量
    uint8_t side;        // 1字节 - 买卖方向 0=买 1=卖
    uint8_t padding[3];  // 3字节 - 内存对齐填充
} __attribute__((packed));

// 订单记录
struct OrderRecord {
    static constexpr uint32_t TYPE_ID = 0x2001;

    uint64_t order_id;   // 8字节
    uint32_t symbol_id;  // 4字节
    double price;        // 8字节
    uint32_t quantity;   // 4字节
    uint8_t side;        // 1字节
    uint8_t status;      // 1字节 0=NEW 1=FILLED 2=CANCELLED
    uint8_t padding[2];  // 2字节
} __attribute__((packed));

// 策略信号记录
struct StrategySignalRecord {
    static constexpr uint32_t TYPE_ID = 0x3001;

    uint32_t strategy_id;      // 4字节 - 策略ID
    uint32_t symbol_id;        // 4字节 - 符号ID
    uint8_t signal_type;       // 1字节 - 信号类型 0=BUY 1=SELL 2=HOLD
    uint8_t confidence;        // 1字节 - 置信度 0-100
    double target_price;       // 8字节 - 目标价格
    uint32_t target_quantity;  // 4字节 - 目标数量
    uint8_t padding[2];        // 2字节
} __attribute__((packed));

// 自定义二进制Sink
template <typename Mutex>
class binary_sink : public spdlog::sinks::base_sink<Mutex> {
private:
    std::ofstream binary_file_;
    static constexpr size_t BUFFER_SIZE = 64 * 1024;  // 64KB缓冲区
    std::vector<char> write_buffer_;
    size_t buffer_pos_ = 0;

public:
    explicit binary_sink(const std::string& filename)
        : binary_file_(filename, std::ios::binary | std::ios::app), write_buffer_(BUFFER_SIZE) {}

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        BinaryLogEntry header{
            .timestamp_us = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::microseconds>(msg.time.time_since_epoch())
                    .count()),
            .event_type = hash_logger_name(msg.logger_name.data()),
            .data_size = static_cast<uint32_t>(msg.payload.size())};

        write_to_buffer(reinterpret_cast<const char*>(&header), sizeof(header));
        write_to_buffer(msg.payload.data(), msg.payload.size());
    }

    void flush_() override {
        flush_buffer();
        binary_file_.flush();
    }

private:
    uint32_t hash_logger_name(const char* name) const {
        // 简单的字符串哈希，用于确定事件类型
        uint32_t hash = 5381;
        while (*name) {
            hash = ((hash << 5) + hash) + *name++;
        }
        return hash;
    }

    void write_to_buffer(const char* data, size_t size) {
        if (buffer_pos_ + size > BUFFER_SIZE) {
            flush_buffer();
        }
        std::memcpy(write_buffer_.data() + buffer_pos_, data, size);
        buffer_pos_ += size;
    }

    void flush_buffer() {
        if (buffer_pos_ > 0) {
            binary_file_.write(write_buffer_.data(), buffer_pos_);
            buffer_pos_ = 0;
        }
    }
};

using binary_sink_mt = binary_sink<std::mutex>;
using binary_sink_st = binary_sink<spdlog::details::null_mutex>;

// 纯二进制日志器
class BinaryLogger {
private:
    std::ofstream binary_file_;
    std::mutex write_mutex_;
    static constexpr size_t BUFFER_SIZE = 64 * 1024;  // 64KB缓冲区
    std::vector<char> write_buffer_;
    size_t buffer_pos_ = 0;

public:
    explicit BinaryLogger(const std::string& filename)
        : binary_file_(filename, std::ios::binary | std::ios::app), write_buffer_(BUFFER_SIZE) {}

    ~BinaryLogger() { flush_buffer(); }

    template <typename RecordType>
    void log_binary(const RecordType& record) {
        BinaryLogEntry header{.timestamp_us = static_cast<uint64_t>(
                                  std::chrono::duration_cast<std::chrono::microseconds>(
                                      std::chrono::high_resolution_clock::now().time_since_epoch())
                                      .count()),
                              .event_type = RecordType::TYPE_ID,
                              .data_size = sizeof(RecordType)};

        std::lock_guard<std::mutex> lock(write_mutex_);

        write_to_buffer(reinterpret_cast<const char*>(&header), sizeof(header));
        write_to_buffer(reinterpret_cast<const char*>(&record), sizeof(record));

        if (buffer_pos_ > BUFFER_SIZE - 1024) {
            flush_buffer();
        }
    }

    void flush() {
        std::lock_guard<std::mutex> lock(write_mutex_);
        flush_buffer();
    }

private:
    void write_to_buffer(const char* data, size_t size) {
        if (buffer_pos_ + size > BUFFER_SIZE) {
            flush_buffer();
        }
        std::memcpy(write_buffer_.data() + buffer_pos_, data, size);
        buffer_pos_ += size;
    }

    void flush_buffer() {
        if (buffer_pos_ > 0) {
            binary_file_.write(write_buffer_.data(), buffer_pos_);
            binary_file_.flush();
            buffer_pos_ = 0;
        }
    }
};

// 混合日志器
class HybridLogger {
private:
    std::shared_ptr<spdlog::logger> text_logger_;
    std::unique_ptr<BinaryLogger> binary_logger_;
    std::unordered_map<uint32_t, std::string> symbol_map_;

public:
    HybridLogger(const std::string& text_log_path = "logs/system.log",
                 const std::string& binary_log_path = "logs/market_data.bin") {
        // 创建文本日志器（spdlog）
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            text_log_path, 5 * 1024 * 1024, 3);  // 5MB, 3个文件

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        text_logger_ = std::make_shared<spdlog::logger>("cppshares", sinks.begin(), sinks.end());
        text_logger_->set_level(spdlog::level::info);
        text_logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");

        // 创建二进制日志器
        binary_logger_ = std::make_unique<BinaryLogger>(binary_log_path);
    }

    // 文本日志接口（委托给spdlog）
    template <typename... Args>
    void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        text_logger_->info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        text_logger_->warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        text_logger_->error(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        text_logger_->debug(fmt, std::forward<Args>(args)...);
    }

    // 二进制日志接口
    template <typename RecordType>
    void log_data(const RecordType& record) {
        binary_logger_->log_binary(record);
    }

    // 便利方法：记录市场数据
    void log_market_data(uint32_t symbol_id, double price, uint64_t volume, bool is_buy) {
        MarketDataRecord record{.symbol_id = symbol_id,
                                .price = price,
                                .volume = volume,
                                .side = static_cast<uint8_t>(is_buy ? 0 : 1),
                                .padding = {0, 0, 0}};
        log_data(record);
    }

    // 便利方法：记录订单
    void log_order(uint64_t order_id,
                   uint32_t symbol_id,
                   double price,
                   uint32_t quantity,
                   bool is_buy,
                   uint8_t status) {
        OrderRecord record{.order_id = order_id,
                           .symbol_id = symbol_id,
                           .price = price,
                           .quantity = quantity,
                           .side = static_cast<uint8_t>(is_buy ? 0 : 1),
                           .status = status,
                           .padding = {0, 0}};
        log_data(record);
    }

    // 便利方法：记录策略信号
    void log_strategy_signal(uint32_t strategy_id,
                             uint32_t symbol_id,
                             uint8_t signal_type,
                             uint8_t confidence,
                             double target_price,
                             uint32_t target_quantity) {
        StrategySignalRecord record{.strategy_id = strategy_id,
                                    .symbol_id = symbol_id,
                                    .signal_type = signal_type,
                                    .confidence = confidence,
                                    .target_price = target_price,
                                    .target_quantity = target_quantity,
                                    .padding = {0, 0}};
        log_data(record);
    }

    void flush() {
        text_logger_->flush();
        binary_logger_->flush();
    }

    // 符号映射管理
    void register_symbol(uint32_t symbol_id, const std::string& symbol) {
        symbol_map_[symbol_id] = symbol;
    }

    const std::string& get_symbol_name(uint32_t symbol_id) const {
        static const std::string unknown = "UNKNOWN";
        auto it = symbol_map_.find(symbol_id);
        return it != symbol_map_.end() ? it->second : unknown;
    }
};

// 全局日志器实例
class Logger {
public:
    static HybridLogger& instance() {
        static HybridLogger logger;
        return logger;
    }

    static void initialize(const std::string& text_log_path = "logs/system.log",
                           const std::string& binary_log_path = "logs/market_data.bin") {
        // 创建日志目录
        std::filesystem::create_directories("logs");
        instance() = HybridLogger(text_log_path, binary_log_path);
    }

    // 静态便利方法
    template <typename... Args>
    static void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        instance().info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        instance().error(fmt, std::forward<Args>(args)...);
    }

    template <typename RecordType>
    static void log_data(const RecordType& record) {
        instance().log_data(record);
    }
};

}  // namespace cppshares::utils