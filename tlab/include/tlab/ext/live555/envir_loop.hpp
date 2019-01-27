/**
 * @file envir_loop.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-27
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/internal.hpp>

#include "BasicUsageEnvironment.hh"
#include "liveMedia.hh"

#ifndef __tlab_ext_live555_envir_loop_h__
#define __tlab_ext_live555_envir_loop_h__

namespace tlab::ext::live555 {

class envir_loop {
public:
    envir_loop(void);
    ~envir_loop(void);

    void handle_task(void);

    template <typename HandlerT> void run(HandlerT &&handler) {
        std::scoped_lock<std::mutex> guard(_lock);
        _functions.emplace_back(handler);
    }
private:
    TaskScheduler *_scheduler_ptr;
    UsageEnvironment *_env_ptr;
    std::thread _loop_thread;
    volatile char _thread_flag;
    TaskToken _task;

    std::mutex _lock;
    std::vector<std::function<void()>> _functions;
};

} // namespace tlab::ext::live555

#endif