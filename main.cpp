#include "Database.h"

#include <iostream>
#include <memory>

int main(int argc, const char** argv)
{
    Database db{"test.db", Database::Mode::write};
    db.prepare("CREATE TABLE foo (a,b,c)").execute();
    db.prepare("INSERT INTO foo VALUES (1,2,3)").execute();
    const auto stmt = db.prepare("INSERT INTO foo VALUES (?,?,?)");
    for (auto i = 0; i < 10; i++) {
        stmt.bind(1, i);
        stmt.bind(2, double(i));
        stmt.bind(3, "Entry " + std::to_string(i));
        stmt.execute();
        stmt.reset();
    }

    return 0;
}
