/**
 * @file util.cpp
 * @author ghtak
 * @brief
 * @version 0.1
 * @date 2019-01-19
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <tlab/util.hpp>

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#else
#if defined(_WIN32) || defined(__WIN32__)
#include <Dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif
#endif

namespace tlab {

uint16_t checksum(void *p, int sz) {
    assert(p != nullptr);
    assert(sz >= 0);
    int32_t sum = 0;
    if (p != nullptr) {
        uint16_t *ptr = static_cast<uint16_t *>(p);
        for (; sz > 1; sz -= 2) {
            sum += *ptr;
            ++ptr;
        }
        if (sz == 1)
            sum += static_cast<uint16_t>(*reinterpret_cast<uint8_t *>(ptr));
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return static_cast<uint16_t>(~sum);
}

std::string demangle(const char *name) {
#ifdef __GNUG__
    int status = -4;
    // some arbitrary value to eliminate the compiler warning
    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void (*)(void *)> res{
        abi::__cxa_demangle(name, NULL, NULL, &status), std::free};
    return (status == 0) ? res.get() : name;
#else
#if defined(_WIN32) || defined(__WIN32__)
    /*
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    SymInitialize(GetCurrentProcess(), NULL, TRUE);
    char undecorated_name[1024];
    if (!UnDecorateSymbolName(name, undecorated_name, sizeof(undecorated_name) /
    sizeof(*undecorated_name), UNDNAME_COMPLETE))
    {
            return undecorated_name;
    }
    */
#endif
    return name;
#endif
}

std::string hex_str(void *ptr, const int size, const int space,
                    const int crlf) {
    uint8_t *p = static_cast<uint8_t *>(ptr);
    const char map[] = "0123456789ABCDEF";
    std::string ans;
    ans.reserve(size * 2 + size / space + size / crlf);
    for (int i = 0; i < size; ++i) {
        ans += map[p[i] >> 4];
        ans += map[p[i] & 0x0f];
        if ((i + 1) % space == 0) {
            ans += ' ';
        }
        if ((i + 1) % crlf == 0) {
            ans += "\r\n";
        }
    }
    return ans;
}

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


std::string base64::encode(void *ptr, const std::size_t size){
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
int base64::decode(const std::string &src, void *ptr,
                      const std::size_t size)
{
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
        //if ((c > 127) || (c < 0) || (b64_table_r[c] > 63)) {}
        accumulator = (accumulator << 6) | b64_table_r[c];
        bits_collected += 6;
        if (bits_collected >= 8) {
            bits_collected -= 8;
            buffer[index++] = (char)((accumulator >> bits_collected) & 0xff);
        }
    }
    return index;
}

} // namespace tlab