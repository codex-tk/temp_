/**
 * @file logger.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-20
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/log/logger.hpp>

void gprintf(const char *fmt, ...);

namespace tlab::log {

logger::logger(void)
    : _filter(static_cast<int>(tlab::log::level::trace) |
              static_cast<int>(tlab::log::level::trace) |
              static_cast<int>(tlab::log::level::info) |
              static_cast<int>(tlab::log::level::warn) |
              static_cast<int>(tlab::log::level::error)) {}

void logger::log(const tlab::log::level lvl, const char *message, ...) {
    tlab::ignore = lvl;
    va_list args;
    va_start(args, message);
    gprintf(message, args);
    va_end(args);
}

void logger::add_service(std::shared_ptr<service> ptr){
    _services.push_back(ptr);
}

logger &logger::instance(void) {
    static logger global_logger;
    return global_logger;
}

} // namespace tlab::log