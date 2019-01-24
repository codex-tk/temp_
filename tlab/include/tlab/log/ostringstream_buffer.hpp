/**
 * @file ostringstream_buffer.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_log_ostringstream_buffer_h__
#define __tlab_log_ostringstream_buffer_h__

#include <tlab/internal.hpp>

namespace tlab::log {

class ostringstream_buffer {
public:
    ostringstream_buffer(void);

    ~ostringstream_buffer(void);

    const char *ptr(void) const;

    std::size_t size(void) const;

    void clear(void);

    template <typename T> ostringstream_buffer &operator<<(T &&t) {
        _ss << t;
        return *this;
    }

private:
    std::ostringstream _ss;
    mutable std::string _str;
};

} // namespace tlab::log

#endif