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
#include <sqlite3pp/Database.h>
#include <sqlite3pp/Error.h>
#include <sqlite3.h>

namespace sqlite3pp {

Database::Database(const std::string& uri) {

    sqlite3* db{nullptr};
    const auto flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
    const auto err = sqlite3_open_v2(uri.c_str(), &db, flags, nullptr);
    // sqlite3 allocate resources even if the open operation failed, so we need to
    // release those resources in any way, even if the returned error code was not
    // good. That's why we first initialize the shared_ptr and then check for error
    m_db = std::shared_ptr<sqlite3>{db, [](auto* db) { sqlite3_close_v2(db); }};
    if (SQLITE_OK != err) {
        throw OpenDatabaseError{uri, sqlite3_errmsg(db)};
    }
}

} // namespace sqlite3pp
