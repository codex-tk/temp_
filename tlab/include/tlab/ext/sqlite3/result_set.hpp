/**
 * @file result_set.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-18
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_ext_sqlite3_result_set_h__
#define __tlab_ext_sqlite3_result_set_h__

#include <tlab/ext/sqlite3/connection.hpp>
#include <tlab/ext/sqlite3/error_category.hpp>

#include <tlab/mp.hpp>

namespace tlab::ext::sqlite3 {

class result_set {
public:
    result_set(sqlite3_stmt *stmt) noexcept;
    result_set(const result_set &rhs) noexcept;
    result_set(result_set &&rhs) noexcept;

    bool next(void);

    std::string get_column(std::size_t col, tlab::mp::type2type<std::string>) {
        return std::string(
            reinterpret_cast<const char *>(sqlite3_column_text(_stmt, static_cast<int>(col))));
    }

    int get_column(std::size_t col, tlab::mp::type2type<int>) {
        return sqlite3_column_int(_stmt, static_cast<int>(col));
    }

    template <typename T> T get_column(std::size_t S) {
        return get_column(S, tlab::mp::type2type<T>{});
    }

    template <typename... Ts, std::size_t... S>
    std::tuple<Ts...> get_row_expantion(tlab::mp::index_sequence<S...>) {
        return std::make_tuple<Ts...>(
            get_column<
                typename tlab::mp::at<S, tlab::mp::type_list<Ts...>>::type>(
                S)...);
    }

    template <typename... Ts> std::tuple<Ts...> get_row(void) {
        return get_row_expantion<Ts...>(
            tlab::mp::make_index_sequence<sizeof...(Ts)>{});
    }

private:
    sqlite3_stmt *_stmt;
};

} // namespace tlab::ext::sqlite3

#endif