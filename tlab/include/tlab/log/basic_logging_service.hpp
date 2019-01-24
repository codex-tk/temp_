/**
 * @file basic_logging_service.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2019-01-22
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __tlab_log_basic_logging_service_h__
#define __tlab_log_basic_logging_service_h__

#include <tlab/log/logging_service.hpp>
#include <tlab/log/ostringstream_buffer.hpp>

namespace tlab::log {

template <typename FormatT, typename BufferT = ostringstream_buffer>
class basic_logging_service : public logging_service {
public:
    basic_logging_service(void) {}

    virtual ~basic_logging_service(void) noexcept {}

    virtual void handle_record(const log::record &r) override {
        _buffer.clear();
        FormatT::write(_buffer, r);
        for (auto it : _outputs) {
            it->put_buffer(_buffer, r);
        }
    }

private:
    BufferT _buffer;
};

} // namespace tlab::log

#endif