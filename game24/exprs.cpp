#include <algorithm>
#include <string>
#include <vector>
#include <range/v3/all.hpp>
#include "exprs.h"
namespace
{

std::vector<std::vector<std::string>> getAllPerms(std::vector<std::string> v)
{
  std::vector<std::vector<std::string>> r;
  r.push_back(v);
  while (std::next_permutation(v.begin(), v.end())) {
    r.push_back(v);
  }
  return r;
}
}
std::vector<std::string> getAllexprs(const std::vector<std::string>& v)
{
  std::vector<std::string> r;
  auto                     all = getAllPerms(v);
  ranges::for_each(all, [&r](auto&& i) {
    auto pairs = generateExprs(i.begin(), i.end());
    r.insert(r.end(), pairs.begin(), pairs.end());
  });
  return r;
}
