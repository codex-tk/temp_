/**
 * @file sqlite3_connection.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-14
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <tlab/db/sqlite3_connection.hpp>
#ifdef TLAB_ENABLE_SQLITE3_DB
namespace tlab::db {

sqlite3_connection::sqlite3_connection(void) : _sqlite3_ptr(nullptr) {}

sqlite3_connection::~sqlite3_connection(void) { close(); }

bool sqlite3_connection::open(const std::string &conn_str,
                              std::error_code &ec) {
    close();
    if(sqlite3_open(conn_str.c_str(),&_sqlite3_ptr) != 0){
        return false;
    }
    return true;
}

void sqlite3_connection::close(void) {
    if (_sqlite3_ptr)
        sqlite3_close_v2(_sqlite3_ptr);
    _sqlite3_ptr = nullptr;
}

} // namespace tlab::db

#endif