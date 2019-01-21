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

logger::logger(void) : _level(tlab::log::level::trace) {}

void logger::log(const tlab::log::level lvl,
                 tlab::log::local_context local_context, const char *message,
                 ...) {
    tlab::log::record r(lvl,nullptr,nullptr,local_context);
    va_list args;
    va_start(args, message);
    gprintf("%s %s %d", local_context.file , local_context.function , local_context.line );
    gprintf(message, args);
    va_end(args);
}

void logger::add_service(std::shared_ptr<service> ptr) {
    _services.push_back(ptr);
}

logger &logger::instance(void) {
    static logger global_logger;
    return global_logger;
}

} // namespace tlab::log