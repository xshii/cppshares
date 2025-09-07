#pragma once

#include <string>
#include <vector>

namespace cppshares::data {

class Database {
public:
    Database() = default;
    ~Database() = default;

    bool connect(const std::string& db_path);
    void disconnect();
    bool is_connected() const;

private:
    bool connected_ = false;
    std::string db_path_;
};

}  // namespace cppshares::data