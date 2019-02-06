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

#include <map>
#include <tlab/ext/live555/rtsp_client.hpp>
#include <tlab/util.hpp>

namespace tlab::ext::live555 {

namespace {
static const char b64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char b64_table_r[128] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64,
    64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64};

} // namespace

std::string base64_encode(void *ptr, std::size_t size) {
    using ::std::numeric_limits;

    if (size > (std::numeric_limits<std::string::size_type>::max() / 4u) * 3u) {
        return "";
    }

    std::string retval((((size + 2) / 3) * 4), '=');
    std::size_t outpos = 0;
    int bits_collected = 0;
    unsigned int accumulator = 0;
    unsigned char *buffer = static_cast<unsigned char *>(ptr);
    for (uint32_t i = 0; i < size; ++i) {
        accumulator = (accumulator << 8) | (buffer[i] & 0xff);
        bits_collected += 8;
        while (bits_collected >= 6) {
            bits_collected -= 6;
            retval[outpos++] =
                b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }
    if (bits_collected > 0) {
        accumulator <<= 6 - bits_collected;
        retval[outpos++] = b64_table[accumulator & 0x3fu];
    }
    return retval;
}

int base64_decode(const std::string &src, void *ptr, std::size_t size) {
    int index = 0;
    unsigned char *buffer = static_cast<unsigned char *>(ptr);
    const std::string::const_iterator last = src.end();
    int bits_collected = 0;
    unsigned int accumulator = 0;

    for (std::string::const_iterator i = src.begin(); i != last; ++i) {
        const int c = *i;
        if (isspace(c) || c == '=') {
            continue;
        }
        if ((c > 127) || (c < 0) || (b64_table_r[c] > 63)) {
            //
        }
        accumulator = (accumulator << 6) | b64_table_r[c];
        bits_collected += 6;
        if (bits_collected >= 8) {
            bits_collected -= 8;
            buffer[index++] = (char)((accumulator >> bits_collected) & 0xff);
        }
    }
    return index;
}

template <typename string_type>
static std::vector<string_type> split(const string_type &message,
                                      const string_type &sep) {
    typename string_type::size_type msgEnd = message.length();
    typename string_type::size_type start = 0;
    typename string_type::size_type stop = 0;

    std::vector<string_type> result;
    start = message.find_first_not_of(sep);
    while (true) {
        if (start >= msgEnd)
            break;

        stop = message.find_first_of(sep, start);
        if (stop == string_type::npos)
            stop = msgEnd;

        result.push_back(message.substr(start, stop - start));
        start = message.find_first_not_of(sep, stop + 1);
    }
    return result;
}

template <typename string_type>
static string_type to_upper(const string_type &msg) {
    string_type out;
    out.reserve(msg.length());
    for (typename string_type::size_type idx = 0; idx < msg.length(); ++idx) {

        if (msg[idx] < 0xff && isalpha(msg[idx]) && islower(msg[idx])) {
            out.push_back(_toupper(msg[idx]));
        } else {
            out.push_back(msg[idx]);
        }
    }
    return out;
}

class sink : public MediaSink {
public:
    static sink *createNew(UsageEnvironment &env, MediaSubsession &subsession,
                    char const *streamId) {
        return new sink(env, subsession, streamId);
    }

    sink(UsageEnvironment &env, MediaSubsession &subsession,
         char const *streamId)
        : MediaSink(env), fSubsession(subsession) {
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
        // We've just received a frame of data.  (Optionally) print out
        // information about it:
        /*
        _client->on_sink_data(fSubsession.mediumName(), fSubsession.codecName(),
                              fReceiveBuffer, frameSize,
                              (presentationTime.tv_sec * 1000000) +
                                  presentationTime.tv_usec);

        */
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
        if (fStreamId != NULL)
            envir() << "Stream \"" << fStreamId << "\"; ";
        envir() << fSubsession.mediumName() << "/" << fSubsession.codecName()
                << ":\tReceived " << frameSize << " bytes";
        if (numTruncatedBytes > 0)
            envir() << " (with " << numTruncatedBytes << " bytes truncated)";
        char uSecsStr[6 + 1]; // used to output the 'microseconds' part of the
                              // presentation time
        sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
        envir() << ".\tPresentation time: " << (unsigned)presentationTime.tv_sec
                << "." << uSecsStr;
        if (fSubsession.rtpSource() != NULL &&
            !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
            envir() << "!"; // mark the debugging output to indicate that this
                            // presentation time is not RTCP-synchronized
        }
        envir() << "\n";
#endif
        // Then continue, to request the next frame of data:
        continuePlaying();
    }

    Boolean continuePlaying() {
        if (fSource == NULL)
            return False; // sanity check (should not happen)

        // Request the next frame of data from our input source.
        // "afterGettingFrame()" will get called later, when it arrives:
        fSource->getNextFrame(fReceiveBuffer, 1024768, afterGettingFrame, this,
                              onSourceClosure, this);
        return True;
    }

private:
    u_int8_t *fReceiveBuffer;
    MediaSubsession &fSubsession;
    char *fStreamId;
};

std::map<RTSPClient *, rtsp_client *> _clients;

void rtsp_client::continueAfterDESCRIBE(RTSPClient *rtspClient, int resultCode,
                                        char *resultString) {
    rtsp_client *pthis = _clients[rtspClient];
    pthis->continueAfterDESCRIBE(resultCode, resultString);
}

void rtsp_client::continueAfterSETUP(RTSPClient *rtspClient, int resultCode,
                                     char *resultString) {
    rtsp_client *pthis = _clients[rtspClient];
    pthis->continueAfterSETUP(resultCode, resultString);
}

void rtsp_client::continueAfterPLAY(RTSPClient *rtspClient, int resultCode,
                                    char *resultString) {
    rtsp_client *pthis = _clients[rtspClient];
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

rtsp_client::rtsp_client(envir_loop &env)
    : _envir(env), _session(nullptr), _iter(nullptr), _subsession(nullptr),
      _timer_task(nullptr), _stream_using_tcp(false), _flag(0),
      _keep_session_time(0), _sps(""), _pps("") {}

rtsp_client::~rtsp_client(void) {}

bool rtsp_client::open(const std::string &url, const std::string &user,
                       const std::string &pw, const bool stream_using_tcp,
                       const std::size_t keep_session_time) {
    _authenticator.setUsernameAndPassword(user.c_str(), pw.c_str());
    _stream_using_tcp = stream_using_tcp;
    _keep_session_time = keep_session_time;
    _client = RTSPClient::createNew(_envir.envir(), url.c_str());
    if (_flag.exchange(1) == 0) {
        _envir.run([&] {
            _clients[_client] = this;
            _client->sendDescribeCommand(&rtsp_client::continueAfterDESCRIBE,
                                         &_authenticator);
        });
        return true;
    }
    return false;
}

void rtsp_client::close(void) {
    if (_flag.exchange(0) == 1) {
        _envir.run([this] {
            if (_timer_task != nullptr) {
                _envir.envir().taskScheduler().unscheduleDelayedTask(
                    _timer_task);
                _timer_task = nullptr;
            }
            // First, check whether any subsessions have still to be closed:
            if (_session != nullptr) {
                Boolean someSubsessionsWereActive = False;
                MediaSubsessionIterator iter(*_session);
                MediaSubsession *subsession;
                while ((subsession = iter.next()) != nullptr) {
                    if (subsession->sink != nullptr) {
                        Medium::close(subsession->sink);
                        subsession->sink = nullptr;

                        if (subsession->rtcpInstance() != nullptr) {
                            subsession->rtcpInstance()->setByeHandler(nullptr,
                                                                      nullptr);
                        }
                        someSubsessionsWereActive = True;
                    }
                }

                if (someSubsessionsWereActive) {
                    _client->sendTeardownCommand(*_session, nullptr,
                                                 &_authenticator);
                }
            }
            if (_iter) {
                delete _iter;
            }
            _iter = nullptr;
            if (_session != nullptr) {
                Medium::close(_session);
            }
            _session = nullptr;
            _clients.erase(_client);
            Medium::close(_client);
            delete this;
        });
    }
}

void rtsp_client::setupNextSubsession(void) {
    _subsession = _iter->next();
    if (_subsession != NULL) {
        if (!_subsession->initiate()) {
            // env << *rtspClient << "Failed to initiate the \"" <<
            // *scs.subsession << "\" subsession: " << env.getResultMsg() <<
            // "\n";
            setupNextSubsession(); // give up on this subsession; go to the next
                                   // one
        } else {
            // env << *rtspClient << "Initiated the \"" << *scs.subsession
            //	<< "\" subsession (client ports " <<
            // scs.subsession->clientPortNum() << "-" <<
            // scs.subsession->clientPortNum()+1 << ")\n";
            // Continue setting up this subsession, by sending a RTSP "SETUP"
            // command:
            _client->sendSetupCommand(
                *_subsession, rtsp_client::continueAfterSETUP, false,
                _stream_using_tcp, false, &_authenticator);
        }
        return;
    }
    // We've finished setting up all of the subsessions.  Now, send a RTSP
    // "PLAY" command to start the streaming:
    if (_session->absStartTime() != NULL) {
        // Special case: The stream is indexed by 'absolute' time, so send an
        // appropriate "PLAY" command:
        _client->sendPlayCommand(*_session, rtsp_client::continueAfterPLAY,
                                 _session->absStartTime(),
                                 _session->absEndTime(), 1.0f, &_authenticator);
    } else {
        //_duration = _session->playEndTime() - _session->playStartTime();
        _client->sendPlayCommand(*_session, rtsp_client::continueAfterPLAY, 0,
                                 -1.0, 1.0f, &_authenticator);
    }
}

void rtsp_client::continueAfterDESCRIBE(int resultCode, char *resultString) {
    if (resultCode != 0) {
        return;
    }
    char *const sdpDescription = resultString;

    _session = MediaSession::createNew(_envir.envir(), sdpDescription);
    if (_session != nullptr) {
        parse_sdp(sdpDescription);
        _iter = new MediaSubsessionIterator(*_session);
        setupNextSubsession();
    }
    delete[] sdpDescription;
}

void rtsp_client::continueAfterSETUP(int resultCode, char *resultString) {
    if (resultCode != 0) {
        return;
    }
    _subsession->sink =
        sink::createNew(_envir.envir(), *_subsession, _client->url());

    if (_subsession->sink == NULL) {
        return;
    }

    _subsession->miscPtr = this; // a hack to let subsession handle functions
                                 // get the "RTSPClient" from the subsession
    _subsession->sink->startPlaying(*(_subsession->readSource()),
                                    rtsp_client::subsessionAfterPlaying,
                                    _subsession);
    // Also set a handler to be called if a RTCP "BYE" arrives for this
    // subsession:
    if (_subsession->rtcpInstance() != NULL) {
        _subsession->rtcpInstance()->setByeHandler(subsessionByeHandler,
                                                   _subsession);
    }
    // Set up the next subsession, if any:
    setupNextSubsession();
}

void rtsp_client::continueAfterPLAY(int resultCode, char *resultString) {
    if (resultCode != 0) {
        return;
    }
    handle_timer();
}

void rtsp_client::handle_timer(void) {
    if (_keep_session_time != 0) {
        _client->sendOptionsCommand(nullptr, &_authenticator);
        _timer_task = _envir.envir().taskScheduler().scheduleDelayedTask(
            _keep_session_time * 1000 * 1000, rtsp_client::handle_timer, this);
    }
}

void rtsp_client::subsessionAfterPlaying(MediaSubsession *subsession) {
    //(*this)(rtsp_events::RTP_SESSION_END, (void
    //*)subsession->savedSDPLines());

    // Begin by closing this subsession's stream:
    Medium::close(subsession->sink);
    subsession->sink = NULL;

    // Next, check whether *all* subsessions' streams have now been closed:
    MediaSession &session = subsession->parentSession();
    MediaSubsessionIterator iter(session);
    while ((subsession = iter.next()) != NULL) {
        if (subsession->sink != NULL)
            return; // this subsession is still active
    }
    //
}

void rtsp_client::subsessionByeHandler(MediaSubsession *subsession) {
    //(*this)(rtsp_events::RTP_SESSION_BYE, (void
    //*)subsession->savedSDPLines());
    subsessionAfterPlaying(subsession);
}

void rtsp_client::parse_sdp(const char *sdp) {
    if (std::string(sdp).find("H264") == std::string::npos)
        return;

    std::vector<std::string> lines =
        split(std::string(sdp), std::string("\r\n"));
    std::string fmtp = "";
    for (const std::string &line : lines) {
        if (line.find("a=fmtp") != std::string::npos) {
            fmtp = line.substr(2, line.length() - 2);
        }
    }

    if (fmtp == "")
        return;

    std::vector<std::string> values = split(fmtp, std::string(";"));
    std::string sps_set = "";
    std::string sps_key = "sprop-parameter-sets=";
    for (const std::string &sps : values) {
        if (sps.find(sps_key) != std::string::npos) {
            sps_set =
                sps.substr(sps_key.length(), sps.length() - sps_key.length());
        }
    }

    if (sps_set == "")
        return;

    values.clear();
    values = split(sps_set, std::string(","));
    if (values.size() != 2)
        return;

    _sps = values[0];
    _pps = values[1];
    return;
}

} // namespace tlab::ext::live555