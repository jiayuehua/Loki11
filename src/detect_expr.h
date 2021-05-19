#pragma once
//#include "https://raw.githubusercontent.com/PeterSommerlad/PSsimplesafeint/main/include/psssafeint.h"""
#include <iostream>
namespace compile_checks {
//using namespace psssint;

template<auto value>
using consume_value = void;


#define concat_line_impl(A, B) A##_##B
#define concat_line(A, B) concat_line_impl(A, B)

#define check_does_compile(NOT, FROM, oper)                                     \
  namespace concat_line(NOT##_test, __LINE__)                                   \
  {                                                                             \
    template<typename T, typename = void>                                       \
    constexpr bool                                                              \
      expression_compiles = false;                                              \
    template<typename T>                                                        \
    constexpr bool                                                              \
      expression_compiles<T, consume_value<(T {} oper T{})>> = true;            \
    static_assert(NOT expression_compiles<FROM>, "should not compile: " #oper); \
  }// namespace tag

check_does_compile(not, int, /)
  check_does_compile(, int, +)
}// namespace compile_checks
#undef check_does_compile
#undef concat_line_impl
#undef concat_line
