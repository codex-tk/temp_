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
#include <tlab/log/logging_service.hpp>

void gprintf(const char *fmt, ...);

namespace tlab::log {

logger::logger(void) : _level(tlab::log::level::trace) {}

void logger::log(const tlab::log::level lvl,
                 tlab::log::local_context local_context, const char *tag,
                 const char *message, ...) {
    char buff[4096] = {0,};
    va_list args;
    va_start(args, message);
    vsnprintf(buff,4096,message,args);
    va_end(args);
    
    tlab::log::record r(lvl, tag, buff, local_context);
    for(auto it : _services){
        it->handle_record(r);
    }
}

void logger::add_service(std::shared_ptr<logging_service> ptr) {
    _services.push_back(ptr);
}

logger &logger::instance(void) {
    static logger global_logger;
    return global_logger;
}

} // namespace tlab::log