#pragma once

#include "BaseDefs.h"
#include "Row.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace sqlite3pp {

SQLITE3PP_EXPORT
class Statement {
public:
    Statement(std::shared_ptr<sqlite3> db, const std::string& sql);

    void bind(size_t index, int value) const;
    void bind(size_t index, double value) const;
    void bind(size_t index, const std::string& value) const;
    void bind(size_t index, const std::vector<char>& value) const;

    template <typename Handler>
    void execute(Handler&& handler) const {
        while (hasNext()) {
            handler(Row{m_stmt.get()});
        }
    }

    void execute() const {
        execute([](const auto&) {});
    }

    template <typename T>
    T execute() const {
        T result{};
        execute([&result](const auto& row) { get(row, result); });
        return result;
    }

private:
    template <typename T>
    static void get(const Row& row, T& result) {
        result = row.get<T>(0);
    }

    template <typename T>
    static void get(const Row& row, std::vector<T>& results) {
        results.push_back(row.get<T>(0));
    }

    template <typename T>
    static void get(const Row& row, std::set<T>& results) {
        results.insert(row.get<T>(0));
    }

    template <typename K, typename V>
    static void get(const Row& row, std::map<K, V>& results) {
        results.insert(row.get<std::pair<K, V>>(0));
    }

    std::shared_ptr<sqlite3> m_db;
    std::unique_ptr<sqlite3_stmt, void (*)(sqlite3_stmt*)> m_stmt{nullptr, nullptr};

    bool hasNext() const;
};

} // namespace sqlite3pp
