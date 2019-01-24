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
#include <tlab/log/output.hpp>

namespace tlab::log {

class logging_service {
public:
    virtual void handle_record(const log::record &r) = 0;

    void add_output(const std::shared_ptr<tlab::log::output>& optr);
protected:
    std::vector<std::shared_ptr<tlab::log::output>> _outputs;
};

} // namespace tlab::log

#endif