#pragma once
#include <functional>
struct Void
{
  Void() = default;
  Void(Void const &) = default;
  Void(Void &&) = default;
  Void &operator=(Void const &) = default;
  Void &operator=(Void &&) = default;
  template<typename Arg, typename... Args, std::enable_if_t<!std::is_base_of_v<Void, std::decay_t<Arg>>, int> = 0>
  explicit Void(Arg &&, Args &&...) {}
};
template<typename T>
using wrap_void_t = std::conditional_t<std::is_void_v<T>, Void, T>;
template<typename T>
using unwrap_void_t = std::conditional_t<std::is_same_v<std::decay_t<T>, Void>, void, T>;
template<typename F, typename... Args, typename Result = std::invoke_result_t<F, Args...>, std::enable_if_t<!std::is_void_v<Result>, int> = 0>
Result invoke_void(F &&f, Args &&...args)
{
  return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}
template<typename F, typename... Args, typename Result = std::invoke_result_t<F, Args...>, std::enable_if_t<std::is_void_v<Result>, int> = 0>
Void invoke_void(F &&f, Args &&...args)
{
  std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
  return Void();
}
