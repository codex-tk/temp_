#include <gtest/gtest.h>
#include <tlab/ext/sqlite3/connection.hpp>
#include <tlab/ext/sqlite3/error_category.hpp>
#include <tlab/ext/sqlite3/prepared_statement.hpp>
#include <tlab/ext/sqlite3/statement.hpp>
#include <tlab/ext/sqlite3/transaction.hpp>

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

    tlab::ext::sqlite3::result_set rs =
        stmt.execute_query("select * from sample", ec);
    ASSERT_TRUE(!ec);
    int proc = 0;
    while (rs.next(ec)) {
        std::tuple<int, std::string> row = rs.get_row<int, std::string>();
        int id = std::get<0>(row);
        ASSERT_STREQ(std::get<1>(row).c_str(), test_values[id].c_str());
        ++proc;
    }
    ASSERT_TRUE(!ec);
    ASSERT_EQ(proc, 2);
    stmt.close();
    _conn.close();
}

TEST_F(sqlite3_fixture, prepared_stmt) {
    std::error_code ec;
    ASSERT_TRUE(_conn.open("./sample.db", ec));
    do {
        tlab::ext::sqlite3::statement stmt(_conn);
        ASSERT_TRUE(stmt.execute("DROP TABLE IF EXISTS sample", ec));
        ASSERT_TRUE(
            stmt.execute("CREATE TABLE IF NOT EXISTS sample([id] INTEGER  "
                         "NOT NULL PRIMARY KEY , value TEXT )",
                         ec));
        stmt.close();
    } while (0);

    do {
        tlab::ext::sqlite3::prepared_statement insert_stmt(_conn);
        ASSERT_TRUE(
            insert_stmt.prepare("insert into sample values(?,?)", ec));

        for (auto it : test_values) {
            insert_stmt.reset();
            insert_stmt.clear_bindings();
            ASSERT_TRUE(insert_stmt.bind(it.first, it.second));
            ASSERT_TRUE(insert_stmt.execute_non_query());
        }

        insert_stmt.close();
    } while (0);

    do {
        tlab::ext::sqlite3::statement stmt(_conn);
        tlab::ext::sqlite3::result_set rs =
            stmt.execute_query("select count(*) from sample", ec);

        ASSERT_FALSE(ec);

        ASSERT_TRUE(rs.next());
        ASSERT_EQ(std::get<0>(rs.get_row<int>()), 2);
        ASSERT_FALSE(rs.next());
        stmt.close();
    } while (0);

    tlab::ext::sqlite3::prepared_statement pstmt(_conn);
    ASSERT_TRUE(
        pstmt.prepare("select * from sample where id = ? and value = ?", ec));

    for (auto it : test_values) {
        pstmt.reset();
        pstmt.clear_bindings();
        ASSERT_TRUE(pstmt.bind(it.first, it.second));
        tlab::ext::sqlite3::result_set rs = pstmt.execute_query();
        ASSERT_TRUE(rs.next(ec));

        std::tuple<int, std::string> row = rs.get_row<int, std::string>();

        ASSERT_EQ(std::get<0>(row), it.first);
        ASSERT_EQ(std::get<1>(row), it.second);

        ASSERT_FALSE(rs.next(ec));
        ASSERT_FALSE(ec);
    }

    pstmt.close();
    _conn.close();
}

TEST_F(sqlite3_fixture, transaction) {
    std::error_code ec;
    ASSERT_TRUE(_conn.open("./sample.db", ec));
    tlab::ext::sqlite3::statement stmt(_conn);
    do {
        ASSERT_TRUE(stmt.execute("DROP TABLE IF EXISTS sample", ec));
        ASSERT_TRUE(
            stmt.execute("CREATE TABLE IF NOT EXISTS sample([id] INTEGER  "
                         "NOT NULL PRIMARY KEY , value TEXT )",
                         ec));
        stmt.close();
    } while (0);

    do {
        tlab::ext::sqlite3::prepared_statement insert_stmt(_conn);
        ASSERT_TRUE(insert_stmt.prepare("insert into sample values(?,?)", ec));

        tlab::ext::sqlite3::transaction tr(_conn);
        ASSERT_TRUE(tr.begin());

        for (auto it : test_values) {
            insert_stmt.reset();
            insert_stmt.clear_bindings();
            ASSERT_TRUE(insert_stmt.bind(it.first, it.second));
            ASSERT_TRUE(insert_stmt.execute_non_query());
        }

        ASSERT_TRUE(tr.rollback());
        insert_stmt.close();
    } while (0);

    do {
        tlab::ext::sqlite3::result_set rs =
            stmt.execute_query("select count(*) from sample", ec);

        ASSERT_FALSE(ec);
        ASSERT_TRUE(rs.next());
        ASSERT_EQ(std::get<0>(rs.get_row<int>()), 0);
        ASSERT_FALSE(rs.next());
        stmt.close();
    } while (0);

    do {
        tlab::ext::sqlite3::prepared_statement insert_stmt(_conn);
        ASSERT_TRUE(insert_stmt.prepare("insert into sample values(?,?)", ec));

        tlab::ext::sqlite3::transaction tr(_conn);
        ASSERT_TRUE(tr.begin());

        for (auto it : test_values) {
            insert_stmt.reset();
            insert_stmt.clear_bindings();
            ASSERT_TRUE(insert_stmt.bind(it.first, it.second));
            ASSERT_TRUE(insert_stmt.execute_non_query());
        }

        ASSERT_TRUE(tr.commit());
        insert_stmt.close();
    } while (0);

    do {
        tlab::ext::sqlite3::result_set rs =
            stmt.execute_query("select count(*) from sample", ec);

        ASSERT_FALSE(ec);
        ASSERT_TRUE(rs.next());
        ASSERT_EQ(std::get<0>(rs.get_row<int>()), 2);
        ASSERT_FALSE(rs.next());
        stmt.close();
    } while (0);

    _conn.close();
}

TEST(sqlite3, error_category) {
    std::error_code ec(516, tlab::ext::sqlite3::error_category::instance());
    gprintf("%s", ec.message().c_str());
}