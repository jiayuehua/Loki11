#pragma once
#include <concepts>
struct boolean
{
private:
  bool _val;

public:
  constexpr boolean() noexcept = default;
  template<std::convertible_to<bool> T>
  explicit(!std::same_as<T, bool>) constexpr boolean(T b) noexcept
    : _val(b) {}

  template<std::constructible_from<bool> T>
  explicit(!std::same_as<bool, T>) constexpr operator T() const noexcept
  {
    return T(_val);
  }
  constexpr boolean operator!() const noexcept
  {
    return !_val;
  }

  constexpr boolean operator==(bool other) const noexcept
  {
    return bool(*this) == other;
  }
  constexpr boolean operator!=(bool other) const noexcept
  {
    return bool(*this) != other;
  }

  constexpr auto operator<=>(bool other) const noexcept
  {
    return bool(*this) <=> other;
  }
};
