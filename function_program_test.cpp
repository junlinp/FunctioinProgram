#include "function_program.hpp"

#include <gtest/gtest.h>

#include "thread_pool.hpp"

#include "range/v3/all.hpp"

TEST(FP, TRANSFORM) {
  auto functor = fp::View::transform([]() { return 0; });
  functor.functor_();
  std::vector<int> base = {1, 2, 3, 4, 5};
  auto trans =
      base | fp::View::transform([](int element) { return 2 * element; });
  int count = 1;
  for (auto begin = trans.begin(); begin != trans.end(); ++begin) {
    EXPECT_EQ(*begin, 2 * count);
    count++;
  }
  count = 1;
  for (auto begin = trans.begin(); begin != trans.end(); ++begin) {
    EXPECT_EQ(*begin, 2 * count);
    count++;
  }

  auto transform_function = [](auto element) -> int { return 4 * element; };
  auto trans2 = base | fp::View::transform(transform_function);
  count = 1;
  for (auto begin = trans2.begin(); begin != trans2.end(); ++begin) {
    EXPECT_EQ(*begin, 4 * count);
    count++;
  }

  // lazy-value
  auto lazy = base | fp::View::transform([](int element) { return 3 * element;});
  base[0] = -1;
  EXPECT_EQ(*lazy.begin(), -3);

  for(int i  = 0; i < 1024 * 1024; i++) {
    int sum = 0;
     for (const auto& element : lazy) {
       sum += element;
     }
  }
}

TEST(FP, GET_KEY_VALUE) {
  std::map<int, int> map_;
  for (int i = 0; i < 1024; i++) {
    map_[i] = 2 * i;
  }

  auto key_res = map_ | fp::View::GetKey();
  auto value_res = map_ | fp::View::GetValue();

  int key_sum = 0, value_sum = 0;
  for (auto v : key_res) {
    key_sum += v;
  }
  int sum = key_res | fp::View::reduce(0);

  EXPECT_EQ(key_sum, (0 + 1023) * 1024 / 2);
  EXPECT_EQ(sum, (0 + 1023) * 1024 / 2);
  for (auto v : value_res) {
    value_sum += v;
  }

  EXPECT_EQ(value_sum, (0 + 2046) * 1024 / 2);
}

TEST(FP, FILTER) {}

TEST(ThreadPool, Enqueue) {
  ThreadPool thread_pool;
  auto future = thread_pool.Enqueue([]() { return 1; });
  EXPECT_EQ(future.get(), 1);
}

TEST(Range, MAP) {
  std::map<int, double> map_;
  map_[1] = 1.0;
  map_[2] = 2.0;
  map_ | ranges::view::transform([](const auto& pair) {
    return std::pair<int, double>(pair.first, pair.second * 2.0);
  });
}
int main() {
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}