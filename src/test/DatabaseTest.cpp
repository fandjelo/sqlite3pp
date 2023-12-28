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
#include <sqlite3pp/Error.h>

#include <fstream>
#include <memory>

#include <gtest/gtest.h>

using namespace sqlite3pp;

struct DatabaseTest : public ::testing::Test {

    std::unique_ptr<Database> db;

    void SetUp() override { db = std::make_unique<Database>(":memory:"); }

    void TearDown() override { db.reset(); }
};

TEST_F(DatabaseTest, OpenCreateDatabase) {

    static const auto* dbFile = "test.db";
    std::remove(dbFile);
    auto uri = [](const auto& mode) { return std::string{"file:"} + dbFile + "?mode=" + mode; };
    ASSERT_THROW(Database(uri("ro")), OpenDatabaseError);
    ASSERT_THROW(Database(uri("rw")), OpenDatabaseError);
    ASSERT_NO_THROW(Database(uri("rwc")));
    ASSERT_NO_THROW(Database(uri("ro")));
    ASSERT_NO_THROW(Database(uri("rw")));
}

TEST_F(DatabaseTest, SimpleExecute) {

    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a,b,c)"));
    ASSERT_THROW(db->execute("INSERT INTO foo VALUES"), PrepareStatementError);
    ASSERT_NO_THROW(db->execute("INSERT INTO foo VALUES (1,2,3),(4,5,6)"));
    ASSERT_EQ(2, db->execute<int>("SELECT count(*) FROM foo"));
}

TEST_F(DatabaseTest, BindParams) {

    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a,b,c,d)"));
    const auto stmt = db->prepare("INSERT INTO foo VALUES (?,?,?,?)");
    ASSERT_NO_THROW(stmt.bind(1, 42));
    ASSERT_NO_THROW(stmt.bind(2, "Hello World"));
    ASSERT_NO_THROW(stmt.bind(3, 3.14));
    ASSERT_NO_THROW(stmt.bind(4, Blob({0xDE, 0xAD, 0xBE, 0xAF})));
    ASSERT_THROW(stmt.bind(5, 0), BindParameterError);
    ASSERT_NO_THROW(stmt.execute());
    db->execute("SELECT * FROM foo", [](const Row& row) {
        EXPECT_EQ(42, row.get<int>(0));
        EXPECT_EQ("Hello World", row.get<std::string>(1));
        EXPECT_EQ(3.14, row.get<double>(2));
        EXPECT_EQ(Blob({0xDE, 0xAD, 0xBE, 0xAF}), row.get<Blob>(3));
        EXPECT_THROW(row.get<int>(1), TypeMismatchError);
        EXPECT_THROW(row.get<int>(4), Error);
    });
}

TEST_F(DatabaseTest, ExtractVector) {

    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a,b,c)"));
    ASSERT_NO_THROW(db->execute("INSERT INTO foo VALUES (1,1.0,'one'),(2,2.0,'two'),(3,3.0,'three')"));

    using T1 = std::vector<int>;
    EXPECT_EQ(T1({1, 2, 3}), db->execute<T1>("SELECT a FROM foo"));

    using T2 = std::vector<double>;
    EXPECT_EQ(T2({1.0, 2.0, 3.0}), db->execute<T2>("SELECT b FROM foo"));

    using T3 = std::vector<std::string>;
    EXPECT_EQ(T3({"one", "two", "three"}), db->execute<T3>("SELECT c FROM foo"));
}

TEST_F(DatabaseTest, ExtractPair) {
    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a,b)"));
    ASSERT_NO_THROW(db->execute("INSERT INTO foo VALUES (1,'one'),(2,'two'),(3,'three')"));
    using T = std::pair<int, std::string>;
    EXPECT_EQ(T(1, "one"), db->execute<T>("SELECT * FROM foo LIMIT 1"));
    EXPECT_EQ(T(3, "three"), db->execute<T>("SELECT * FROM foo"));
}

TEST_F(DatabaseTest, ExtractMap) {
    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a,b,c)"));
    ASSERT_NO_THROW(db->execute("INSERT INTO foo VALUES (1,1.0,'one'),(2,2.0,'two'),(3,3.0,'three')"));

    using T1 = std::map<int, std::string>;
    EXPECT_EQ(T1({{1, "one"}, {2, "two"}, {3, "three"}}), db->execute<T1>("SELECT a,c FROM foo"));

    using T2 = std::map<std::pair<int, double>, std::string>;
    EXPECT_EQ(T2({{{1, 1}, "one"}, {{2, 2}, "two"}, {{3, 3}, "three"}}), db->execute<T2>("SELECT a,b,c FROM foo"));

    using T3 = std::map<int, std::pair<double, std::string>>;
    EXPECT_EQ(T3({{1, {1, "one"}}, {2, {2, "two"}}, {3, {3, "three"}}}), db->execute<T3>("SELECT a,b,c FROM foo"));
}

TEST_F(DatabaseTest, ExtractSet) {
    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a,b)"));
    ASSERT_NO_THROW(db->execute("INSERT INTO foo VALUES (1,'one'),(2,'two'),(3,'one')"));

    using T1 = std::set<std::string>;
    EXPECT_EQ(T1({"one", "two"}), db->execute<T1>("SELECT b FROM foo"));

    using T2 = std::set<std::pair<int, std::string>>;
    EXPECT_EQ(T2({{1, "one"}, {2, "two"}, {3, "one"}}), db->execute<T2>("SELECT a,b FROM foo"));
}

TEST_F(DatabaseTest, ExtractBlob) {
    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a)"));
    ASSERT_NO_THROW(db->execute("INSERT INTO foo VALUES (X'DEADBEAF')"));
    EXPECT_EQ(Blob({0xDE, 0xAD, 0xBE, 0xAF}), db->execute<Blob>("SELECT a FROM foo"));
}

TEST_F(DatabaseTest, TransactionConflict) {
    ASSERT_THROW(db->transaction([](const auto& db) { db.execute("BEGIN"); }), Error);
}

TEST_F(DatabaseTest, TransactionRollback) {

    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a,b)"));
    // Workaround: declaring transaction outside of the macro to avoid errors
    // when compiling on MSVC
    auto batch = [](const sqlite3pp::Database& db) {
        ASSERT_NO_THROW(db.execute("INSERT INTO foo VALUES (1,'one')"));
        ASSERT_NO_THROW(db.execute("INSERT INTO foo VALUES (2,'two')"));
        ASSERT_NO_THROW(db.execute("INSERT INTO foo VALUES (3,'three')"));
        throw 42;
    };
    ASSERT_THROW(db->transaction(batch), int);
    ASSERT_EQ(0, db->execute<int>("SELECT count(*) FROM foo"));
}

TEST_F(DatabaseTest, TransactionCommit) {

    ASSERT_NO_THROW(db->execute("CREATE TABLE foo(a,b)"));
    auto batch = [](const sqlite3pp::Database& db) {
        ASSERT_NO_THROW(db.execute("INSERT INTO foo VALUES (1,'one')"));
        ASSERT_NO_THROW(db.execute("INSERT INTO foo VALUES (2,'two')"));
        ASSERT_NO_THROW(db.execute("INSERT INTO foo VALUES (3,'three')"));
    };
    ASSERT_NO_THROW(db->transaction(batch));
    ASSERT_EQ(3, db->execute<int>("SELECT count(*) FROM foo"));
}
