/**
 * @file ostringstream_buffer.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/log/ostringstream_buffer.hpp>

namespace tlab::log {

ostringstream_buffer::ostringstream_buffer(void) {}

ostringstream_buffer::~ostringstream_buffer(void) {}

const char *ostringstream_buffer::ptr(void) const {
    _str = _ss.str();
    return _str.c_str();
}

std::size_t ostringstream_buffer::size(void) const { return _str.length(); }

void ostringstream_buffer::clear(void) {
    _ss.clear();
    _ss.str("");
}

} // namespace tlab::log