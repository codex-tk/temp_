/**
 * @file rtsp_to_ts_handler.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2019-02-11
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_ext_live555_rtsp_to_ts_handler_h__
#define __tlab_ext_live555_rtsp_to_ts_handler_h__

#include <tlab/ext/live555/envir_loop.hpp>
#include <tlab/ext/live555/memory_buffer_sink.hpp>
#include <tlab/ext/live555/rtsp_handler.hpp>
#include <tlab/ext/live555/simple_h264_framed_source.hpp>
#include <tlab/internal.hpp>

namespace tlab::ext::live555 {

class rtsp_to_ts_handler : public tlab::ext::live555::rtsp_handler {
public:
    rtsp_to_ts_handler(tlab::ext::live555::envir_loop &envir)
        : _envir(envir) {
        envir.run([&] {
            _source =
                tlab::ext::live555::simple_h264_framed_source::createNew(envir);
            _sink = tlab::ext::live555::memory_buffer_sink::createNew(envir);
            _mux = MPEG2TransportStreamFromESSource::createNew(envir.envir());
            _mux->addNewVideoSource(_source, 5);
            _sink->startPlaying(*_mux, nullptr, nullptr);
        });
    }

    virtual ~rtsp_to_ts_handler(void) {
        _envir.run([&] {
            if (_sink) {
                _sink->stopPlaying();
                Medium::close(_sink);
            }
            if (_mux)
                Medium::close(_mux);
            if (_source)
                Medium::close(_source);
        });
    }

    virtual void on_frame(const char *midium, const char *codec,
                          struct timeval presentation_time, void *ptr,
                          const std::size_t size) override {
        if (strcmp(midium, "video") == 0) {
            tlab::ext::live555::simple_h264_framed_source::frame frame;
            char marker[] = {0, 0, 0, 1};
            frame.data.reserve(size + 4);
            frame.data.write(marker, 4);
            frame.data.write(ptr, size);
            frame.presenttime = presentation_time;
            _source->append(frame);
        }
    }

private:
    tlab::ext::live555::envir_loop& _envir;
    tlab::ext::live555::simple_h264_framed_source *_source;
    tlab::ext::live555::memory_buffer_sink *_sink;
    MPEG2TransportStreamFromESSource *_mux;
};

} // namespace tlab::ext::live555

#endif