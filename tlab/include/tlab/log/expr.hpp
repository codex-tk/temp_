/**
 * @file expr.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2019-01-21
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __tlab_log_expr_h__
#define __tlab_log_expr_h__

#include <tlab/log/log.hpp>

namespace tlab::log::expr {

struct timestamp {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &) {
        std::time_t now = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        std::tm tm;
#if defined(_WIN32) || defined(__WIN32__)
        localtime_s(&tm, &now);
#else
        localtime_r(&now , &tm);
#endif
        char buf[16] = {
            0,
        };
        snprintf(buf, 32, "%04d%02d%02d %02d%02d%02d", tm.tm_year + 1900,
                 tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        strm << buf;
    }
};

struct level {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &r) {
        switch(r.lvl){
        case log::level::trace: strm << 'T'; break;
        case log::level::debug: strm << 'D'; break;
        case log::level::info:  strm << 'I'; break;
        case log::level::warn:  strm << 'W'; break;
        case log::level::error: strm << 'E'; break;
        default: strm << '?'; break;
        }
    }
};

struct tag {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &r) {
        strm << r.tag;
    }
};

struct message {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &r) {
        strm << r.message;
    }
};

struct file {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &r) {
        std::string path(r.local_context.file);
        strm << path.substr(path.find_last_of("/\\")+1);
    }
};

struct function {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &r) {
        strm << r.local_context.function;
    }
};

struct line {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &r) {
        strm << r.local_context.line;
    }
};

struct thread_id {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &) {
        strm << std::this_thread::get_id();
    }
};

template <char Ch> struct ch {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &) {
        strm << Ch;
    }
};

template <char Prefix, char PostFix, typename... Exprs> struct wrap_expr {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &r) {
        strm << Prefix;
#if __cplusplus > 201402L
        (Exprs::append_to(strm, r), ...);
#else
        int dummy[] = {(Exprs::append_to(strm, r),0) ... };
        tlab::ignore = dummy;
#endif
        strm << PostFix;
    }
};

template <typename... Ts> using wrap = wrap_expr<'[', ']', Ts...>;

template <typename... Exprs> struct format {
    template <typename StreamT>
    static void append_to(StreamT &strm, const log::record &r) {
#if __cplusplus > 201402L
        (Exprs::append_to(strm, r), ...);
#else
        int dummy[] = {(Exprs::append_to(strm, r),0) ... };
        tlab::ignore = dummy;
#endif
    }
};

} // namespace tlab::log::expr

#endif