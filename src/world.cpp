#include "fold_wrapper.hpp"
#include <tuple>
#include <utility>
#include <iostream>
int main()
{
  std::tuple<int> a{ 1 };
  std::tuple<float> b{ 1.2 };
  auto r = fold_left([](auto &&x, auto &&y) {
    return std::tuple_cat(std::forward<decltype(x)>(x),
      std::forward<decltype(y)>(y));
  },
    a,
    b);
  std::cout << std::get<1>(r) << std::endl;
}
