#include "Database.h"

#include <iostream>

int main(int argc, const char** argv) {

    Database db{"foo.db"};
    db.execute("CREATE TABLE foo (a,b,c)");
    db.execute("INSERT INTO foo VALUES (1,2,3), (2,3,4), (3,4,5)");
    for (const auto& entry : db.execute<std::vector<int>>("SELECT a FROM foo")) {
        std::cout << entry << '\n';
    }
    return 0;
}
