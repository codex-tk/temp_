#include <gtest/gtest.h>
#include <tlab/log/basic_logging_service.hpp>
#include <tlab/log/expr.hpp>
#include <tlab/log/logger.hpp>
#include <tlab/mp.hpp>

#define _CRT_SECURE_NO_WARNINGS

void gprintf(const char *fmt, ...);

class timestamp {
public:
    template <typename StreamT> static void print(StreamT &strm) {
        // std::time_t now = std::chrono::system_clock::to_time_t(
        //    std::chrono::system_clock::now());
        // std::tm *ptm = std::gmtime(&now);
        // strm << std::ctime(&now);
        strm << "timestamp";
    }
};

class message {
public:
    template <typename StreamT> static void print(StreamT &strm) {
        strm << "message";
    }
};

template <char... Cs> class _c {
public:
    template <typename StreamT> static void print(StreamT &strm) {
        ((strm << Cs), ...);
    }
};

template <char *message> struct _s {
    template <typename StreamT> static void print(StreamT &strm) {
        strm << message;
    }
};

template <char prefix, typename SentenceT, char postfix> class _w {
public:
    template <typename StreamT> static void print(StreamT &strm) {
        strm << prefix;
        SentenceT::print(strm);
        strm << postfix;
    }
};

template <char b, char e, typename... Ts> class w0 {
public:
    template <typename StreamT, typename SentenceT>
    static void print_expansion(StreamT &strm) {
        _w<b, SentenceT, e>::print(strm);
    }

    template <typename StreamT> static void print(StreamT &strm) {
        (print_expansion<StreamT, Ts>(strm), ...);
    }
};

template <typename... Ts> class format_service {
public:
    std::string value(void) {
        std::stringstream ss;
        (Ts::print(ss), ...);
        return ss.str();
    }
};

TEST(log, format) {
    format_service< _w<'[', timestamp,  ']'>, _c<' '>,
                    _w<'[', message,    ']'>, _c<' '>, 
                    _w<'[', message,    ']'>, _c<' '>,
                    _w<'[', message,    ']'>, _c<' '>, 
                    _w<'[', message,    ']'>, _c<' '>, 
                    timestamp> svc;

    ASSERT_EQ(
        std::string(
            "[timestamp] [message] [message] [message] [message] timestamp"),
        svc.value());

    format_service<w0<'[', ']', timestamp, message, message>> svc2;

    ASSERT_EQ(
        std::string(
            "[timestamp][message][message]"),
        svc2.value());
}
namespace {

template <std::size_t I> using const_str_type = char const (&)[I];

template <std::size_t N> constexpr std::size_t length(const_str_type<N>) {
    return N - 1;
}

template <std::size_t I, std::size_t N>
constexpr char at(const_str_type<N> arr) {
    return arr[I];
}

template <std::size_t I, const_str_type<I + 1> arr>
struct const_str_template {};

template <char... Chars> struct chars {};
/*
template <typename T, const char *arr> struct const_str_template0;

template <char Ch> struct ch_fw { static const char ch = ch; };

template <std::size_t I, const char *arr> struct ch_at {
    static const char ch = arr[I];
};


template <std::size_t... Is, const char *arr>
struct const_str_template0<tlab::mp::index_sequence<Is...>, arr> {
    using type = chars<ch_at<Is, arr>::ch...>;
};
*/
}
TEST(log, const_str_type_) {
    static_assert(std::is_same<tlab::mp::make_index_sequence<length("Hello")>,
                               tlab::mp::index_sequence<0, 1, 2, 3, 4>>::value);
    ASSERT_EQ(at<0>("Hello"), 'H');

    const char hello[] = "Hello";
    // const_str_template<length("Hello") , "Hello" > e;
    /*
    const_str_template<length(hello) , hello> e;
    */
    // const_str_template0<tlab::mp::make_index_sequence<length("Hello")>,hello>::type
    // t;
}

TEST(log, expr) {
    tlab::log::expr::basic_format formatter;

    std::stringstream ss;

    tlab::log::local_context lctx(__FILE__, __FUNCTION__, __LINE__);
    tlab::log::record r(tlab::log::level::debug, "tag", "message", lctx);
    formatter.append_to(ss, r);
    gprintf("%s", ss.str().c_str());
}

class gprintf_ostream {
public:
    struct record_stream_type {
        std::stringstream ss;
        tlab::log::level lvl;

        template <typename T> record_stream_type &operator<<(const T &t) {
            ss << t;
            return *this;
        }
    };

    struct extract_level {
        static void append_to(record_stream_type &strm,
                              const tlab::log::record &r) {
            strm.lvl = r.lvl;
        }
    };

    void operator<<(const record_stream_type &rstrm) {
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
                                colors[static_cast<int>(rstrm.lvl)]);
#else
        int colors[] = {32, 33, 36, 35, 31, 34};
        printf("\033[%dm", colors[static_cast<int>(rstrm.lvl)]);
#endif
        gprintf("%s", rstrm.ss.str().c_str());
#if defined(_WIN32) || defined(__WIN32__)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_BLUE | FOREGROUND_GREEN |
                                    FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
        printf("\033[0m");
#endif
    }

    template <typename T> struct rebind { using type = T; };

    template <template <typename...> class List, typename... Ts>
    struct rebind<List<Ts...>> {
        using type = List<extract_level, Ts...>;
    };
};

TEST(log, logger) {
    std::shared_ptr<tlab::log::logging_service> svc(
        new tlab::log::basic_logging_service<tlab::log::expr::basic_format,
                                             gprintf_ostream>());
    tlab::log::logger::instance().add_service(svc);
    TLOG_D("test message");
    TLOG_D("test message int %d", int(32));
}