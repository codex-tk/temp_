/**
 * @file prepared_statement.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-19
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_ext_sqlite3_prepared_statement_h__
#define __tlab_ext_sqlite3_prepared_statement_h__

#include <tlab/ext/sqlite3/connection.hpp>
#include <tlab/ext/sqlite3/error_category.hpp>
#include <tlab/ext/sqlite3/result_set.hpp>

#include <tlab/mp.hpp>

namespace tlab::ext::sqlite3 {

class prepared_statement {
public:
    prepared_statement(connection &conn);

    ~prepared_statement(void);

    bool prepare(const std::string &sql) {
        std::error_code ec;
        return prepare(sql, ec);
    }

    bool prepare(const std::string &sql, std::error_code &ec);

    bool bind_parameter(std::size_t col, const int value);

    bool bind_parameter(std::size_t col, const std::string &value);

    template <typename... Ts, std::size_t... S>
    bool bind_expantion(tlab::mp::index_sequence<S...>, Ts &&... args) {
        return (bind_parameter(S + 1, args) | ...);
    }

    template <typename... Ts> bool bind(Ts &&... args) {
        return bind_expantion<Ts...>(
            tlab::mp::make_index_sequence<sizeof...(Ts)>{},
            std::forward<Ts>(args)...);
    }

    result_set execute_query(void);

    bool execute_non_query(void) {
        std::error_code ec;
        return execute_non_query(ec);
    }

    bool execute_non_query(std::error_code &ec);

    void reset(void);

    void clear_bindings(void);

    void close(void);

private:
    connection &_connection;
    sqlite3_stmt *_stmt;
};

} // namespace tlab::ext::sqlite3

#endif