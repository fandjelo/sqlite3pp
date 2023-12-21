#pragma once

#include "DatabaseError.h"
#include "ScopeGuard.h"

#include <sqlite3.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

class Row {
public:
    explicit Row(sqlite3_stmt* stmt) : m_stmt{stmt} {}

    template <typename T>
    T get(size_t index) const {
        T result{};
        extract(index, result);
        return result;
    }

private:
    sqlite3_stmt* m_stmt;

    size_t extract(size_t index, int& value) const {
        if (SQLITE_INTEGER != sqlite3_column_type(m_stmt, index)) {
            throw TypeMismatchError{index};
        }
        value = sqlite3_column_int(m_stmt, index);
        return index + 1;
    }

    size_t extract(size_t index, double& value) const {
        if (SQLITE_FLOAT != sqlite3_column_type(m_stmt, index)) {
            throw TypeMismatchError{index};
        }
        value = sqlite3_column_double(m_stmt, index);
        return index + 1;
    }

    size_t extract(size_t index, std::string& value) const {
        if (SQLITE_TEXT != sqlite3_column_type(m_stmt, index)) {
            throw TypeMismatchError{index};
        }
        const auto length = sqlite3_column_bytes(m_stmt, index);
        const auto* text = sqlite3_column_text(m_stmt, index);
        value.assign(text, text + length);
        return index + 1;
    }

    size_t extract(size_t index, std::vector<char>& value) const {
        if (SQLITE_BLOB != sqlite3_column_type(m_stmt, index)) {
            throw TypeMismatchError{index};
        }
        const auto length = sqlite3_column_bytes(m_stmt, index);
        const auto* data = static_cast<const char*>(sqlite3_column_blob(m_stmt, index));
        value.assign(data, data + length);
        return index + 1;
    }

    template <typename K, typename V>
    size_t extract(size_t index, std::pair<K, V>& result) const {
        K key{};
        index = extract(index, key);
        V value{};
        index = extract(index, value);
        result = {key, value};
        return index;
    }
};

class PreparedStatement {
public:
    PreparedStatement(std::shared_ptr<sqlite3> db, const std::string& sql) : m_db{std::move(db)} {
        sqlite3_stmt* stmt{nullptr};
        const auto err = sqlite3_prepare_v2(m_db.get(), sql.c_str(), sql.size(), &stmt, nullptr);
        m_stmt = PointerType{stmt, [](auto* stmt) { sqlite3_finalize(stmt); }};
        if (SQLITE_OK != err) {
            const auto what = std::string{sqlite3_errmsg(m_db.get())};
            throw PrepareStatementError{what, sql};
        }
    }

    void bind(size_t index, int value) const {
        if (SQLITE_OK != sqlite3_bind_int(m_stmt.get(), index, value)) {
            throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
        }
    }

    void bind(size_t index, double value) const {
        if (SQLITE_OK != sqlite3_bind_double(m_stmt.get(), index, value)) {
            throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
        }
    }

    void bind(size_t index, const std::string& value) const {
        auto* data = new char[value.size()];
        std::copy(std::begin(value), std::end(value), data);
        auto deleter = [](void* ptr) { delete[] static_cast<char*>(ptr); };
        if (SQLITE_OK != sqlite3_bind_text(m_stmt.get(), index, data, value.size(), deleter)) {
            throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
        }
    }

    void bind(size_t index, const std::vector<char>& value) const {

        const auto err = [this, index, &value]() {
            if (value.size() > 0) {
                auto* data = new char[value.size()];
                auto deleter = [](void* ptr) { delete[] static_cast<char*>(ptr); };
                return sqlite3_bind_blob(m_stmt.get(), index, data, value.size(), deleter);
            }
            return sqlite3_bind_null(m_stmt.get(), index);
        }();

        if (SQLITE_OK != err) {
            throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
        }
    }

    void reset() const { sqlite3_reset(m_stmt.get()); }

    template <typename T>
    void execute(T&& handler) const {
        while (true) {
            switch (sqlite3_step(m_stmt.get())) {
            case SQLITE_DONE:
                return;
            case SQLITE_ROW:
                handler(Row{m_stmt.get()});
                break;
            default:
                throw DatabaseError("Failed in step");
            }
        }
    }

    void execute() const {
        execute([](const auto&) {});
    }

    template <typename T>
    T execute() const {
        T result{};
        execute([this, &result](const auto& row) { extract(row, result); });
        return result;
    }

private:
    using PointerType = std::unique_ptr<sqlite3_stmt, void (*)(sqlite3_stmt*)>;

    template <typename T>
    static void extract(const Row& row, T& result) {
        result = row.get<T>(0);
    }

    template <typename T>
    static void extract(const Row& row, std::vector<T>& results) {
        results.push_back(row.get<T>(0));
    }

    template <typename T>
    static void extract(const Row& row, std::set<T>& results) {
        results.insert(row.get<T>(0));
    }

    template <typename K, typename V>
    static void extract(const Row& row, std::map<K, V>& results) {
        results.insert(row.get<std::pair<K, V>>(0));
    }

    std::shared_ptr<sqlite3> m_db;
    PointerType m_stmt{nullptr, nullptr};
};

class Database {
public:
    enum class Mode { read, write };

    Database(const std::string& fileName, Mode mode) {

        const auto flag = (mode == Mode::read) ? SQLITE_OPEN_READONLY : SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;

        sqlite3* db{nullptr};
        const auto err = sqlite3_open_v2(fileName.c_str(), &db, flag, nullptr);
        m_db = std::shared_ptr<sqlite3>{db, [](auto* db) { sqlite3_close_v2(db); }};
        if (SQLITE_OK != err) {
            throw OpenDatabaseError{fileName, sqlite3_errmsg(db)};
        }
    }

    PreparedStatement prepare(const std::string& sql) const { return PreparedStatement{m_db, sql}; }

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
        auto guard = makeScopeGuard([this]() { execute("ROLLBACK"); });
        execute("BEGIN");
        actions(*this);
        guard.released = true;
        execute("COMMIT");
    }

private:
    std::shared_ptr<sqlite3> m_db;
};

