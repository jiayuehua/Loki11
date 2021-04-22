#include <complex>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <range/v3/all.hpp>

#include "exprs.h"
#include "cal.h"
namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;
namespace parser {
using x3::eps;
using x3::int_;
using x3::lexeme;
x3::rule<class expr, int> const expr = "expr";
x3::rule<class term, int> const term = "term";
x3::rule<class factor, int> const factor = "factor";
x3::rule<class group, int> const group = "group";
auto indentity = [](auto &ctx) { _val(ctx) = _attr(ctx); };
auto multi = [](auto &ctx) { _val(ctx) *= _attr(ctx); };
auto divide = [](auto &ctx) {
  if (_attr(ctx) && (!(_val(ctx) % _attr(ctx)))) {
    _val(ctx) /= _attr(ctx);
  } else {
    _pass(ctx) = false;
  }
};
auto add = [](auto &ctx) { _val(ctx) += _attr(ctx); };
auto sub = [](auto &ctx) { _val(ctx) -= _attr(ctx); };
auto const expr_def = term[indentity] >> *(('+' >> term[add]) | ('-' >> term[sub]));//左结合
auto const term_def = factor[indentity] >> *(('*' >> (factor)[multi]) | ('/' >> (factor)[divide]));//左结合
auto const factor_def = (int_ | group);
auto const group_def = '(' >> expr >> ')';

BOOST_SPIRIT_DEFINE(expr, term, factor, group)
}// namespace parser
namespace {
bool validateOneArg(std::string const &sv) noexcept
{
  size_t l = 0;
  try {
    int n = std::stoi(sv, &l);
    return (l == sv.size() && (n >= 0) && (n < 14));
  } catch (...) {
    std::cerr << "validateOneArg: arg:" << sv << " stoi failed" << std::endl;
    return false;
  }
}
int validateArgs(int argc, char **argv, bool &retflag)
{
  retflag = true;
  if (argc < 5 || !std::all_of(&argv[1], &argv[5], validateOneArg)) {
    std::cerr << "usage: ./game [1-13] [1-13] [1-13] [1-13] \n";
    return 0;
  }
  retflag = false;
  return {};
}
std::vector<std::string> getAll24Exprs(const std::vector<std::string> &v)
{
  std::vector<std::string> expr24strs;
  expr24strs.reserve(8);
  using parser::expr;
  auto allexprs = getAllexprs(v);
  ranges::for_each(allexprs, [&expr24strs](auto &&oneexpr) {
    int result{ 0 };
    std::string::const_iterator iter = oneexpr.cbegin();
    std::string::const_iterator end = oneexpr.cend();
    bool r = x3::parse(iter, end, expr, result);
    if (r && iter == end && result == 24) {
      expr24strs.push_back(oneexpr);
    }
  });
  return expr24strs;
}

}// namespace

int main(int argc, char **argv)
{
  bool retflag{};
  int retval = validateArgs(argc, argv, retflag);
  if (retflag) return retval;

  std::vector v{ std::string(argv[1]), std::string(argv[2]), std::string(argv[3]), std::string(argv[4]) };
  for (auto &&val24exprstr : getAll24Exprs(v)) {
    //fmt::print("{}", val24exprstr)
    std::cout << val24exprstr << std::endl;
  }
}
