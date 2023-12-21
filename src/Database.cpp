#include <SQLite3pp/Database.h>
#include <SQLite3pp/Error.h>
#include <sqlite3.h>

namespace SQLite3pp {

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

} // namespace SQLite3pp
