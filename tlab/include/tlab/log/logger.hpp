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

class logging_service;

class logger {
public:
    logger(void);

    inline bool accept(const tlab::log::level lvl) { return lvl >= _level; }

    void log(const tlab::log::level lvl, tlab::log::local_context local_context,
             const char *tag, const char *message, ...);

    template <typename... Ts>
    void logT(const tlab::log::level lvl,
              tlab::log::local_context local_context, const char *tag,
              const char *message, Ts &&... args) {

    }

    void add_service(std::shared_ptr<logging_service> ptr);
public:
    static logger &instance(void);

private:
    tlab::log::level _level;
    std::vector<std::shared_ptr<logging_service>> _services;
};

} // namespace tlab::log

#endif