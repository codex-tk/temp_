#include <gtest/gtest.h>
#include <tlab.hpp>

TEST(tlab,something){
    ASSERT_EQ(tlab::something() , 81 );
}