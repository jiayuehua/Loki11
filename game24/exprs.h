#pragma once

#include <vector>
#include <string>
#include <range/v3/view.hpp>
#include <range/v3/algorithm.hpp>
template <class I>
std::vector<std::string> generateExprs(I b, I e)
{
  int                      n = e - b;
  std::vector<std::string> v;
  v.reserve(24);
  if (n == 0) {
    v.push_back(std::string{});
    return v;
  } else if (n == 1) {
    v.push_back(std::string("") + *b + std::string(""));
    return v;
  } else {
    ranges::for_each(ranges::views::ints | ranges::views::drop(1) | ranges::views::take(n - 1), [b, e, &v](int i) {
      auto l = generateExprs(b, b + i);
      auto r = generateExprs(b + i, e);
      ranges::for_each(l, [&v, &r](auto&& li) {
        ranges::for_each(r, [&li, &v](auto&& ri) {
          std::string s = std::string("(") + li + std::string("+") + ri + ")";
          v.push_back(s);
          s = std::string("(") + li + std::string("-") + ri + ")";
          v.push_back(s);
          s = std::string("(") + li + std::string("*") + ri + ")";
          v.push_back(s);
          s = std::string("(") + li + std::string("/") + ri + ")";
          v.push_back(s);
        });
      });
    });
    return v;
  }
}
std::vector<std::string> getAllexprs(const std::vector<std::string>& v);
