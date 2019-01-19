/**
 * @file result_set.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-18
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/ext/sqlite3/result_set.hpp>

namespace tlab::ext::sqlite3 {

result_set::result_set(sqlite3_stmt *stmt) noexcept : _stmt(stmt) {}

result_set::result_set(const result_set &rhs) noexcept : _stmt(rhs._stmt) {}

result_set::result_set(result_set &&rhs) noexcept : _stmt(rhs._stmt) {}

bool result_set::next(void) {
    if (_stmt) {
        return sqlite3_step(_stmt) == SQLITE_ROW;
    }
    return false;
}

} // namespace tlab::ext::sqlite3