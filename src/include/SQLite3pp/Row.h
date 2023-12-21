#pragma once

#include "BaseDefs.h"

#include <string>
#include <vector>
#include <utility>

namespace SQLite3pp {

SQLITE3PP_EXPORT class Row {
public:
    explicit Row(sqlite3_stmt* stmt) : m_stmt{stmt} {}

    template <typename T>
    T get(std::size_t index) const {
        T result{};
        get(index, result);
        return result;
    }

private:
    sqlite3_stmt* m_stmt;

    template <typename K, typename V>
    std::size_t get(std::size_t index, std::pair<K, V>& result) const {
        return get(get(index, result.first), result.second);
    }

    std::size_t get(std::size_t index, int& value) const;
    std::size_t get(std::size_t index, double& value) const;
    std::size_t get(std::size_t index, std::string& value) const;
    std::size_t get(std::size_t index, std::vector<char>& value) const;
};

} // namespace SQLite3pp
