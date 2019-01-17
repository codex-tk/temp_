#include <gtest/gtest.h>
#include <tlab/ext/sqlite3/connection.hpp>
#include <tlab/ext/sqlite3/error_category.hpp>
#include <tlab/ext/sqlite3/statement.hpp>

void gprintf(const char *fmt, ...);

class sqlite3_fixture : public testing::Test {
protected:
    // Sets up the test fixture.
    virtual void SetUp() {
#if defined(_WIN32)
        char dirbuff[1024] = {
            0,
        };
        GetCurrentDirectoryA(1024, dirbuff);
        gprintf("%s", dirbuff);
#endif
        std::error_code ec;
        ASSERT_TRUE(_conn.open("./sample.db", ec));
    }

    // Tears down the test fixture.
    virtual void TearDown() { _conn.close(); }

    tlab::ext::sqlite3::connection _conn;
};

TEST_F(sqlite3_fixture, open) {}

TEST_F(sqlite3_fixture, stmt) {
    std::error_code ec;
    tlab::ext::sqlite3::statement stmt(_conn);
    ASSERT_TRUE(stmt.execute("DROP TABLE IF EXISTS sample", ec));
    ASSERT_TRUE(stmt.execute("CREATE TABLE IF NOT EXISTS sample([id] INTEGER  "
                             "NOT NULL PRIMARY KEY , value TEXT )",
                             ec));

    ASSERT_TRUE(stmt.execute("insert into sample values( 0 , 'text' )", ec));
    ASSERT_TRUE(stmt.execute("insert into sample values( 1 , 'text1' )", ec));

    std::vector<std::tuple<int, std::string>> rs;
    stmt.execute_query("select * from sample", rs, ec);
    ASSERT_EQ(rs.size(), 2);
    ASSERT_EQ(std::get<0>(rs[0]), 0);
    ASSERT_STREQ(std::get<1>(rs[0]).c_str(), "text");

    ASSERT_EQ(std::get<0>(rs[1]), 1);
    ASSERT_STREQ(std::get<1>(rs[1]).c_str(), "text1");
}

TEST(sqlite3, error_category) {
    std::error_code ec(516, tlab::ext::sqlite3::error_category::instance());
    gprintf("%s", ec.message().c_str());
}