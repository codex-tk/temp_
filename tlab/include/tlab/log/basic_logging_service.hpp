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

namespace tlab::log {

template <typename Format, typename Ostream>
class basic_logging_service : public logging_service {
public:
    basic_logging_service(void) {}

    virtual ~basic_logging_service(void) noexcept {}

    virtual void handle_record(const log::record &r) override {
        typename Ostream::record_stream_type rstrm;
        using ostream_format = typename Ostream::rebind<Format>::type;
        ostream_format::append_to(rstrm,r);
        _ostream << rstrm;
    }
private:
    Ostream _ostream;
};

} // namespace tlab::log

#endif