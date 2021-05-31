////////////////////////////////////////////////////////////////////////////////
// MOJO: MOving Joint Objects
// Copyright (c) 2002 by Andrei Alexandrescu
//
// Created by Andrei Alexandrescu
//
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this software
//     for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

#ifndef MOJO_H_
#define MOJO_H_

#include <cassert>
#include <iterator>
#include <type_traits>

namespace mojo {
template<class T>
class constant// type sugar for constants
{
  const T *data_;

public:
  explicit constant(const T &obj) : data_(&obj)
  {}
  const T &get() const
  {
    return *data_;
  }
};

template<class T>
class Temporary : private constant<T>// type sugar for temporaries
{
public:
  explicit Temporary(T &obj) : constant<T>(obj)
  {
  }

  T &get() const
  {
    return const_cast<T &>(constant<T>::get());
  }
};

template<class T>
class Fnresult : public T
{
public:
  // The cast below is valid given that nobody ever really creates a
  // const fnresult object
  Fnresult(const Fnresult &rhs)
    : T(Temporary<T>(const_cast<Fnresult &>(rhs)))
  {
  }

  explicit Fnresult(T &rhs) : T(Temporary<T>(rhs))
  {
  }
};

template<class T>
struct Enabled
{
  operator Temporary<T>()
  {
    return Temporary<T>(static_cast<T &>(*this));
  }
  operator constant<T>() const
  {
    return constant<T>(static_cast<const T &>(*this));
  }
  operator Fnresult<T>()
  {
    return Fnresult<T>(static_cast<T &>(*this));
  }

protected:
  Enabled() {}// intended to be derived from
  ~Enabled() {}// intended to be derived from
};

template<class T>
struct traits
{
  enum { enabled = std::is_base_of_v<Enabled<T>, T> && !std::is_same_v<Enabled<T>, T> };
  typedef typename std::conditional<
    enabled,
    Temporary<T>,
    T &>::type
    temporary;

  typedef typename std::conditional<
    enabled,
    Fnresult<T>,
    T>::type
    fnresult;
};

template<class T>
inline typename traits<T>::temporary as_temporary(T &src)
{
  typedef typename traits<T>::temporary temp;
  return temp(src);
}

template<class Iter1, class Iter2>
Iter2 move(Iter1 begin, Iter1 end, Iter2 dest)
{
  for (; begin != end; ++begin, ++dest) {
    *dest = as_temporary(*begin);
  }
  return dest;
}

template<class Iter1, class Iter2>
Iter2 uninitialized_move(Iter1 begin, Iter1 end, Iter2 dest)
{
  typedef typename std::iterator_traits<Iter2>::value_type T;

  for (; begin != end; ++begin, ++dest) {
    new (*dest) T(as_temporary(*begin));
  }
  return dest;
}
}// namespace mojo

#endif// MOJO_H_