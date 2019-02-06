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

namespace tlab::ext::live555 {

class rtsp_client {
public:
    rtsp_client(envir_loop &env);

    ~rtsp_client(void);

    bool open(const std::string &url, const std::string &user,
              const std::string &pw, const bool stream_using_tcp = true,
              const std::size_t keep_session_time = 0);

    void close(void);

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
    RTSPClient* _client;
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