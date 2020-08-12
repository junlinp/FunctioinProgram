#include <gtest/gtest.h>
#include "function_program.hpp"
TEST(FP, TRANSFORM) {

    auto functor = fp::View::transform([](){ return 0;});
    functor.functor_();
    std::vector<double> base = {1, 2, 3, 4, 5};
    auto trans = base | fp::View::transform([](double element){return 2 * element;});
    int count = 1;
    for(auto begin = trans.begin(); begin != trans.end(); ++begin) {
        EXPECT_EQ(*begin, 2 * count);
        count++;
    }
    count = 1;

    for(auto begin = trans.begin(); begin != trans.end(); ++begin) {
        EXPECT_EQ(*begin, 2 * count);
        count++;
    }
}
int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}