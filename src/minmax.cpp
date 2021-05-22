#define DEBUG
#include "loki_ext/MinMax.h"
#include <iostream>
#include <vector>
#include <boost/mp11.hpp>
#include <type_traits>
namespace mp = boost::mp11;
namespace Loki {
namespace Private {
  //template<class L, class R>
  //struct CmpStructSize
  //{
  //  static const bool value = sizeof(L) < sizeof(R);
  //};
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
  //using ArithTypes2 = mp::mp_sort<ArithTypesImpl, mp::mp_not_fn<std::is_base_of>::fn>;
}// namespace Private
}// namespace Loki

template<class T>
struct CreatePolicy
{
  //void create()
  //{
  //}
};
struct B
{
};

template<template<class> class CP>
class A : CP<A<CP>>
{
public:
  void f()
  {
    CP<A>::create();
    //obj.create();
  }
};
template<class A>
struct S
{
  std::vector<A> v;
  int maxsize() const
  {
    return v.maxsize();
  }
  int max_size() const
  {
    return v.max_size();
  }
};

int main()
{
  std::cout << Loki::Min(3, 4) << std::endl;
  int a = 3;
  int b = 4;
  Loki::Min(a, b);
  A<CreatePolicy> n;
  S<int> v;
  // v.maxsize();
  //n.f();
  //  n.f();
}