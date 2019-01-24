/**
 * @file console_output.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_log_console_output_h__
#define __tlab_log_console_output_h__

#include <tlab/log/output.hpp>

namespace tlab::log {

class console_output : public tlab::log::output {
public:
    console_output(void);

    virtual void put(const char *ptr, const std::size_t sz,
                     const tlab::log::record &r) override;
};

} // namespace tlab::log

#endif