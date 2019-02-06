#include <gtest/gtest.h>
#include <tlab/util.hpp>

void gprintf(const char *fmt, ...);

TEST(util, clazz) {
    struct sample {
        int value;
        int value_4;
        void set(int i) { value = i; }
    };
    sample test_sample;
    test_sample.value = 0;

    tlab::clazz<sample>::mem_ptr_t<int> int_ptr(&sample::value);
    (test_sample.*int_ptr) = 1;

    ASSERT_EQ(test_sample.value, 1);

    tlab::clazz<sample>::mem_func_ptr_t<void(int)> func_ptr(&sample::set);
    (test_sample.*func_ptr)(2);

    ASSERT_EQ(test_sample.value, 2);

    ASSERT_EQ(tlab::clazz<sample>::offset_of(&sample::value), 0);
    ASSERT_EQ(tlab::clazz<sample>::offset_of(&sample::value_4), 4);

    ASSERT_EQ(
        tlab::clazz<sample>::container_of(&test_sample.value, &sample::value),
        &test_sample);
    ASSERT_EQ(
        tlab::clazz<sample>::container_of(&test_sample.value_4, &sample::value_4),
        &test_sample);
}

TEST(util,move){
    int v = 1;
    int v1 = std::move(v);
    ASSERT_EQ(v,1);
    void* p = &v;
    void* p2 = std::move(p);
    ASSERT_EQ(p,&v);
}