#include <cstdio>
#include <string>
template<class... T>
void param()
{
  std::puts(__PRETTY_FUNCTION__);
}
template<template<class... T> class... F>
void param()
{
  std::puts(__PRETTY_FUNCTION__);
}
template<template<template<class... T> class...> class... F>
void param()
{
  std::puts(__PRETTY_FUNCTION__);
}
template<template<template<auto... T> class...> class... F>
void param()
{
  std::puts(__PRETTY_FUNCTION__);
}
template<template<auto... T> class... F>
void param()
{
  std::puts(__PRETTY_FUNCTION__);
}
template<auto... F>
void param()
{
  std::puts(__PRETTY_FUNCTION__);
}

#define EXPLORE(expr)                   \
  printf("decltype(" #expr ") is... "); \
  param<decltype(expr)>();

//template<class F>
//struct S{};
//template<class F,class U>
//struct DS{};
//template<template<class S> class G>
//struct GS{};
//template<template<int S> class G>
//struct QS{};
//
//template<int G>
//struct TS{};
//template<class T>
//void f(T&& t)
//{
//  EXPLORE(t);
//  param<T>();
//}

//int main()
//{
// // param<TS> ();
//  param<GS> ();
//  param<QS> ();
//  puts("\n");
//  param<int>();
//  param<int,std::string>();
//  int a=100;
//  EXPLORE(a);
//  param<S,S> ();
//  param<DS,DS,DS> ();
//  param<1,2,2.1> ();
//  f(1);
//  f(a);
//}