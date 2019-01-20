/**
 * @file log.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-20
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_log_h__
#define __tlab_log_h__

#include <tlab/internal.hpp>

namespace tlab::log {

enum class level {
    trace = 0x01,
    debug = 0x02,
    info = 0x04,
    warn = 0x08,
    error = 0x10
};
}

#ifndef TLOG
#if defined(_WIN32) || defined(__WIN32__)
#define TLOG(logger, lvl, message, ...)                                        \
    do {                                                                       \
        if (logger.accept(lvl))                                                \
            logger.log(lvl, message, __VA_ARGS__);                             \
    } while (0)
#else
#define TLOG(logger, lvl, message, ...)                                        \
    do {                                                                       \
        if (logger.accept(lvl))                                                \
            logger.log(lvl, message, ##__VA_ARGS__);                           \
    } while (0)
#endif
#endif

using loglvl = tlab::log::level;

#ifndef TLAB_LOG
#if defined(_WIN32) || defined(__WIN32__)
#define TLOG_T(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::trace, message, __VA_ARGS__)
#define TLOG_D(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::debug, message, __VA_ARGS__)
#define TLOG_I(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::info, message, __VA_ARGS__)
#define TLOG_W(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::warn, message, __VA_ARGS__)
#define TLOG_E(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::error, message, __VA_ARGS__)
#else
#define TLOG_T(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::trace, message, ##__VA_ARGS__)
#define TLOG_D(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::debug, message, ##__VA_ARGS__)
#define TLOG_I(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::info, message, ##__VA_ARGS__)
#define TLOG_W(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::warn, message, ##__VA_ARGS__)
#define TLOG_E(message, ...)                                                   \
    TLOG(tlab::log::logger::instance(), loglvl::error, message, ##__VA_ARGS__)
#endif
#endif

#endif