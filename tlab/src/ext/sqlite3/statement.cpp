/**
 * @file statement.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-17
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/ext/sqlite3/statement.hpp>

namespace tlab::ext::sqlite3 {

statement::statement(connection &conn) : _connection(conn), _stmt(nullptr) {}

statement::~statement(void) {}

bool statement::execute(const std::string &sql) {
    std::error_code ec;
    return execute(sql, ec);
}

bool statement::execute(const std::string &sql, std::error_code &ec) {
    int r = sqlite3_exec(_connection.handle(), sql.c_str(), nullptr, nullptr,
                         nullptr);
    if (r != 0) {
        ec = std::error_code(r, tlab::ext::sqlite3::error_category::instance());
        return false;
    }
    return true;
}

result_set statement::execute_query(const std::string &sql) {
    std::error_code ec;
    return execute_query(sql, ec);
}

result_set statement::execute_query(const std::string &sql, std::error_code &ec) {
    close();
    int r = sqlite3_prepare_v2(_connection.handle(), sql.c_str(),
                               static_cast<int>(sql.length()), &_stmt, nullptr);
    if (r != 0) {
        ec = std::error_code(r, tlab::ext::sqlite3::error_category::instance());
        close();
        return result_set(nullptr);
    }
    return result_set(_stmt);
}

void statement::close(void) {
    if (_stmt)
        sqlite3_finalize(_stmt);
    _stmt = nullptr;
}

} // namespace tlab::ext::sqlite3
