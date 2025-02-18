/*
 * MIT License
 *
 * Copyright (c) 2025 Filipp Andjelo
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
#include <sqlite3pp/Database.hpp>

#include <iostream>
#include <vector>

int main(int argc, const char** argv) {

    // How to create and fill a database
    sqlite3pp::Database db{":memory:"};
    db.execute("CREATE TABLE foo (id, name)");
    db.execute("INSERT INTO foo VALUES (1,'one'), (2,'two'), (3,'three')");

    // How to read values from the database into an int
    std::cout << db.execute<int>("SELECT id FROM foo LIMIT 1") << '\n';

    // How to read values from the database into a string
    std::cout << db.execute<std::string>("SELECT name FROM foo LIMIT 1") << '\n';

    // How to read values from the database into a vector
    for (const auto& id : db.execute<std::vector<int>>("SELECT id FROM foo")) {
        std::cout << id << '\n';
    }

    // How to read values from the database into a vector of pairs
    for (const auto& [id, name] : db.execute<std::vector<std::pair<int, std::string>>>("SELECT id, name FROM foo")) {
        std::cout << id << ',' << name << '\n';
    }

    // How to read values from the database into a map
    std::cout << db.execute<std::map<int, std::string>>("SELECT id, name FROM foo")[1] << '\n';

    // How to use prepared statement
    const auto stmt = db.prepare("SELECT name FROM foo WHERE id >= ?");
    stmt.bind(1, 2);
    for (const auto& name : stmt.execute<std::vector<std::string>>()) {
        std::cout << name << '\n';
    }

    // How to use transaction
    db.transaction([](const auto& t) {
        t.execute("INSERT INTO foo VALUES (4,'four')");
        t.execute("INSERT INTO foo VALUES (5,'five')");
    });

    return 0;
}
