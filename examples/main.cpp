#include "Database.h"

#include <iostream>

int main(int argc, const char** argv) {

    Database db { "foo.db", Database::Mode::write };
    db.execute("PRAGMA multiplex_enabled=1");
    db.execute("PRAGMA multiplex_chunksize=1024");
    db.execute("BEGIN");
    db.execute("CREATE TABLE foo (blob)");
    std::vector<char> data(1024 * 1024);
    const auto stmt = db.prepare("INSERT INTO foo VALUES (?)");
    stmt.bind(1, data);
    for (auto i = 0u; i < 5; i++) {
        stmt.execute();
    }
    db.execute("COMMIT");

    return 0;
}
