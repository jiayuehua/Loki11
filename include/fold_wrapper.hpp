#pragma once
#include <utility>
template<class F, class X>
struct fold_wrapper
{
  F f;
  X state;
  template<typename FoldWrapper>
  constexpr auto operator>>=(FoldWrapper &&fw)
  {
    auto result = f(state, std::forward<FoldWrapper>(fw).state);
    return fold_wrapper<F, decltype(result)>{ f, result };
  }
};
template<class F, class... Xs>
constexpr auto fold_left(const F &f, Xs &&...xs)
{
  auto result = (... >>= fold_wrapper<F, Xs>{ f, std::forward<Xs>(xs) });
  return result.state;
}
