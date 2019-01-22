/**
 * @file logging_service.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-20
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __tlab_log_logging_service_h__
#define __tlab_log_logging_service_h__

#include <tlab/log/log.hpp>

namespace tlab::log {

class logging_service {
public:
    virtual ~logging_service(void) noexcept {}

    virtual void handle_record(const log::record& r) = 0;
};

} // namespace tlab::log

#endif