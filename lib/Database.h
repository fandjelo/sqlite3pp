#pragma once

#include "DatabaseError.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;

class Row {
public:
    explicit Row(sqlite3_stmt* stmt) : m_stmt{stmt} {}

    template <typename T>
    T get(size_t index) const {
        T result{};
        get(index, result);
        return result;
    }

private:
    sqlite3_stmt* m_stmt;

    template <typename K, typename V>
    size_t get(size_t index, std::pair<K, V>& result) const {
        return get(get(index, result.first), result.second);
    }

    size_t get(size_t index, int& value) const;
    size_t get(size_t index, double& value) const;
    size_t get(size_t index, std::string& value) const;
    size_t get(size_t index, std::vector<char>& value) const;
};

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

class Database {
public:
    Database(const std::string& uri);

    Statement prepare(const std::string& sql) const { return Statement{m_db, sql}; }

    void execute(const std::string& sql) const { prepare(sql).execute(); }

    template <typename T>
    void execute(const std::string& sql, T&& action) const {
        prepare(sql).execute(std::forward<T>(action));
    }

    template <typename T>
    T execute(const std::string& sql) const {
        return prepare(sql).execute<T>();
    }

    template <typename T>
    void transaction(T&& actions) const {
        execute("BEGIN");
        // Misuse unique_ptr as a scope guard to do rollback on error
        using Guard = std::unique_ptr<const Database, void (*)(const Database*)>;
        auto guard = Guard{this, [](const auto* db) { db->execute("ROLLBACK"); }};
        actions(*this);
        guard.release()->execute("COMMIT");
    }

private:
    std::shared_ptr<sqlite3> m_db;
};

