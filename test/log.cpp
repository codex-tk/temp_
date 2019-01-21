#include <gtest/gtest.h>
#include <tlab/log/logger.hpp>
#include <tlab/log/expr.hpp>
#include <tlab/mp.hpp>

#define _CRT_SECURE_NO_WARNINGS

void gprintf(const char *fmt, ...);

TEST(log, basic) {
    TLOG_D("message_value");
}

class timestamp {
public:
    template <typename StreamT> static void print(StreamT &strm) {
        //std::time_t now = std::chrono::system_clock::to_time_t(
        //    std::chrono::system_clock::now());
        //std::tm *ptm = std::gmtime(&now);
        //strm << std::ctime(&now);
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

template <char* message> struct _s{
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
    format_service< _w<'[', timestamp, ']'>, _c<' '>, 
                    _w<'[', message  , ']'>, _c<' '>, 
                    _w<'[', message  , ']'>, _c<' '>, 
                    _w<'[', message  , ']'>, _c<' '>, 
                    _w<'[', message  , ']'>, _c<' '>,
                    timestamp> svc;

    gprintf("%s", svc.value().c_str());

    format_service<w0<'[', ']', timestamp, message, message>> svc2;
    gprintf("%s", svc2.value().c_str());
}

template <std::size_t N> constexpr std::size_t length(char const (&)[N]) {
    return N - 1;
}

template <std::size_t I,std::size_t N> constexpr char at(char const (&arr)[N]){
    return arr[I];
}

template <std::size_t I , char const (&Str)[I + 1]> struct eeeee{

};

TEST(log,expand){
    tlab::mp::make_index_sequence<length("Hello")> type;
    tlab::ignore = type;
    at<0>("Hello");

    ///eeeee<length("Hello") , "Hello" > e;
}

TEST(log,expr){
    tlab::log::expr::format<
            tlab::log::expr::wrap<tlab::log::expr::timestamp>,
            tlab::log::expr::wrap<tlab::log::expr::level>,
            tlab::log::expr::wrap<tlab::log::expr::tag>,
            tlab::log::expr::wrap<tlab::log::expr::message>,
            tlab::log::expr::wrap<
                tlab::log::expr::file, 
                tlab::log::expr::ch<':'>, 
                tlab::log::expr::line> ,
            tlab::log::expr::wrap<tlab::log::expr::thread_id>
    > formatter;
    
    std::stringstream ss;

    tlab::log::local_context lctx(__FILE__, __FUNCTION__, __LINE__);
    tlab::log::record r(tlab::log::level::debug, "tag", "message", lctx);
    formatter.append_to(ss, r);
    gprintf("%s", ss.str().c_str());
}