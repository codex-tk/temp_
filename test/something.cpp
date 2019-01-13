#include <gtest/gtest.h>
#include <tlab/tlab.hpp>

TEST(tlab,something){
    ASSERT_EQ(tlab::something() , 81 );
}