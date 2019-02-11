/**
 * @file rtsp_client.hpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-28
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef __tlab_ext_live555_rtsp_client_h__
#define __tlab_ext_live555_rtsp_client_h__

#include <tlab/ext/live555/envir_loop.hpp>
#include <tlab/ext/live555/rtsp_handler.hpp>

namespace tlab::ext::live555 {

class rtsp_client {
public:
    rtsp_client(envir_loop &env);

    ~rtsp_client(void);

    bool open(const rtsp_handler_ptr &handler, const std::string &url,
              const std::string &user, const std::string &pw,
              const bool stream_using_tcp = true,
              const std::size_t keep_session_time = 0);

    void close(void);

private:
    class sink : public MediaSink {
    public:
        static sink *createNew(UsageEnvironment &env,
                               MediaSubsession &subsession,
                               char const *streamId , rtsp_client *client) {
            return new sink(env, subsession, streamId, client);
        }

        sink(UsageEnvironment &env, MediaSubsession &subsession,
             char const *streamId, rtsp_client *client)
            : MediaSink(env), _client(client), fSubsession(subsession) {
            fStreamId = strDup(streamId);
            fReceiveBuffer = new u_int8_t[1024768];
        }

        virtual ~sink() {
            delete[] fReceiveBuffer;
            delete[] fStreamId;
        }

        static void afterGettingFrame(void *clientData, unsigned frameSize,
                                      unsigned numTruncatedBytes,
                                      struct timeval presentationTime,
                                      unsigned durationInMicroseconds) {
            sink *s = (sink *)clientData;
            s->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime,
                                 durationInMicroseconds);
        }

// If you don't want to see debugging output for each received frame, then
// comment out the following line:
#define DEBUG_PRINT_EACH_RECEIVED_FRAME 0
        void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
                               struct timeval presentationTime,
                               unsigned /*durationInMicroseconds*/) {
            _client->_handler_ptr->on_frame(
                fSubsession.mediumName(), fSubsession.codecName(),
                presentationTime, fReceiveBuffer, frameSize);
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
            if (strcmp(fSubsession.codecName(), "H264") == 0) {

                if (fStreamId != NULL)
                    envir() << "Stream \"" << fStreamId << "\"; ";
                envir() << fSubsession.mediumName() << "/"
                        << fSubsession.codecName() << ":\tReceived "
                        << frameSize << " bytes";
                if (numTruncatedBytes > 0)
                    envir() << " (with " << numTruncatedBytes
                            << " bytes truncated)";
                char uSecsStr[6 + 1]; // used to output the 'microseconds' part
                                      // of the presentation time
                sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
                envir() << ".\tPresentation time: "
                        << (unsigned)presentationTime.tv_sec << "." << uSecsStr;
                if (fSubsession.rtpSource() != NULL &&
                    !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
                    envir() << "!"; // mark the debugging output to indicate
                                    // that this presentation time is not
                                    // RTCP-synchronized
                }
                if ((fReceiveBuffer[0] & 0x1f) != 1)
                    envir() << "type: " << (fReceiveBuffer[0] & 0x1f) << "\n";
                else
                    envir() << "\n";
            }
#endif
            // Then continue, to request the next frame of data:
            continuePlaying();
        }

        Boolean continuePlaying() {
            if (fSource == NULL)
                return False; // sanity check (should not happen)

            // Request the next frame of data from our input source.
            // "afterGettingFrame()" will get called later, when it arrives:
            fSource->getNextFrame(fReceiveBuffer, 1024768, afterGettingFrame,
                                  this, onSourceClosure, this);
            return True;
        }

    private:
        u_int8_t *fReceiveBuffer;
        MediaSubsession &fSubsession;
        char *fStreamId;
        rtsp_client *_client;
    };

private:
    // RTSP 'response handlers':
    void continueAfterDESCRIBE(int resultCode, char *resultString);
    void continueAfterSETUP(int resultCode, char *resultString);
    void continueAfterPLAY(int resultCode, char *resultString);
    void setupNextSubsession(void);
    void subsessionAfterPlaying(MediaSubsession *subsession);
    void subsessionByeHandler(MediaSubsession *subsession);
    //
    void handle_timer(void);

    void parse_sdp(const char *sdp);

private:
    static void continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode,
                                      char *resultString);
    static void continueAfterSETUP(RTSPClient *rtspClient, int resultCode,
                                   char *resultString);
    static void continueAfterPLAY(RTSPClient *rtspClient, int resultCode,
                                  char *resultString);
    static void subsessionAfterPlaying(void *clientData);
    static void subsessionByeHandler(void *clientData);
    static void handle_timer(void *ctx);

private:
    envir_loop &_envir;
    rtsp_handler_ptr _handler_ptr;
    RTSPClient *_client;
    Authenticator _authenticator;
    MediaSession *_session;
    MediaSubsessionIterator *_iter;
    MediaSubsession *_subsession;
    TaskToken _timer_task;
    bool _stream_using_tcp;
    std::size_t _keep_session_time;
    std::atomic<int> _flag;
    std::string _sps;
    std::string _pps;
};

} // namespace tlab::ext::live555

#endif