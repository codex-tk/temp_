/**
 * @file simple_h264_framed_source.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-02-10
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __tlab_ext_live555_simple_h264_framed_source_h__
#define __tlab_ext_live555_simple_h264_framed_source_h__

#include <list>
#include <tlab/basic_buffer.hpp>
#include <tlab/internal.hpp>

#include "FramedSource.hh"

namespace tlab::ext::live555 {

class simple_h264_framed_source : public FramedSource {
public:
    struct frame {
        tlab::byte_buffer data;
        timeval presenttime;
    };

    static FramedSource *createNew(UsageEnvironment &env) {
        return new simple_h264_framed_source(env);
    }

    static void deliverFrame0(void *clientData) {
        ((simple_h264_framed_source *)clientData)->deliverFrame();
    }

    simple_h264_framed_source(UsageEnvironment &env) : FramedSource(env) {
        _event_trigger_id =
            envir().taskScheduler().createEventTrigger(deliverFrame0);
    }

    virtual ~simple_h264_framed_source(void) {
        envir().taskScheduler().deleteEventTrigger(_event_trigger_id);
        _event_trigger_id = 0;
    }

    void append(const frame &f) {
        _buffers.push_back(f);
        envir().taskScheduler().triggerEvent(_event_trigger_id, this);
    }

    void deliverFrame() {
        if (!isCurrentlyAwaitingData())
            return;
        fDurationInMicroseconds = 0;
        if (!_buffers.empty) {
            frame new_frame = _buffers.front();
            _buffers.pop_front();

            char *newFrameDataStart = new_frame.data.gptr();
            unsigned newFrameSize = new_frame.data.gsize();

            // Deliver the data here:
            if (newFrameSize > fMaxSize) {
                fFrameSize = fMaxSize;
                fNumTruncatedBytes = newFrameSize - fMaxSize;
            } else {
                fFrameSize = newFrameSize;
            }
            fPresentationTime = new_frame.presenttime;
            memmove(fTo, newFrameDataStart, fFrameSize);
            FramedSource::afterGetting(this);
        }
    }

public:
    virtual void doGetNextFrame() override { deliverFrame(); }

private:
    std::list<frame> _buffers;
    EventTriggerId _event_trigger_id;
};

} // namespace tlab::ext::live555
#endif