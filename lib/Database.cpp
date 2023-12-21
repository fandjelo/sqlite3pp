#include "Database.h"

#include <sqlite3.h>

size_t Row::get(size_t index, int& value) const {
    if (SQLITE_INTEGER != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    value = sqlite3_column_int(m_stmt, index);
    return index + 1;
}

size_t Row::get(size_t index, double& value) const {
    if (SQLITE_FLOAT != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    value = sqlite3_column_double(m_stmt, index);
    return index + 1;
}

size_t Row::get(size_t index, std::string& value) const {
    if (SQLITE_TEXT != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    const auto length = sqlite3_column_bytes(m_stmt, index);
    const auto* text = sqlite3_column_text(m_stmt, index);
    value.assign(text, text + length);
    return index + 1;
}

size_t Row::get(size_t index, std::vector<char>& value) const {
    if (SQLITE_BLOB != sqlite3_column_type(m_stmt, index)) {
        throw TypeMismatchError{index};
    }
    const auto length = sqlite3_column_bytes(m_stmt, index);
    const auto* data = static_cast<const char*>(sqlite3_column_blob(m_stmt, index));
    value.assign(data, data + length);
    return index + 1;
}

Statement::Statement(std::shared_ptr<sqlite3> db, const std::string& sql) : m_db{std::move(db)} {
    sqlite3_stmt* stmt{nullptr};
    const auto err = sqlite3_prepare_v2(m_db.get(), sql.c_str(), sql.size(), &stmt, nullptr);
    m_stmt = {stmt, [](auto* stmt) { sqlite3_finalize(stmt); }};
    if (SQLITE_OK != err) {
        const auto what = std::string{sqlite3_errmsg(m_db.get())};
        throw PrepareStatementError{what, sql};
    }
}

void Statement::bind(size_t index, int value) const {
    if (SQLITE_OK != sqlite3_bind_int(m_stmt.get(), index, value)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(size_t index, double value) const {
    if (SQLITE_OK != sqlite3_bind_double(m_stmt.get(), index, value)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(size_t index, const std::string& value) const {
    auto* data = new char[value.size()];
    std::copy(std::begin(value), std::end(value), data);
    auto deleter = [](void* ptr) { delete[] static_cast<char*>(ptr); };
    if (SQLITE_OK != sqlite3_bind_text(m_stmt.get(), index, data, value.size(), deleter)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(size_t index, const std::vector<char>& value) const {

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

void Statement::reset() const { sqlite3_reset(m_stmt.get()); }

void Statement::execute(std::function<void(const Row&)> handler) const {
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

Database::Database(const std::string& fileName, Mode mode) {

    const auto flag = (mode == Mode::read) ? SQLITE_OPEN_READONLY : SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;

    sqlite3* db{nullptr};
    const auto err = sqlite3_open_v2(fileName.c_str(), &db, flag, nullptr);
    m_db = std::shared_ptr<sqlite3>{db, [](auto* db) { sqlite3_close_v2(db); }};
    if (SQLITE_OK != err) {
        throw OpenDatabaseError{fileName, sqlite3_errmsg(db)};
    }
}
