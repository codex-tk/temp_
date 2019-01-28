/**
 * @file rtsp_client.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-28
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/ext/live555/rtsp_client.hpp>
#include <tlab/util.hpp>

namespace tlab::ext::live555 {

void rtsp_client::continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode,
                                        char *resultString) {
    rtsp_client *pthis = static_cast<impl*>(rtspClient)->client;
    pthis->continueAfterDESCRIBE(resultCode, resultString);
}

void rtsp_client::continueAfterSETUP(RTSPClient *rtspClient, int resultCode,
                                     char *resultString) {
    rtsp_client *pthis = static_cast<impl*>(rtspClient)->client;
    pthis->continueAfterSETUP(resultCode, resultString);
}

void rtsp_client::continueAfterPLAY(RTSPClient *rtspClient, int resultCode,
                                    char *resultString) {
    rtsp_client *pthis = static_cast<impl*>(rtspClient)->client;
    pthis->continueAfterPLAY(resultCode, resultString);
}

void rtsp_client::subsessionAfterPlaying(void *clientData) {
    MediaSubsession *subsession = (MediaSubsession *)clientData;
    rtsp_client *pthis = static_cast<rtsp_client *>(subsession->miscPtr);
    pthis->subsessionAfterPlaying(subsession);
}

void rtsp_client::subsessionByeHandler(void *clientData) {
    MediaSubsession *subsession = (MediaSubsession *)clientData;
    rtsp_client *pthis = static_cast<rtsp_client *>(subsession->miscPtr);
    pthis->subsessionByeHandler(subsession);
}

void rtsp_client::handle_timer(void *ctx) {
    rtsp_client *c = static_cast<rtsp_client *>(ctx);
    c->handle_timer();
}

bool rtsp_client::open(const std::string &url, const std::string &user,
                       const std::string &pw, const bool stream_using_tcp,
                       const std::size_t keep_session_time) {
    
    return false;
}

void rtsp_client::close(void){}
void rtsp_client::continueAfterDESCRIBE(int resultCode, char *resultString){}
void rtsp_client::continueAfterSETUP(int resultCode, char *resultString){}
void rtsp_client::continueAfterPLAY(int resultCode, char *resultString){}
void rtsp_client::setupNextSubsession(void){}
void rtsp_client::subsessionAfterPlaying(MediaSubsession *subsession){}
void rtsp_client::subsessionByeHandler(MediaSubsession *subsession){}
void rtsp_client::handle_timer(void){}

} // namespace tlab::ext::live555