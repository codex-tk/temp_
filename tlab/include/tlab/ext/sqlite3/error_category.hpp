/**
 * @file error_category.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-17
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __tlab_ext_sqlite3_error_category_h__
#define __tlab_ext_sqlite3_error_category_h__

#include <system_error>

namespace tlab::ext::sqlite3 {

class error_category : public ::std::error_category {
public:
    error_category(void) noexcept;

    virtual const char *name() const noexcept override;

    virtual ::std::string message(int code) const override;

    static error_category &instance(void);
};

} // namespace tlab::ext::sqlite3

#endif