#pragma once
#include <type_traits>

template<class T>
concept AlwaysTrue =
  std::is_void_v<std::void_t<T>>;

template<class T>
concept EvenMoreTrue = AlwaysTrue<T> &&
  std::is_void_v<std::void_t<T, void>>;

template<class... Ts>
struct Overload : Ts...
{
  explicit constexpr Overload(Ts... ts) : Ts(ts)... {}
  using Ts::operator()...;
};

template<class, class = int>
struct HasUnambiguousCallOperator : std::false_type
{
};

template<class MAB>
struct HasUnambiguousCallOperator<MAB, decltype(std::declval<MAB>()(0))>
  : std::true_type
{
};

template<class MAB>
constexpr bool extract_value(MAB)
{
  return HasUnambiguousCallOperator<MAB>::value;
}

#ifdef MAKE_IT_COMPILE_BUT_NOT_DO_THE_JOB
#define SUBSUMES(A, B)    \
  extract_value(Overload( \
    [](auto) {},          \
    [](auto) { return 0; }))
#else
#define SUBSUMES(A, B)                               \
  extract_value(Overload(                            \
    []<class T>(T) requires B<T> || AlwaysTrue<T>{}, \
    []<class T>(T) requires A<T> || EvenMoreTrue<T> { return 0; }))
#endif

// ========================================

template<class T>
concept Scalar = std::is_scalar_v<T>;
template<class T>
concept Integral = Scalar<T> &&std::is_integral_v<T>;

static_assert(SUBSUMES(Integral, Scalar));
static_assert(not SUBSUMES(Scalar, Integral));
