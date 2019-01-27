/**
 * @file envir_loop.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-27
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <tlab/ext/live555/envir_loop.hpp>

namespace tlab::ext::live555 {

static void handle_task(void *ptr) {
    envir_loop *pthis = static_cast<envir_loop *>(ptr);
    pthis->handle_task();
}

envir_loop::envir_loop(void) {
    _thread_flag = 0;
    _scheduler_ptr = BasicTaskScheduler::createNew();
    _env_ptr = BasicUsageEnvironment::createNew(*_scheduler_ptr);
    _loop_thread = std::thread([this] {
        _task = _env_ptr->taskScheduler().scheduleDelayedTask(
            100000, live555::handle_task, this);
        _env_ptr->taskScheduler().doEventLoop(&_thread_flag);
    });
}

envir_loop::~envir_loop(void) {
    _thread_flag = 1;
    _loop_thread.join();
}

void envir_loop::handle_task(void) {
    std::vector<std::function<void()>> functions(std::move(_functions));
    for (auto it : functions) {
        (it)();
    }
    _task = _env_ptr->taskScheduler().scheduleDelayedTask(
        100000, live555::handle_task, this);
}
} // namespace tlab::ext::live555