/**
 * @file statement.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-17
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_ext_sqlite3_statment_h__
#define __tlab_ext_sqlite3_statment_h__

#include <tlab/ext/sqlite3/connection.hpp>
#include <tlab/ext/sqlite3/error_category.hpp>

#include <tlab/mp.hpp>

namespace tlab::ext::sqlite3 {
namespace internal {

template <typename T> T convert(const char *);

template <> inline int convert<int>(const char *val) { return std::stoi(val); }
template <> inline double convert<double>(const char *val) {
    return std::stod(val);
}
template <> inline std::string convert<std::string>(const char *val) {
    return std::string(val);
}

template <std::size_t I, typename... Ts>
void set(std::tuple<Ts...> &tuple, char **arg_val) {
    std::get<I>(tuple) =
        convert<typename tlab::mp::at<I, tlab::mp::type_list<Ts...>>::type>(
            arg_val[I]);
}

template <std::size_t... S, typename... Ts>
void expantion0(tlab::mp::index_sequence<S...>, std::tuple<Ts...> &tuple,
                char **arg_val) {
    ((set<S, Ts...>(tuple, arg_val)), ...);
}

template <std::size_t I, typename... Ts>
auto get(char **arg_val) ->
    typename tlab::mp::at<I, tlab::mp::type_list<Ts...>>::type {
    return convert<typename tlab::mp::at<I, tlab::mp::type_list<Ts...>>::type>(
        arg_val[I]);
}

template <std::size_t... S, typename... Ts>
void expantion(tlab::mp::index_sequence<S...>,
               std::vector<std::tuple<Ts...>> *records_ptr, char **arg_val) {
    if (records_ptr)
        records_ptr->emplace_back(get<S, Ts...>(arg_val)...);
}
} // namespace internal

class statement {
public:
    statement(connection &conn);
    ~statement(void);

    bool execute(const std::string &sql);
    bool execute(const std::string &sql, std::error_code &ec);

    template <typename... Ts>
    bool execute_query_simple(const std::string &sql,
                              std::vector<std::tuple<Ts...>> &records,
                              std::error_code &ec) {
        struct context {
            static int callback(void *data, int /*argc*/, char **arg_val,
                                char ** /*arg_col*/) {
                internal::expantion(
                    tlab::mp::make_index_sequence<sizeof...(Ts)>{},
                    static_cast<std::vector<std::tuple<Ts...>> *>(data),
                    arg_val);
                return 0;
                /*
                std::vector<std::tuple<Ts...>> *prs =
                    static_cast<std::vector<std::tuple<Ts...>> *>(data);
                std::tuple<Ts...> tuple;
                internal::expantion0(
                    tlab::mp::make_index_sequence<sizeof...(Ts)>{}, tuple,
                    arg_val);
                prs->emplace_back(tuple);
                return 0;*/
            }
        };
        int r = sqlite3_exec(_connection.handle(), sql.c_str(),
                             &context::callback, &records, nullptr);
        if (r != 0) {
            ec = std::error_code(
                r, tlab::ext::sqlite3::error_category::instance());
            return false;
        }
        return true;
    }

private:
    connection &_connection;
};

} // namespace tlab::ext::sqlite3

#endif