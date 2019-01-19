#include <gtest/gtest.h>
#include <tlab/ext/sqlite3/connection.hpp>
#include <tlab/ext/sqlite3/error_category.hpp>
#include <tlab/ext/sqlite3/statement.hpp>

void gprintf(const char *fmt, ...);

class sqlite3_fixture : public testing::Test {
protected:
    // Sets up the test fixture.
    virtual void SetUp() {
        test_values.insert(std::make_pair<int, std::string>(0, "text"));
        test_values.insert(std::make_pair<int, std::string>(1, "text1"));
    }

    // Tears down the test fixture.
    virtual void TearDown() {}

    tlab::ext::sqlite3::connection _conn;
    std::map<int, std::string> test_values;
};

TEST_F(sqlite3_fixture, open) {
    std::error_code ec;
    ASSERT_TRUE(_conn.open("./sample.db", ec));
    _conn.close();
}

TEST_F(sqlite3_fixture, stmt_execute_query_simple) {
    std::error_code ec;
    ASSERT_TRUE(_conn.open("./sample.db", ec));
    tlab::ext::sqlite3::statement stmt(_conn);
    ASSERT_TRUE(stmt.execute("DROP TABLE IF EXISTS sample", ec));
    ASSERT_TRUE(stmt.execute("CREATE TABLE IF NOT EXISTS sample([id] INTEGER  "
                             "NOT NULL PRIMARY KEY , value TEXT )",
                             ec));

    ASSERT_TRUE(stmt.execute("insert into sample values( 0 , 'text' )", ec));
    ASSERT_TRUE(stmt.execute("insert into sample values( 1 , 'text1' )", ec));

    do {
        std::vector<std::tuple<int, std::string>> id_value_records;
        ASSERT_TRUE(stmt.execute_query_simple("select * from sample",
                                              id_value_records, ec));
        ASSERT_EQ(id_value_records.size(), test_values.size());
        for (auto r : id_value_records) {
            int id = std::get<0>(r);
            ASSERT_STREQ(std::get<1>(r).c_str(), test_values[id].c_str());
        }
    } while (0);

    do {
        std::vector<std::tuple<std::string, int>> value_id_records;
        ASSERT_TRUE(stmt.execute_query_simple("select value,[id] from sample",
                                              value_id_records, ec));
        ASSERT_EQ(value_id_records.size(), test_values.size());
        for (auto r : value_id_records) {
            int id = std::get<1>(r);
            ASSERT_STREQ(std::get<0>(r).c_str(), test_values[id].c_str());
        }
    } while (0);
    _conn.close();
}

TEST_F(sqlite3_fixture, stmt_execute_query) {
    std::error_code ec;
    ASSERT_TRUE(_conn.open("./sample.db", ec));
    tlab::ext::sqlite3::statement stmt(_conn);
    ASSERT_TRUE(stmt.execute("DROP TABLE IF EXISTS sample", ec));
    ASSERT_TRUE(stmt.execute("CREATE TABLE IF NOT EXISTS sample([id] INTEGER  "
                             "NOT NULL PRIMARY KEY , value TEXT )",
                             ec));

    ASSERT_TRUE(stmt.execute("insert into sample values( 0 , 'text' )", ec));
    ASSERT_TRUE(stmt.execute("insert into sample values( 1 , 'text1' )", ec));

    tlab::ext::sqlite3::result_set rs = stmt.execute_query("select * from sample" , ec);
    ASSERT_TRUE(!ec);
    int proc = 0;
    while(rs.next()){
        std::tuple<int,std::string> row = rs.get_row<int,std::string>();
        int id = std::get<0>(row);
        ASSERT_STREQ(std::get<1>(row).c_str(), test_values[id].c_str());
        ++proc;
    }
    ASSERT_EQ(proc,2);
    stmt.close();
    _conn.close();
}
TEST(sqlite3, error_category) {
    std::error_code ec(516, tlab::ext::sqlite3::error_category::instance());
    gprintf("%s", ec.message().c_str());
}