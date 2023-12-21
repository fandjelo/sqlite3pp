#include "Database.h"

#include <iostream>

int main(int argc, const char** argv) {

    Database db{":memory:", Database::Mode::write};
    db.prepare("CREATE TABLE foo (a,b,c)").execute();
    const auto stmt = db.prepare("INSERT INTO foo VALUES (?,?,?)");
    for (auto i = 0; i < 10; i++) {
        stmt.bind(1, i);
        stmt.bind(2, double(i));
        stmt.bind(3, "Entry " + std::to_string(i));
        stmt.execute();
        stmt.reset();
    }

    std::cout << "--------\n";

    db.execute("SELECT * FROM foo", [](const Row& row) {
        std::cout << row.get<int>(0) << ',';
        std::cout << row.get<double>(1) << ',';
        std::cout << row.get<std::string>(2) << '\n';
    });

    const auto results = db.execute<std::vector<std::pair<int, std::string>>>("SELECT a,c FROM foo");
    for (const auto& result : results) {
        std::cout << result.first << ", " << result.second << '\n';
    }


    return 0;
}
