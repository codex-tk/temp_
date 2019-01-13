/**
 * @file internal.hpp
 * @author ghtak
 * @brief 
 * @version 0.1
 * @date 2019-01-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __tlab_internal_h__
#define __tlab_internal_h__

/*
Darwin	 DARWIN						Darwin is not necessarily OS X, see below
FreeBSD	 __FreeBSD__				Consider using the BSD macro
Linux	 linux or __linux			I also found LINUX
MacOS X	 __MACOSX__ or __APPLE__
NetBSD	 __NetBSD__					Consider using the BSD macro
OpenBSD	 __OpenBSD__				Consider using the BSD macro
Solaris	 sun or __sun				SunOS versions < 5 will not have __SVR4 or __svr4__ defined
Windows	 _WIN32 or __WIN32__
*/

#if defined(_WIN32) || defined(__WIN32__)

#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#include <WinSock2.h>
#include <MSWsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>

#else

#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>

#if defined(linux) || defined(__linux)
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#endif
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cstdarg>

#include <string>
#include <system_error>
#include <chrono>
#include <queue>
#include <array>
#include <atomic>
#include <memory>
#include <functional>
#include <cassert>
#include <sstream>
#include <mutex>
#include <thread>
#include <ctime>

#define _USE_MATH_DEFINES

#include <math.h>

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

// old macro for remove unused warning
#ifndef TLAB_IGNORE
#define TLAB_IGNORE(expr)                                                      \
    do {                                                                       \
        (void)expr;                                                            \
    } while (0)
#endif

namespace tlab {
namespace internal {

template <typename T> constexpr void ignore(const T &) {}

struct _ignore {
    template <typename T> constexpr const _ignore &operator=(const T &) const {
        return (*this);
    }
};

} // namespace internal

constexpr internal::_ignore ignore{};

/**
 * @brief 
 * 
 */
class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;
};

} // namespace tlab

#endif