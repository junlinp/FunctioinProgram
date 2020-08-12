#include <gtest/gtest.h>
#include "function_program.hpp"
TEST(FP, TRANSFORM) {

    auto functor = fp::View::transform([](){ return 0;});
    functor.functor_();
}
int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}