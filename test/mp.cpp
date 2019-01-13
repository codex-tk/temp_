#include <gtest/gtest.h>
#include <tlab/mp.hpp>

void gprintf(const char *fmt, ...);

struct fold_expantion_tester {
    std::string value;
    void print(std::size_t i, int v) {
        value += (std::to_string(i) + " int " + std::to_string(v) + "\n");
    }

    void print(std::size_t i, char v) {
        value += (std::to_string(i) + " char " + std::to_string(v) + "\n");
    }

    void print(std::size_t i, double v) {
        value += (std::to_string(i) + " double " + std::to_string(v) + "\n");
    }

    void print(std::size_t i, const std::string &v) {
        value += (std::to_string(i) + " std::string " + v + "\n");
    }

    template <std::size_t... S, typename... Args>
    void test0(tlab::mp::index_sequence<S...>, Args &&... args) {
        TLAB_PACK_EXPANTION(print(S, args));
    }

    template <typename... Args> void test(Args... args) {
        test0(tlab::mp::make_index_sequence<sizeof...(args)>{},
              std::forward<Args>(args)...);
    }
};

TEST(tlab, mp_sequence) {
    static_assert(std::is_same<tlab::mp::make_index_sequence<4>,
                               tlab::mp::index_sequence<0, 1, 2, 3>>::value);
}

TEST(tlab, mp_expantion) {
    fold_expantion_tester td;
    td.print(0, int(1));
    td.print(1, char('a'));
    td.print(2, double(0.2));
    td.print(3, std::string("StringValue"));
    fold_expantion_tester tester;
    tester.test(int(1), char('a'), double(0.2), std::string("StringValue"));
    ASSERT_EQ(td.value, tester.value);
}

TEST(tlab_mp, index_sequence) {
    tlab::mp::index_sequence<0, 1> seq;
    ASSERT_EQ(seq.size(), 2);
    static_assert(
        std::is_same<tlab::mp::index_sequence<0, 1, 2>,
                     tlab::mp::make_index_sequence_linear<3>::type>::value);

    tlab::ignore = tlab::mp::make_index_sequence_linear<256>::type{};

    static_assert(
        std::is_same<tlab::mp::index_sequence<0, 1, 2>,
                     tlab::mp::make_index_sequence_merge<3>::type>::value);

    tlab::ignore = tlab::mp::make_index_sequence_merge<512>::type{};

    static_assert(std::is_same<tlab::mp::index_sequence<0, 1, 2>,
                               tlab::mp::make_index_sequence<3>>::value);

    tlab::ignore = tlab::mp::make_index_sequence<1024>{};
}

TEST(tlab_mp, type_list) {
    tlab::mp::type_list<int, char, std::string> tl;
    tlab::ignore = tl;
}

// template <typename... Ts> using tl = tlab::mp::type_list<Ts...>;
template <typename... Ts> using tl = std::tuple<Ts...>;

TEST(tlab_mp, push_back_front) {
    static_assert(
        std::is_same<
            tl<int, char, std::string>,
            tlab::mp::push_back<tl<int, char>, std::string>::type>::value);
            
    static_assert(
        std::is_same<
            tlab::mp::push_back<tl<int, char>, std::string>::type,
            tlab::mp::push_back<tl<int>, char, std::string>::type>::value);

    static_assert(
        std::is_same<
            tl<std::string, int, char>,
            tlab::mp::push_front<tl<int, char>, std::string>::type>::value);
}

TEST(tlab_mp, pop_back) {
    static_assert(std::is_same<tlab::mp::pop_back<tl<int>>::type, tl<>>::value);
    static_assert(
        std::is_same<tlab::mp::pop_back<tl<int, char>>::type, tl<int>>::value);
}

TEST(tlab_mp, pop_back0) {
    static_assert(
        std::is_same<tlab::mp::pop_back0<tl<int>>::type, tl<>>::value);
    static_assert(
        std::is_same<tlab::mp::pop_back0<tl<int, char>>::type, tl<int>>::value);
}

TEST(tlab_mp, at) {
    static_assert(
        std::is_same<tlab::mp::at<0, tl<int, char, double, std::string>>::type,
                     int>::value);
    static_assert(
        std::is_same<tlab::mp::at<1, tl<int, char, double, std::string>>::type,
                     char>::value);
    static_assert(
        std::is_same<tlab::mp::at<2, tl<int, char, double, std::string>>::type,
                     double>::value);
    static_assert(
        std::is_same<tlab::mp::at<3, tl<int, char, double, std::string>>::type,
                     std::string>::value);
}

TEST(tlab_mp, transform) {
    static_assert(
        std::is_same<
            tlab::mp::transform<tl<int &, char>, std::remove_reference>::type,
            tl<int, char>>::value);
}

TEST(tlab_mp, size) { ASSERT_EQ(tlab::mp::size<tl<int>>::value, 1); }