#pragma once

#include "DatabaseError.h"

#include <sqlite3.h>

#include <memory>
#include <string>
#include <vector>

class PreparedStatement {
public:

    class Row {
    public:
        explicit Row(sqlite3_stmt* stmt) : m_stmt{stmt} {}
        template <typename T> T get(size_t index) const;
    private:
        sqlite3_stmt* m_stmt;
    };

    PreparedStatement(std::shared_ptr<sqlite3> db, const std::string& sql) 
    : m_db{std::move(db)} {
        sqlite3_stmt* stmt{nullptr};
        const auto err = sqlite3_prepare_v2(m_db.get(), sql.c_str(), sql.size(), &stmt, nullptr);
        m_stmt = PointerType { stmt, [](auto* stmt) { sqlite3_finalize(stmt); }};
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
        auto deleter = [](void* ptr) { delete [] static_cast<char*>(ptr); };
        if (SQLITE_OK != sqlite3_bind_text(m_stmt.get(), index, data, value.size(), deleter)) {
            throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
        }
    }

    void bind(size_t index, const std::vector<char>& value) const {
        auto err = SQLITE_OK;
        if (value.size() > 0) {
            auto* data = new char[value.size()];
            auto deleter = [](void* ptr) { delete [] static_cast<char*>(ptr); };
            err = sqlite3_bind_blob(m_stmt.get(), index, data, value.size(), deleter);
        }
        else {
            err = sqlite3_bind_null(m_stmt.get(), index);
        }

        if (SQLITE_OK != err) {
            throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
        }
    }

    void reset() const {
        sqlite3_reset(m_stmt.get());
    }

    void execute() const {
        execute([](const auto&){});
    }

    template <typename T>
    void execute(T&& handler) const {
        while(true) {
            switch(sqlite3_step(m_stmt.get())) {
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

private:
    using PointerType = std::unique_ptr<sqlite3_stmt, void(*)(sqlite3_stmt*)>;

    std::shared_ptr<sqlite3> m_db;
    PointerType m_stmt{nullptr, nullptr};
};

template<>
int PreparedStatement::Row::get<int>(size_t index) const {
    if (SQLITE_INTEGER != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    return sqlite3_column_int(m_stmt, index);
}

template<>
double PreparedStatement::Row::get<double>(size_t index) const {
    if (SQLITE_FLOAT != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    return sqlite3_column_double(m_stmt, index);
}

template<>
std::string PreparedStatement::Row::get<std::string>(size_t index) const {
    if (SQLITE_TEXT != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    const auto length = sqlite3_column_bytes(m_stmt, index);
    const auto* text = sqlite3_column_text(m_stmt, index);
    return { text, text + length };
}

template<>
std::vector<char> PreparedStatement::Row::get<std::vector<char>>(size_t index) const {
    if (SQLITE_BLOB != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    const auto length = sqlite3_column_bytes(m_stmt, index);
    if (length == 0) {
        return {};
    }
    const auto* data = static_cast<const char*>(sqlite3_column_blob(m_stmt, index));
    return { data, data + length };
}

class Database
{
public:

    enum class Mode {
        read = 0x1,
        write = 0x2,
    };

    Database(const std::string& fileName, Mode mode) {

        const auto flag = [mode]() {
            if (mode == Mode::read) {
                return SQLITE_OPEN_READONLY;
            }
            return SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
        }();

        sqlite3* db{nullptr};
        const auto err = sqlite3_open_v2(fileName.c_str(), &db, flag, nullptr);
        m_db = std::shared_ptr<sqlite3>{ db, [](auto* db) { sqlite3_close_v2(db); } };
        if (SQLITE_OK != err) {                
            sqlite3_close_v2(db);
            throw OpenDatabaseError{fileName};
        }
    }

    PreparedStatement prepare(const std::string& sql) const {
        return PreparedStatement{m_db, sql};
    }

private:
    std::shared_ptr<sqlite3> m_db;
};

