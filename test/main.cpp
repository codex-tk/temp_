#include <gtest/gtest.h>
#include <cstdarg>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}

void gprintf(const char *fmt, ...) {
    printf("[   USER   ] ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}