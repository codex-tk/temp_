/**
 * @file rtsp_handler.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2019-02-11
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_ext_live555_rtsp_handler_h__
#define __tlab_ext_live555_rtsp_handler_h__

#include <tlab/internal.hpp>

namespace tlab::ext::live555 {

class rtsp_handler {
public:
    virtual void on_frame(const char *midium, const char *codec,
                          struct timeval presentation_time, void *ptr,
                          const std::size_t size) = 0;
};

using rtsp_handler_ptr = std::shared_ptr<rtsp_handler>;

} // namespace tlab::ext::live555

#endif