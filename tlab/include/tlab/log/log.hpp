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

struct local_context {
    const char *file;
    const char *function;
    int line;
    local_context(const char *afile, const char *afunction, int aline)
        : file(afile), function(afunction), line(aline) {}
};

struct record {
    log::level lvl;
    const char *tag;
    const char *message;
    log::local_context &local_context;
    record(level alvl, const char *atag, const char *amessage,
           log::local_context &alocal_context)
        : lvl(alvl), tag(atag), message(amessage),
          local_context(alocal_context) {}
};
} // namespace tlab::log

#ifndef TLOG
#if defined(_WIN32) || defined(__WIN32__)
#define TLOG(logger, lvl, message, ...)                                        \
    do {                                                                       \
        if (logger.accept(lvl))                                                \
            logger.log(                                                        \
                lvl,                                                           \
                tlab::log::local_context(__FILE__, __FUNCTION__, __LINE__),    \
                message, __VA_ARGS__);                                         \
    } while (0)
#else
#define TLOG(logger, lvl, message, ...)                                        \
    do {                                                                       \
        if (logger.accept(lvl))                                                \
            logger.log(                                                        \
                lvl,                                                           \
                tlab::log::local_context(__FILE__, __FUNCTION__, __LINE__),    \
                message, ##__VA_ARGS__);                                       \
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