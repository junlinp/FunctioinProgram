#include <gtest/gtest.h>
#include "function_program.hpp"
TEST(FP, TRANSFORM) {

    auto functor = fp::View::transform([](){ return 0;});
    functor.functor_();
    std::vector<int> base = {1, 2, 3, 4, 5};
    auto trans = base | fp::View::transform([](int element){return 2 * element;});
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

    auto transform_function = [](auto element) -> int{ return 4 * element;};
    auto trans2 = base | fp::View::transform(transform_function);
    count = 1 ;
    for(auto begin = trans2.begin(); begin != trans2.end(); ++begin) {
        EXPECT_EQ(*begin, 4 * count);
        count++;
    }
}
int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}