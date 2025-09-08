#include "cppshares/utils/logger.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace cppshares::utils {

void Logger::log_json_response(const std::string& provider_name,
                               const std::string& operation_type,
                               const std::string& symbol,
                               const std::string& json_response) {
    try {
        // 创建logs目录（如果不存在）
        std::filesystem::create_directories("logs/json_responses");

        // 生成文件名：logs/json_responses/{provider}_{operation}_{symbol}_{timestamp}.json
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::ostringstream filename;
        filename << "logs/json_responses/" << provider_name << "_" << operation_type << "_"
                 << symbol << "_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << "_"
                 << std::setfill('0') << std::setw(3) << ms.count() << ".json";

        // 写入JSON响应到文件
        std::ofstream file(filename.str());
        if (file.is_open()) {
            file << json_response;
            file.close();

            // 记录到文本日志
            Logger::debug("JSON response saved: {}", filename.str());
        } else {
            Logger::error("Failed to create JSON response file: {}", filename.str());
        }
    } catch (const std::exception& e) {
        Logger::error("Error saving JSON response: {}", e.what());
    }
}

}  // namespace cppshares::utils