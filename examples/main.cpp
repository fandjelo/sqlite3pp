#include <SQLite3pp/Database.h>
#include <iostream>

int main(int argc, const char** argv) {

    SQLite3pp::Database db{":memory:"};
    db.execute("CREATE TABLE foo (a,b,c)");
    db.execute("INSERT INTO foo VALUES (1,2,3), (2,3,4), (3,4,5)");
    const auto results = db.prepare("SELECT a,c FROM foo").execute<std::map<int,int>>();
    for (const auto& result : results) {
        std::cout << result.first << ',' << result.second << '\n';
    }
    return 0;
}
