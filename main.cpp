#include "Database.h"

#include <iostream>
#include <memory>

PreparedStatement foobar(const std::string& fileName, const std::string& sql) {
    return Database{fileName, Database::Mode::read}.prepare(sql);
}

int main(int argc, const char** argv)
{
    Database db{"test.db", Database::Mode::write};
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

    foobar("test.db", "SELECT * FROM foo").execute([](const PreparedStatement::Row& row) {
            std::cout << row.get<int>(0) << ',';
            std::cout << row.get<double>(1) << ',';
            std::cout << row.get<std::string>(2) << '\n';
    });

    return 0;
}
