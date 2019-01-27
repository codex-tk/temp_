/**
 * @file console_output.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <tlab/log/outputs/console_output.hpp>

namespace tlab::log {

console_output::console_output(void) {
#if defined(_WIN32) || defined(__WIN32__)
    if (GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) {
        if (AllocConsole() == TRUE) {
            freopen("CONOUT$", "wt", stdout);
        }
    }
#endif
}

void console_output::put(const char *ptr, const std::size_t /*sz*/,
                                 const tlab::log::record &r) {
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
                            colors[static_cast<int>(r.lvl)]);
    printf("%s\n", ptr);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_BLUE | FOREGROUND_GREEN |
                                FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
    int colors[] = {32, 33, 36, 35, 31, 34};
    printf("\033[%dm%s\033[0m\n", colors[static_cast<int>(r.lvl)], ptr);
#endif
}

} // namespace tlab::log
