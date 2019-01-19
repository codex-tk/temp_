/**
 * @file prepared_statement.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-19
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/ext/sqlite3/prepared_statement.hpp>

namespace tlab::ext::sqlite3 {

prepared_statement::prepared_statement(connection &conn)
    : _connection(conn), _stmt(nullptr) {}

prepared_statement::~prepared_statement(void) {}

bool prepared_statement::prepare(const std::string &sql, std::error_code &ec) {
    close();
    int r = sqlite3_prepare_v2(_connection.handle(), sql.c_str(),
                               static_cast<int>(sql.length()), &_stmt, nullptr);
    if (r != 0) {
        ec = std::error_code(r, tlab::ext::sqlite3::error_category::instance());
        close();
        return false;
    }
    return true;
}

bool prepared_statement::bind_parameter(std::size_t col, const int value) {
    if (_stmt)
        return sqlite3_bind_int(_stmt, static_cast<int>(col), value) == 0;
    return false;
}

bool prepared_statement::bind_parameter(std::size_t col,
                                        const std::string &value) {
    if (_stmt)
        return sqlite3_bind_text(_stmt, static_cast<int>(col), value.c_str(),
                                 static_cast<int>(value.length()),
                                 SQLITE_TRANSIENT) == 0;
    return false;
}

result_set prepared_statement::execute_query(void) { return result_set(_stmt); }

bool prepared_statement::execute_non_query(std::error_code &ec) {
    if (_stmt) {
        int r = sqlite3_step(_stmt);
        switch (r) {
        case SQLITE_DONE: return true;
        default:
            ec = std::error_code(
                r, tlab::ext::sqlite3::error_category::instance());
        }
    }
    ec = std::make_error_code(std::errc::owner_dead);
    return false;
}

void prepared_statement::reset(void) {
    if (_stmt)
        sqlite3_reset(_stmt);
}

void prepared_statement::clear_bindings(void) {
    if (_stmt)
        sqlite3_clear_bindings(_stmt);
}

void prepared_statement::close(void) {
    if (_stmt)
        sqlite3_finalize(_stmt);
    _stmt = nullptr;
}

} // namespace tlab::ext::sqlite3
