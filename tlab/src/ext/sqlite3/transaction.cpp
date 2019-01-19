/**
 * @file transaction.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-19
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/ext/sqlite3/transaction.hpp>

namespace tlab::ext::sqlite3 {

transaction::transaction(tlab::ext::sqlite3::connection &conn) noexcept
    : _statement(conn) {}

bool transaction::begin(std::error_code &ec) {
    return _statement.execute("BEGIN TRANSACTION", ec);
}

bool transaction::commit(std::error_code &ec) {
    return _statement.execute("COMMIT TRANSACTION", ec);
}

bool transaction::rollback(std::error_code &ec) {
    return _statement.execute("ROLLBACK TRANSACTION", ec);
}

} // namespace tlab::ext::sqlite3