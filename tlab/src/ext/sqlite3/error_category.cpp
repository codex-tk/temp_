/**
 * @file error_category.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-17
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <tlab/ext/sqlite3/error_category.hpp>

#include <sqlite3/sqlite3.h>

namespace tlab::ext::sqlite3 {

error_category::error_category(void) noexcept {}

const char *error_category::name() const noexcept {
    return "sqlite3_error_category";
}

::std::string error_category::message(int code) const { 
    return std::string(sqlite3_errstr(code));
}

error_category &error_category::instance(void) {
    static error_category category;
    return category;
}

} // namespace tlab::ext::sqlite3
