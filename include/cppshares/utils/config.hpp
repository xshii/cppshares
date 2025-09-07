#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace cppshares::utils {

class Config {
public:
    static Config& instance();
    
    void load_from_file(const std::string& filename);
    void save_to_file(const std::string& filename) const;
    
    template<typename T>
    T get(const std::string& key, const T& default_value = T{}) const {
        try {
            return config_.at(key).get<T>();
        } catch (...) {
            return default_value;
        }
    }
    
    template<typename T>
    void set(const std::string& key, const T& value) {
        config_[key] = value;
    }

private:
    nlohmann::json config_;
    Config() = default;
};

} // namespace cppshares::utils