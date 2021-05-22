////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: May 19, 2002

#ifndef MINMAX_INC_
#define MINMAX_INC_

#include <boost/mp11.hpp>
#include <type_traits>
namespace mp = boost::mp11;
namespace Loki {
namespace Private {
  template<class L, class R>
  struct CmpStructSize
  {
    static const bool value = sizeof(L) < sizeof(R);
  };
  typedef mp::mp_list<
    const bool,
    const char,
    const char8_t,
    const signed char,
    const unsigned char,
    const char16_t,
    const short int,
    const unsigned short int,
    const wchar_t,
    const char32_t,
    const int,
    const unsigned int,
    const float,
    const long int,
    const unsigned long int,
    const long long int,
    const unsigned long long int,
    const double,
    const long double>
    ArithTypesImpl;
  using ArithTypes = mp::mp_sort<ArithTypesImpl, CmpStructSize>;
}// namespace Private

template<class L, class R>
class MinMaxTraits
{
  typedef std::conditional_t<std::is_convertible_v<R, L>,
    L,
    R>
    T1;

  static constexpr int pos1 = mp::mp_find<Private::ArithTypes, const L>::value;
  static constexpr int pos2 = mp::mp_find<Private::ArithTypes, const R>::value;
  typedef std::conditional_t < pos1 != -1 && pos1<pos2, R, T1> T2;

  enum { rConst = std::is_const_v<R> >= std::is_const_v<L> };
  enum { l2r = rConst && std::is_convertible_v<typename std::remove_const_t<L> &, typename std::remove_const_t<R> &> };
  typedef std::conditional_t<l2r, R &, T2> T3;

  enum { lConst = std::is_const_v<L> >= std::is_const_v<R> };
  enum { r2l = lConst && std::is_convertible_v<std::remove_const_t<R> &, std::remove_const_t<L> &> };

public:
  typedef std::conditional_t<r2l, L &, T3> Result;
};

template<class L, class R>
typename MinMaxTraits<L, R>::Result
  Min(L &lhs, R &rhs)
{
  if (lhs < rhs) return lhs;
  return rhs;
}

template<class L, class R>
typename MinMaxTraits<const L, R>::Result
  Min(const L &lhs, R &rhs)
{
  if (lhs < rhs) return lhs;
  return rhs;
}

template<class L, class R>
typename MinMaxTraits<L, const R>::Result
  Min(L &lhs, const R &rhs)
{
  if (lhs < rhs) return lhs;
  return rhs;
}

template<class L, class R>
typename MinMaxTraits<const L, const R>::Result
  Min(const L &lhs, const R &rhs)
{
  if (lhs < rhs) return lhs;
  return rhs;
}

template<class L, class R>
typename MinMaxTraits<L, R>::Result
  Max(L &lhs, R &rhs)
{
  if (lhs > rhs) return lhs;
  return rhs;
}

template<class L, class R>
typename MinMaxTraits<const L, R>::Result
  Max(const L &lhs, R &rhs)
{
  if (lhs > rhs) return lhs;
  return rhs;
}

template<class L, class R>
typename MinMaxTraits<L, const R>::Result
  Max(L &lhs, const R &rhs)
{
  if (lhs > rhs) return lhs;
  return rhs;
}

template<class L, class R>
typename MinMaxTraits<const L, const R>::Result
  Max(const L &lhs, const R &rhs)
{
  if (lhs > rhs) return lhs;
  return rhs;
}
}// namespace Loki
#endif// MINMAX_INC_
