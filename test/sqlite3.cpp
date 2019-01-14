#include <gtest/gtest.h>
#include <tlab/db/sqlite3_connection.hpp>

TEST(tlab,sqlite3){
    tlab::db::sqlite3_connection conn;
    std::error_code ec;
    ASSERT_TRUE(conn.open("./sample.db",ec));
    conn.close();
}