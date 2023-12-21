#pragma once

#include "DatabaseError.h"

#include <sqlite3.h>
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

    PreparedStatement(sqlite3* db, const std::string& sql) : m_db{db} {
        if (SQLITE_OK != sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &m_stmt, nullptr)) {
            const auto what = std::string{sqlite3_errmsg(m_db)};
            sqlite3_finalize(m_stmt);
            throw PrepareStatementError{what, sql};
        }
    }

    PreparedStatement(PreparedStatement&& that) noexcept {
        std::swap(m_stmt, that.m_stmt);
    }

    PreparedStatement& operator=(PreparedStatement&& that) noexcept {
        std::swap(m_stmt, that.m_stmt);
        return *this;
    }

    PreparedStatement(const PreparedStatement&) = delete;
    PreparedStatement& operator=(const PreparedStatement&) = delete;
    ~PreparedStatement() noexcept {
        sqlite3_finalize(m_stmt);
    }

    void bind(size_t index, int value) const {
        if (SQLITE_OK != sqlite3_bind_int(m_stmt, index, value)) {
            throw BindParameterError{sqlite3_errmsg(m_db), index};
        }
    }

    void bind(size_t index, double value) const {
        if (SQLITE_OK != sqlite3_bind_double(m_stmt, index, value)) {
            throw BindParameterError{sqlite3_errmsg(m_db), index};
        }
    }

    void bind(size_t index, const std::string& value) const {
        auto* data = new char[value.size()];
        std::copy(std::begin(value), std::end(value), data);
        auto deleter = [](void* ptr) { delete [] static_cast<char*>(ptr); };
        if (SQLITE_OK != sqlite3_bind_text(m_stmt, index, data, value.size(), deleter)) {
            throw BindParameterError{sqlite3_errmsg(m_db), index};
        }
    }

    void bind(size_t index, const std::vector<char>& value) const {
        auto err = SQLITE_OK;
        if (value.size() > 0) {
            auto* data = new char[value.size()];
            auto deleter = [](void* ptr) { delete [] static_cast<char*>(ptr); };
            err = sqlite3_bind_blob(m_stmt, index, data, value.size(), deleter);
        }
        else {
            err = sqlite3_bind_null(m_stmt, index);
        }

        if (SQLITE_OK != err) {
            throw BindParameterError{sqlite3_errmsg(m_db), index};
        }
    }

    void reset() const {
        sqlite3_reset(m_stmt);
    }

    void execute() const {
        execute([](const auto&){});
    }

    template <typename T>
    void execute(T&& handler) const {
        while(true) {
            switch(sqlite3_step(m_stmt)) {
                case SQLITE_DONE:
                    return;
                case SQLITE_ROW:
                    handler(Row{m_stmt});
                    break;
                default:
                    throw DatabaseError("Failed in step");
            }
        }
    }

private:
    sqlite3* m_db{nullptr};
    sqlite3_stmt* m_stmt{nullptr};
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

        if (SQLITE_OK != sqlite3_open_v2(fileName.c_str(), &m_db, flag, nullptr)) {
            sqlite3_close_v2(m_db);
            throw OpenDatabaseError{fileName};
        }
    }

    Database(Database&& that) noexcept {
        std::swap(m_db, that.m_db);
    }

    Database& operator=(Database&& that) noexcept {
        std::swap(m_db, that.m_db);
        return *this;
    }

    ~Database() {
        sqlite3_close_v2(m_db);
    }

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    PreparedStatement prepare(const std::string& sql) const {
        return PreparedStatement{m_db, sql};
    }

private:
    sqlite3* m_db{nullptr};
};

