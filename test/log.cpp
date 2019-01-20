#include <gtest/gtest.h>
#include <tlab/log/logger.hpp>

void gprintf(const char *fmt, ...);

TEST(log,basic){
    //tlab::log::logger::instance().add_service(...);
    TLOG_D("message_value");
}