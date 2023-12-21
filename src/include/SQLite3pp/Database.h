#pragma once

#include "BaseDefs.h"
#include "Statement.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace SQLite3pp {

SQLITE3PP_EXPORT
class Database {
public:
    explicit Database(const std::string& uri);

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

} // namespace SQLite3pp
