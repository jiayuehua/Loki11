////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Data Generator by Shannon Barber
// This code DOES NOT accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
// Code covered by the MIT License
// The author makes no representations about the suitability of this software
//  for any purpose. It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_DATAGENERATORS_INC_
#define LOKI_DATAGENERATORS_INC_

// $Id: DataGenerators.h 751 2006-10-17 19:50:37Z syntheticpp $


#include <array>

//Reference version

/************************************************************************************

Example Use
//void test()
//{
//  constexpr auto a = transform<Size, int, double>();
//  for (auto i : a) {
//    std::cout << i << std::endl;
//  }
//
//  auto b = transform<TypeidName, int, double>();
//  for (auto i : b) {
//    std::cout << i << std::endl;
//  }
//}

*******************************************************************************/
namespace Loki {
template<typename T>
struct nameof_type
{
  const char *operator()() const noexcept
  {
    return typeid(T).name();
  }
};
template<typename T>
struct sizeof_type
{
  constexpr auto operator()() const noexcept
  {
    return sizeof(T);
  }
};
template<template<class> class F, class... T>
constexpr auto transform() noexcept
{
  std::array a = { F<T>{}()... };
  return a;
}

}// namespace Loki

#endif// end file guardian
