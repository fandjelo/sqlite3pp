/*
 * MIT License
 *
 * Copyright (c) 2023 Filipp Andjelo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <sqlite3.h>
#include <sqlite3pp/Error.hpp>
#include <sqlite3pp/Statement.hpp>

namespace sqlite3pp {

Statement::Statement(std::shared_ptr<sqlite3> db, const std::string& sql) : m_db{std::move(db)} {
    sqlite3_stmt* stmt{nullptr};
    const auto err = sqlite3_prepare_v2(m_db.get(), sql.c_str(), static_cast<int>(sql.size()), &stmt, nullptr);
    m_stmt = {stmt, [](auto* stmt) { sqlite3_finalize(stmt); }};
    if (SQLITE_OK != err) {
        throw PrepareStatementError{sqlite3_errmsg(m_db.get()), sql};
    }
}

void Statement::bind(std::size_t index, int value) const {
    if (SQLITE_OK != sqlite3_bind_int(m_stmt.get(), static_cast<int>(index), value)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(std::size_t index, double value) const {
    if (SQLITE_OK != sqlite3_bind_double(m_stmt.get(), static_cast<int>(index), value)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(std::size_t index, const std::string& value) const {
    if (SQLITE_OK != sqlite3_bind_text(m_stmt.get(), static_cast<int>(index), value.data(),
                                       static_cast<int>(value.size()), SQLITE_TRANSIENT)) {
        throw BindParameterError{sqlite3_errmsg(m_db.get()), index};
    }
}

void Statement::bind(std::size_t index, const Blob& value) const {
    if (SQLITE_OK != sqlite3_bind_blob(m_stmt.get(), static_cast<int>(index), value.data(),
                                       static_cast<int>(value.size()), SQLITE_TRANSIENT)) {
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
