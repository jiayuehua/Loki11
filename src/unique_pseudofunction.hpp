#pragma once
#include <new>
#include <type_traits>
#include <utility>
#include <functional>
#include <memory>

namespace std {
template<typename T>
using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;
}

union buffer {
  mutable char here[32];
  void *there;

  template<typename T>
  static constexpr bool fits() noexcept
  {
    return sizeof(T) <= sizeof(buffer) && alignof(T) <= alignof(buffer) && std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>;
  }

  template<typename T, typename... ARGS>
  void construct(ARGS &&...args)
  {
    if constexpr (fits<T>()) {
      new (here) T(std::forward<ARGS>(args)...);
    } else {
      there = new T(std::forward<ARGS>(args)...);
    }
  }

  template<typename T>
  void destroy() noexcept
  {
    if constexpr (fits<T>()) {
      reinterpret_cast<T *>(here)->~T();
    } else {
      delete static_cast<T *>(there);
    }
  }

  template<typename T>
  void move_to(buffer &other) noexcept
  {
    if constexpr (fits<T>()) {
      new (other.here) T(std::move(*reinterpret_cast<T *>(here)));
    } else {
      other.there = std::exchange(there, nullptr);
    }
  }

  template<typename T>
  T &get() const noexcept
  {
    if constexpr (fits<T>()) {
      return *reinterpret_cast<T *>(here);
    } else {
      return *static_cast<T *>(there);
    }
  }
};

inline void destroy_move_impl(buffer &, buffer *) noexcept {}

template<typename T>
void destroy_move_impl(buffer &from, buffer *to) noexcept
{
  if (to) {
    from.move_to<T>(*to);
  }
  from.destroy<T>();
}

template<typename RET, typename... ARGS>
RET invoke_impl(buffer const &, ARGS...)
{
  throw std::bad_function_call();
}

template<typename T, typename RET, typename... ARGS>
RET invoke_impl(buffer const &data, ARGS... args) noexcept
{
  return std::invoke(data.get<T>(), std::forward<ARGS>(args)...);
}

template<typename>
struct vtable_entry;// Not defined.

template<typename RET, typename... ARGS>
struct vtable_entry<RET(ARGS...)>
{
  RET (*invoke)
  (buffer const &, ARGS...);

  constexpr vtable_entry() noexcept : invoke(&invoke_impl) {}

  template<typename T, typename = std::enable_if_t<std::is_invocable_r_v<RET, T, ARGS...>>>
  constexpr explicit vtable_entry(std::in_place_type_t<T> const &) noexcept : invoke(&invoke_impl<T>)
  {
  }

  vtable_entry(vtable_entry const &) = delete;

  constexpr vtable_entry(vtable_entry &&other) noexcept : invoke(std::exchange(other.invoke, &invoke_impl)) {}

  vtable_entry &operator=(vtable_entry const &) = delete;

  constexpr vtable_entry &operator=(vtable_entry &&other) noexcept
  {
    invoke = std::exchange(other.invoke, &invoke_impl);
    return *this;
  }

  void swap(vtable_entry &other) noexcept { std::swap(invoke, other.invoke); }
};

struct unique_lifetime;
template<typename, typename>
struct pseudo_base;
template<typename... FUNCS>
class unique_pseudofunction;

template<typename RET, typename... ARGS, typename... FUNCS>
struct pseudo_base<RET(ARGS...), unique_pseudofunction<FUNCS...>> : vtable_entry<RET(ARGS...)>
{
  using vtable_entry<RET(ARGS...)>::vtable_entry;

  constexpr pseudo_base(vtable_entry<RET(ARGS...)> &&other) noexcept : vtable_entry<RET(ARGS...)>(std::move(other)) {}

  constexpr pseudo_base &operator=(vtable_entry<RET(ARGS...)> &&other) noexcept
  {
    vtable_entry<RET(ARGS...)>::operator=(std::move(other));
    return *this;
  }

  RET operator()(ARGS... args) const
  {
    unique_pseudofunction<FUNCS...> const &f = static_cast<unique_pseudofunction<FUNCS...> const &>(*this);
    return vtable_entry<RET(ARGS...)>::invoke(f.data, std::forward<ARGS>(args)...);
  }
};

struct unique_lifetime
{
  buffer data;
  void (*destroy_move)(buffer &, buffer *) noexcept;

  unique_lifetime() noexcept : destroy_move(&destroy_move_impl) {}

  unique_lifetime(unique_lifetime const &) = delete;

  unique_lifetime(unique_lifetime &&other) noexcept
    : destroy_move(std::exchange(other.destroy_move, &destroy_move_impl))
  {
    destroy_move(other.data, &data);
  }

  unique_lifetime &operator=(unique_lifetime const &) = delete;

  unique_lifetime &operator=(unique_lifetime &&other) noexcept
  {
    if (&other != this) {
      destroy_move(data, nullptr);
      destroy_move = std::exchange(other.destroy_move, &destroy_move_impl);
      destroy_move(other.data, &data);
    }
    return *this;
  }

  ~unique_lifetime() noexcept { destroy_move(data, nullptr); }

  template<typename T, typename... ARGS>
  unique_lifetime(std::in_place_type_t<T> const &, ARGS &&...args) : destroy_move(destroy_move_impl<T>)
  {
    data.construct<T>(std::forward<ARGS>(args)...);
  }
};

template<typename... FUNCS>
class unique_pseudofunction : unique_lifetime
  , pseudo_base<FUNCS, unique_pseudofunction<FUNCS...>>...
{
  template<typename, typename>
  friend struct pseudo_base;
  template<typename...>
  friend class unique_pseudofunction;

public:
  unique_pseudofunction() noexcept = default;
  unique_pseudofunction(unique_pseudofunction &&) noexcept = default;
  unique_pseudofunction &operator=(unique_pseudofunction &&) noexcept = default;
  ~unique_pseudofunction() noexcept = default;

  using pseudo_base<FUNCS, unique_pseudofunction>::operator()...;

  template<
    typename T,
    typename = std::enable_if_t<
      !std::is_same_v<unique_pseudofunction, std::remove_cvref_t<T>> && (... && std::is_constructible_v<vtable_entry<FUNCS>, std::in_place_type_t<std::remove_cvref_t<T>> const &>)>>
  unique_pseudofunction(T &&data)
    : unique_lifetime(std::in_place_type<std::remove_cvref_t<T>>, std::forward<T>(data)),
      pseudo_base<FUNCS, unique_pseudofunction>(std::in_place_type<std::remove_cvref_t<T>>)...
  {
  }

  template<typename... OTHER_FUNCS,
    typename = std::enable_if_t<
      (... && std::is_base_of_v<vtable_entry<FUNCS>, unique_pseudofunction<OTHER_FUNCS...>>)>>
  unique_pseudofunction(unique_pseudofunction<OTHER_FUNCS...> &&other) noexcept
    : unique_lifetime(std::move(static_cast<unique_lifetime &>(other))),
      pseudo_base<FUNCS, unique_pseudofunction>(std::move(static_cast<vtable_entry<FUNCS> &>(other)))...
  {
  }
};

unique_pseudofunction<void(int), int()> convert(unique_pseudofunction<int(), void(int)> value) { return value; }
#include <iostream>
struct A
{
  void operator()(int)
  {
    std::cout << "foo\n";
  }
  int operator()()
  {
    std::cout << "bar";
    return 3;
  }
};
//int main()
//{
//  A a1;
//  unique_pseudofunction<void(int),int()> uf(a1);
//  uf(1);
//  std::cout<<uf()<<std::endl;
//  unique_pseudofunction<void(int)> uf2(std::move(uf));
//  uf2(3);
//
//}
