
#include <gtest/gtest.h>
#include <tlab/ext/live555/envir_loop.hpp>

TEST(live555, BasicUsageEnvironment) {
    tlab::ext::live555::envir_loop loop;
    loop.run([] {});
}