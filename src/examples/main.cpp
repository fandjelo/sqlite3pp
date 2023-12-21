#include <sqlite3pp/Database.h>
#include <iostream>

int main(int argc, const char** argv) {

    // How to create and fill a database
    sqlite3pp::Database db{":memory:"};
    db.execute("CREATE TABLE foo (a,b,c)");
    db.execute("INSERT INTO foo VALUES (1,2,3), (2,3,4), (3,4,5)");

    // How to read values from the database into a map
    for (const auto& [key, value] : db.execute<std::map<int,int>>("SELECT a,c FROM foo")) {
        std::cout << key << ',' << value << '\n';
    }

    // How to use prepared statement
    const auto stmt = db.prepare("SELECT a FROM foo WHERE c = ? LIMIT 1");
    stmt.bind(1, 5);
    std::cout << stmt.execute<int>() << '\n';
    return 0;
}
