/**
 * @file error.cpp
 * @author ghtak
 * @brief 
 * @version 0.1
 * @date 2019-01-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <gtest/gtest.h>
#include <tlab/error.hpp>

TEST(error_system, basic) {
    std::error_code ec;
    ASSERT_FALSE(ec == tlab::errc::success);

    ASSERT_FALSE(tlab::make_error_code(tlab::errc::success));

    ec = tlab::make_error_code(tlab::errc::success);
    ASSERT_FALSE(ec == std::errc::operation_canceled);

    ec = tlab::make_error_code(tlab::errc::operation_canceled);
    ASSERT_TRUE(ec == std::errc::operation_canceled);
    ASSERT_TRUE(std::errc::operation_canceled == ec);
}

// error_code
template <typename Enum> struct is_check_sample : std::false_type {};

struct sample_wrapper {
    template <typename Enum, typename = typename std::enable_if_t<
                                 is_check_sample<Enum>::value>>
    sample_wrapper(Enum e) {
        value = adl_function(e);
    }

    int value;
};

bool operator==(const sample_wrapper &lhs, const sample_wrapper &rhs) {
    return lhs.value == rhs.value;
}

namespace internal {

enum sample {
    sample_0,
    sample_1,
};

int adl_function(sample s) { return static_cast<int>(s); }

} // namespace internal

template <> struct is_check_sample<internal::sample> : std::true_type {};

TEST(error_system, adl) {
    sample_wrapper s(internal::sample_0);

    ASSERT_TRUE(internal::sample_0 == s);
    ASSERT_EQ(internal::sample_0, s);
}