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
