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

    template <std::size_t ... S , typename... Args >
    void test0(tlab::mp::index_sequence<S...> , Args&&... args) {
        TLAB_PACK_EXPANTION(print(S,args));
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

TEST(tlab,mp_expantion){
    fold_expantion_tester td;
    td.print(0,int(1));
    td.print(1,char('a'));
    td.print(2,double(0.2));
    td.print(3,std::string("StringValue"));
    fold_expantion_tester tester;
    tester.test(int(1), char('a'), double(0.2), std::string("StringValue"));
    ASSERT_EQ(td.value,tester.value);
}