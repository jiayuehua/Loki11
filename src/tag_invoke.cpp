#pragma once
#include <utility>
#include <type_traits>
#include <concepts>
#include <fmt/format.h>

namespace _tag_invoke {
void tag_invoke();

struct _fn
{
  template<typename CPO, typename... Args>
  constexpr auto operator()(CPO cpo, Args &&...args) const
    noexcept(noexcept(tag_invoke((CPO &&) cpo, (Args &&) args...)))
      -> decltype(tag_invoke((CPO &&) cpo, (Args &&) args...))
  {
    return tag_invoke((CPO &&) cpo, (Args &&) args...);
  }
};

template<typename CPO, typename... Args>
using tag_invoke_result_t = decltype(
  tag_invoke(std::declval<CPO>(), std::declval<Args>()...));

using yes_type = char;
using no_type = char (&)[2];

template<typename CPO, typename... Args>
auto try_tag_invoke(int)//
  noexcept(noexcept(tag_invoke(
    std::declval<CPO>(),
    std::declval<Args>()...)))
    -> decltype(static_cast<void>(tag_invoke(
                  std::declval<CPO>(),
                  std::declval<Args>()...)),
      yes_type{});

template<typename CPO, typename... Args>
no_type try_tag_invoke(...) noexcept(false);

template<template<typename...> class T, typename... Args>
struct defer
{
  using type = T<Args...>;
};

struct empty
{
};
}// namespace _tag_invoke

namespace _tag_invoke_cpo {
inline constexpr _tag_invoke::_fn tag_invoke{};
}
using namespace _tag_invoke_cpo;

template<auto &CPO>
using tag_t = std::remove_cvref_t<decltype(CPO)>;

using _tag_invoke::tag_invoke_result_t;

template<typename CPO, typename... Args>
inline constexpr bool is_tag_invocable_v =
  (sizeof(_tag_invoke::try_tag_invoke<CPO, Args...>(0)) == sizeof(_tag_invoke::yes_type));

template<typename CPO, typename... Args>
struct tag_invoke_result
  : std::conditional_t<
      is_tag_invocable_v<CPO, Args...>,
      _tag_invoke::defer<tag_invoke_result_t, CPO, Args...>,
      _tag_invoke::empty>
{
};

template<typename CPO, typename... Args>
using is_tag_invocable = std::bool_constant<is_tag_invocable_v<CPO, Args...>>;

template<typename CPO, typename... Args>
inline constexpr bool is_nothrow_tag_invocable_v =
  noexcept(_tag_invoke::try_tag_invoke<CPO, Args...>(0));

template<typename CPO, typename... Args>
using is_nothrow_tag_invocable =
  std::bool_constant<is_nothrow_tag_invocable_v<CPO, Args...>>;

template<typename CPO, typename... Args>
concept tag_invocable =
  (sizeof(_tag_invoke::try_tag_invoke<CPO, Args...>(0)) == sizeof(_tag_invoke::yes_type));


namespace M {
struct eq_fn
{
  template<typename T>
  requires std::same_as<
    tag_invoke_result_t<eq_fn, T const &, T const &>,
    bool> constexpr bool
    operator()(T const &x, T const &y) const
  {
    return tag_invoke(*this, x, y);
  }
};

inline constexpr eq_fn eq{};

struct ne_fn
{
  template<typename T>
  requires std::invocable<eq_fn, T const &, T const &> friend constexpr bool tag_invoke(
    ne_fn,
    T const &x,
    T const &y)
  {
    return not eq(x, y);
  }

  template<typename T>
  requires std::same_as<
    tag_invoke_result_t<ne_fn, T const &, T const &>,
    bool> constexpr bool
    operator()(T const &x, T const &y) const
  {
    return tag_invoke(*this, x, y);
  }
};

inline constexpr ne_fn ne{};

template<typename T>
concept equality_comparable =
  requires(std::remove_reference_t<T> const &t)
{
  eq(t, t);
  ne(t, t);
};
}// namespace M
namespace N {
struct eq_fn
{
  template<typename T>
  requires std::same_as<
    tag_invoke_result_t<eq_fn, T const &, T const &>,
    bool> constexpr bool
    operator()(T const &x, T const &y) const
  {
    return tag_invoke(*this, x, y);
  }
};

inline constexpr eq_fn eq{};

struct ne_fn
{
  template<typename T>
  requires std::invocable<eq_fn, T const &, T const &> friend constexpr bool tag_invoke(
    ne_fn,
    T const &x,
    T const &y)
  {
    return not eq(x, y);
  }

  template<typename T>
  requires std::same_as<
    tag_invoke_result_t<ne_fn, T const &, T const &>,
    bool> constexpr bool
    operator()(T const &x, T const &y) const
  {
    return tag_invoke(*this, x, y);
  }
};

inline constexpr ne_fn ne{};

template<typename T>
concept equality_comparable =
  requires(std::remove_reference_t<T> const &t)
{
  eq(t, t);
  ne(t, t);
};
}// namespace N
struct Widget
{
  int i;

  // with tag_invoke: we are visibly opting
  // into support for N::eq
  constexpr friend bool tag_invoke(tag_t<N::eq>,
    Widget a,
    Widget b)
  {
    return a.i == b.i;
  }
  constexpr friend bool tag_invoke(tag_t<M::eq>,
    Widget a,
    Widget b)
  {
    return a.i + 1 == b.i + 1;
  }
};
//void tag_invoke_test()
//{
//Widget wa{1},w{3};
//fmt::print("{}\n",N::eq(wa,w));
//fmt::print("{}\n",M::eq(wa,w));
//  static_assert(tag_invocable<N::eq_fn,Widget,Widget>);
//}
