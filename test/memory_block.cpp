#include <gtest/gtest.h>
#include <tlab/basic_buffer.hpp>
#include <tlab/memory_block.hpp>

TEST(tlab, memory_block) {
    tlab::memory_block<char> block;
    tlab::memory_block<char> block_10(10);

    do {
        tlab::memory_block<char> block_10_r(block_10);
        ASSERT_EQ(block_10.use_count(), block_10_r.use_count());
        ASSERT_EQ(tlab::block_refs(block_10), 2);
        ASSERT_EQ(tlab::block_refs(int()), 0);
        ASSERT_EQ(block_10.ptr(), block_10_r.ptr());
        ASSERT_EQ(block_10.size(), block_10_r.size());
    } while (0);

    ASSERT_EQ(tlab::block_refs(block_10), 1);
}

TEST(tlab, byte_buffer) {
    tlab::byte_buffer buf(32);
    tlab::byte_buffer buf2(buf);
    ASSERT_EQ(buf.gptr(), buf2.gptr());
    buf.reserve(64);
    ASSERT_NE(buf.gptr(), buf2.gptr());
}