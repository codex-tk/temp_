/**
 * @file output.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __tlab_log_output_h__
#define __tlab_log_output_h__

#include <tlab/log/log.hpp>

namespace tlab::log {

class output {
public:
    virtual void put(const char *ptr, const std::size_t sz,
                     const tlab::log::record &r) = 0;

    template <typename BufferT>
    void put_buffer(const BufferT &buf, const record &r) {
        put(buf.ptr(), buf.size(), r);
    }
};

} // namespace tlab::log

#endif