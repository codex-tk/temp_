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
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &) {
        std::time_t now = std::time(nullptr);
        struct std::tm tm;
#if defined(_WIN32) || defined(__WIN32__)
        localtime_s(&tm, &now);
#else
        localtime_r(&now, &tm);
#endif
        char timestr[16] = {
            0,
        };
        snprintf(timestr, 16, "%04d%02d%02d %02d%02d%02d", tm.tm_year + 1900,
                 tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        buf << timestr;
    }
};

struct level {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
        switch (r.lvl) {
        case log::level::trace: buf << 'T'; break;
        case log::level::debug: buf << 'D'; break;
        case log::level::info: buf << 'I'; break;
        case log::level::warn: buf << 'W'; break;
        case log::level::error: buf << 'E'; break;
        default: buf << '?'; break;
        }
    }
};

struct ansi_color_begin {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
        /*
#if defined(_WIN32) || defined(__WIN32__)
        WORD colors[7] = {FOREGROUND_GREEN,
                          FOREGROUND_GREEN | FOREGROUND_RED,
                          FOREGROUND_BLUE | FOREGROUND_GREEN,
                          FOREGROUND_BLUE | FOREGROUND_RED,
                          FOREGROUND_RED,
                          FOREGROUND_BLUE,
                          FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED |
                              FOREGROUND_INTENSITY};
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
colors[static_cast<int>(r.lvl)]); tlab::ignore = buf; #else int colors[] = {32,
33, 36, 35, 31, 34}; buf << "\033[" << colors[static_cast<int>(r.lvl)] << "m";
#endif  */
        int colors[] = {32, 33, 36, 35, 31, 34};
        buf << "\033[" << colors[static_cast<int>(r.lvl)] << "m";
    }
};

struct ansi_color_end {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &) {
        /*
#if defined(_WIN32) || defined(__WIN32__)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE
| FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY); tlab::ignore = buf;
#else
        buf << "\033[0m";
#endif  */
        buf << "\033[0m";
    }
};

struct tag {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
        buf << r.tag;
    }
};

struct message {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
        buf << r.message;
    }
};

struct file {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
        std::string path(r.local_context.file);
        buf << path.substr(path.find_last_of("/\\") + 1);
    }
};

struct function {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
        buf << r.local_context.function;
    }
};

struct line {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
        buf << r.local_context.line;
    }
};

struct thread_id {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &) {
        buf << std::this_thread::get_id();
    }
};

template <char... Chars> struct chars {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &) {
#if __cplusplus > 201402L
        ((buf << Chars), ...);
#else
        int dummy[] = {((buf << Chars), 0)...};
        tlab::ignore = dummy;
#endif
    }
};

template <typename... Exprs> struct list {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
#if __cplusplus > 201402L
        (Exprs::write(buf, r), ...);
#else
        int dummy[] = {(Exprs::write(buf, r), 0)...};
        tlab::ignore = dummy;
#endif
    }
};

template <char Prefix, typename Expr, char PostFix> struct wrap {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
        buf << Prefix;
        Expr::write(buf, r);
        buf << PostFix;
    }
};

template <typename... Exprs> struct format {
    template <typename BufferT>
    static void write(BufferT &buf, const log::record &r) {
#if __cplusplus > 201402L
        (Exprs::write(buf, r), ...);
#else
        int dummy[] = {(Exprs::write(buf, r), 0)...};
        tlab::ignore = dummy;
#endif
    }
};

using basic_format = format<
    wrap<'[', timestamp, ']'>, wrap<'[', level, ']'>, wrap<'[', message, ']'>,
    wrap<'[', list<file, chars<':'>, line>, ']'>, wrap<'[', thread_id, ']'>>;

} // namespace tlab::log::expr

#endif