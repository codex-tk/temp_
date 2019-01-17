/**
 * @file connection.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-17
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/ext/sqlite3/connection.hpp>
#include <tlab/ext/sqlite3/error_category.hpp>

namespace tlab::ext::sqlite3 {

connection::connection(void) : _sqlite3(nullptr) {}

connection::~connection(void) { close(); }

bool connection::open(const std::string &info) {
    std::error_code ec;
    return open(info, ec);
}

bool connection::open(const std::string &info, std::error_code &ec) {
    close();
    int r = sqlite3_open(info.c_str(), &_sqlite3);
    if (r != 0) {
        ec = std::error_code(r, sqlite3::error_category::instance());
        return false;
    }
    return true;
}

void connection::close(void) {
    if (_sqlite3)
        sqlite3_close(_sqlite3);
    _sqlite3 = nullptr;
}

::sqlite3* connection::handle(void){
    return _sqlite3;
}

} // namespace tlab::ext::sqlite3