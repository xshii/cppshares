#pragma once

#include "logger.hpp"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <format>
#include <functional>

namespace cppshares::utils {

class BinaryLogReader {
private:
    std::ifstream binary_file_;
    std::unordered_map<uint32_t, std::string> symbol_map_;
    std::unordered_map<uint32_t, std::string> strategy_map_;

public:
    explicit BinaryLogReader(const std::string& filename) 
        : binary_file_(filename, std::ios::binary) {
        if (!binary_file_.is_open()) {
            throw std::runtime_error("Failed to open binary log file: " + filename);
        }
    }
    
    // 加载符号映射
    void load_symbol_map(const std::string& symbol_file) {
        std::ifstream file(symbol_file);
        std::string line;
        while (std::getline(file, line)) {
            auto pos = line.find(',');
            if (pos != std::string::npos) {
                uint32_t id = std::stoul(line.substr(0, pos));
                std::string symbol = line.substr(pos + 1);
                symbol_map_[id] = symbol;
            }
        }
    }
    
    // 手动注册符号
    void register_symbol(uint32_t symbol_id, const std::string& symbol) {
        symbol_map_[symbol_id] = symbol;
    }
    
    // 注册策略
    void register_strategy(uint32_t strategy_id, const std::string& strategy_name) {
        strategy_map_[strategy_id] = strategy_name;
    }
    
    // 导出所有数据到CSV
    void export_all_to_csv(const std::string& csv_filename) {
        std::ofstream csv_file(csv_filename);
        csv_file << "timestamp,event_type,data\n";
        
        export_records(csv_file, [](uint32_t) { return true; });
    }
    
    // 导出特定事件类型到CSV
    void export_to_csv(const std::string& csv_filename, uint32_t event_type_filter) {
        std::ofstream csv_file(csv_filename);
        
        write_csv_header(csv_file, event_type_filter);
        export_records(csv_file, [event_type_filter](uint32_t type) { 
            return type == event_type_filter; 
        });
    }
    
    // 导出市场数据
    void export_market_data_to_csv(const std::string& csv_filename) {
        export_to_csv(csv_filename, MarketDataRecord::TYPE_ID);
    }
    
    // 导出订单数据
    void export_orders_to_csv(const std::string& csv_filename) {
        export_to_csv(csv_filename, OrderRecord::TYPE_ID);
    }
    
    // 导出策略信号
    void export_strategy_signals_to_csv(const std::string& csv_filename) {
        export_to_csv(csv_filename, StrategySignalRecord::TYPE_ID);
    }
    
    // 统计信息
    struct Statistics {
        uint64_t total_records = 0;
        uint64_t market_data_records = 0;
        uint64_t order_records = 0;
        uint64_t strategy_signal_records = 0;
        uint64_t unknown_records = 0;
        std::chrono::system_clock::time_point first_timestamp;
        std::chrono::system_clock::time_point last_timestamp;
    };
    
    Statistics get_statistics() {
        Statistics stats;
        binary_file_.clear();
        binary_file_.seekg(0, std::ios::beg);
        
        BinaryLogEntry header;
        bool first_record = true;
        
        while (binary_file_.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            stats.total_records++;
            
            auto timestamp = std::chrono::system_clock::time_point(
                std::chrono::microseconds(header.timestamp_us));
            
            if (first_record) {
                stats.first_timestamp = timestamp;
                first_record = false;
            }
            stats.last_timestamp = timestamp;
            
            switch (header.event_type) {
                case MarketDataRecord::TYPE_ID:
                    stats.market_data_records++;
                    break;
                case OrderRecord::TYPE_ID:
                    stats.order_records++;
                    break;
                case StrategySignalRecord::TYPE_ID:
                    stats.strategy_signal_records++;
                    break;
                default:
                    stats.unknown_records++;
                    break;
            }
            
            // 跳过数据部分
            binary_file_.seekg(header.data_size, std::ios::cur);
        }
        
        return stats;
    }

private:
    void export_records(std::ofstream& csv_file, std::function<bool(uint32_t)> filter) {
        binary_file_.clear();
        binary_file_.seekg(0, std::ios::beg);
        
        BinaryLogEntry header;
        while (binary_file_.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            if (!filter(header.event_type)) {
                binary_file_.seekg(header.data_size, std::ios::cur);
                continue;
            }
            
            switch (header.event_type) {
                case MarketDataRecord::TYPE_ID:
                    export_market_data_record(csv_file, header);
                    break;
                case OrderRecord::TYPE_ID:
                    export_order_record(csv_file, header);
                    break;
                case StrategySignalRecord::TYPE_ID:
                    export_strategy_signal_record(csv_file, header);
                    break;
                default:
                    // 跳过未知类型
                    binary_file_.seekg(header.data_size, std::ios::cur);
                    break;
            }
        }
    }
    
    void write_csv_header(std::ofstream& csv, uint32_t event_type) {
        switch (event_type) {
            case MarketDataRecord::TYPE_ID:
                csv << "timestamp,symbol,price,volume,side\n";
                break;
            case OrderRecord::TYPE_ID:
                csv << "timestamp,order_id,symbol,price,quantity,side,status\n";
                break;
            case StrategySignalRecord::TYPE_ID:
                csv << "timestamp,strategy,symbol,signal_type,confidence,target_price,target_quantity\n";
                break;
            default:
                csv << "timestamp,event_type,raw_data\n";
                break;
        }
    }
    
    void export_market_data_record(std::ofstream& csv, const BinaryLogEntry& header) {
        MarketDataRecord record;
        binary_file_.read(reinterpret_cast<char*>(&record), sizeof(record));
        
        auto time_str = format_timestamp(header.timestamp_us);
        auto symbol = get_symbol_name(record.symbol_id);
        auto side = record.side == 0 ? "BUY" : "SELL";
        
        // 复制packed字段值避免引用绑定问题
        double price = record.price;
        uint64_t volume = record.volume;
        csv << std::format("{},{},{:.6f},{},{}\n",
            time_str, symbol, price, volume, side);
    }
    
    void export_order_record(std::ofstream& csv, const BinaryLogEntry& header) {
        OrderRecord record;
        binary_file_.read(reinterpret_cast<char*>(&record), sizeof(record));
        
        auto time_str = format_timestamp(header.timestamp_us);
        auto symbol = get_symbol_name(record.symbol_id);
        auto side = record.side == 0 ? "BUY" : "SELL";
        
        std::string status;
        switch (record.status) {
            case 0: status = "NEW"; break;
            case 1: status = "FILLED"; break;
            case 2: status = "CANCELLED"; break;
            default: status = "UNKNOWN"; break;
        }
        
        // 复制packed字段值避免引用绑定问题
        uint64_t order_id = record.order_id;
        double price = record.price;
        uint32_t quantity = record.quantity;
        csv << std::format("{},{},{},{:.6f},{},{},{}\n",
            time_str, order_id, symbol, price, quantity, side, status);
    }
    
    void export_strategy_signal_record(std::ofstream& csv, const BinaryLogEntry& header) {
        StrategySignalRecord record;
        binary_file_.read(reinterpret_cast<char*>(&record), sizeof(record));
        
        auto time_str = format_timestamp(header.timestamp_us);
        auto strategy = get_strategy_name(record.strategy_id);
        auto symbol = get_symbol_name(record.symbol_id);
        
        std::string signal_type;
        switch (record.signal_type) {
            case 0: signal_type = "BUY"; break;
            case 1: signal_type = "SELL"; break;
            case 2: signal_type = "HOLD"; break;
            default: signal_type = "UNKNOWN"; break;
        }
        
        // 复制packed字段值避免引用绑定问题
        uint8_t confidence = record.confidence;
        double target_price = record.target_price;
        uint32_t target_quantity = record.target_quantity;
        csv << std::format("{},{},{},{},{},{:.6f},{}\n",
            time_str, strategy, symbol, signal_type, confidence,
            target_price, target_quantity);
    }
    
    std::string format_timestamp(uint64_t timestamp_us) {
        auto time_point = std::chrono::system_clock::time_point(
            std::chrono::microseconds(timestamp_us));
        auto time_t = std::chrono::system_clock::to_time_t(time_point);
        auto microsec = timestamp_us % 1000000;
        
        return std::format("{:%Y-%m-%d %H:%M:%S}.{:06d}", 
            std::chrono::system_clock::from_time_t(time_t), microsec);
    }
    
    const std::string& get_symbol_name(uint32_t symbol_id) {
        static const std::string unknown = "UNKNOWN";
        auto it = symbol_map_.find(symbol_id);
        return it != symbol_map_.end() ? it->second : unknown;
    }
    
    const std::string& get_strategy_name(uint32_t strategy_id) {
        static const std::string unknown = "UNKNOWN";
        auto it = strategy_map_.find(strategy_id);
        return it != strategy_map_.end() ? it->second : unknown;
    }
};

} // namespace cppshares::utils