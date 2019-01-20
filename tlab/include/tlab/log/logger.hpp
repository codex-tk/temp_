/**
 * @file logger.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-20
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_log_logger_h__
#define __tlab_log_logger_h__

#include <tlab/log/log.hpp>

namespace tlab::log {

class service;

class logger {
public:
    logger(void);

    inline bool accept(const tlab::log::level lvl) {
        return static_cast<int>(lvl) & _filter;
    }

    void log(const tlab::log::level lvl, const char *message, ...);

    void add_service(std::shared_ptr<service> ptr);

public:
    static logger &instance(void);

private:
    int _filter;
    std::vector<std::shared_ptr<service>> _services;
};

} // namespace tlab::log

#endif