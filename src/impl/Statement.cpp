#include <sqlite3.h>
#include <sqlite3pp/Error.h>
#include <sqlite3pp/Statement.h>

namespace sqlite3pp {

Statement::Statement(std::shared_ptr<sqlite3> db, const std::string& sql) : m_db{std::move(db)} {
    sqlite3_stmt* stmt{nullptr};
    const auto err = sqlite3_prepare_v2(m_db.get(), sql.c_str(), sql.size(), &stmt, nullptr);
    m_stmt = {stmt, [](auto* stmt) { sqlite3_finalize(stmt); }};
    if (SQLITE_OK != err) {
        throw PrepareStatementError{sqlite3_errmsg(m_db.get()), sql};
    }
}

void Statement::bind(std::size_t index, int value) const {
    if (SQLITE_OK != sqlite3_bind_int(m_stmt.get(), index, value)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(std::size_t index, double value) const {
    if (SQLITE_OK != sqlite3_bind_double(m_stmt.get(), index, value)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(std::size_t index, const std::string& value) const {
    if (SQLITE_OK != sqlite3_bind_text(m_stmt.get(), index, value.data(), value.size(), SQLITE_TRANSIENT)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(std::size_t index, const std::vector<char>& value) const {
    if (SQLITE_OK != sqlite3_bind_blob(m_stmt.get(), index, value.data(), value.size(), SQLITE_TRANSIENT)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

bool Statement::hasNext() const {
    switch (sqlite3_step(m_stmt.get())) {
    case SQLITE_DONE:
        return false;
    case SQLITE_ROW:
        return true;
    default:
        throw Error{"Failed in step"};
    }
}

} // namespace sqlite3pp
