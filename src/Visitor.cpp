#include <cassert>
#include <utility>
#include <vector>
#include <iostream>
#include <boost/mp11.hpp>
#include "loki/Visitor.h"
#include "loki/DataGenerators.h"

//class Base : public Loki::BaseVisitable<>
//{
//public:
//  LOKI_DEFINE_VISITABLE()
//};
//
//class Type1 : public Base
//{
//public:
//  LOKI_DEFINE_VISITABLE()
//};
//
//class VariableVisitor : public Loki::Visitor<void, false, Base, Type1>
//{
//public:
//  ~VariableVisitor() {}
//  void Visit(Base &) { std::cout << "void Visit(Base&)\n"; }
//  void Visit(Type1 &) { std::cout << "void Visit(Type1&)\n"; }
//};
class BaseB;
class TypeB;

using CyclicV = Loki::CyclicVisitor<void, false, BaseB, TypeB>;
class MyVisitor : public CyclicV
{
public:
  ~MyVisitor() {}
  void Visit(BaseB &) { std::cout << "void Visit(BaseB&)\n"; }
  void Visit(TypeB &) { std::cout << "void Visit(TypeB&)\n"; }
};
class BaseB
{
public:
  LOKI_DEFINE_CYCLIC_VISITABLE(CyclicV);
};

class TypeB : public BaseB
{
public:
  LOKI_DEFINE_CYCLIC_VISITABLE(CyclicV);
};
void CyclicVisitorTest()
{
  MyVisitor visitor;
  TypeB type1;
  BaseB b;
  BaseB *dyn = &type1;
  dyn->Accept(visitor);
  dyn = &b;
  dyn->Accept(visitor);
}
//template<template<class> class F, class... T>
//constexpr auto transform()
//{
//  std::array a = { F<T>{}()... };
//  return a;
//}


int main()
{
  constexpr auto a = Loki::transform<Loki::sizeof_type, int, double>();
  for (auto i : a) {
    std::cout << i << std::endl;
  }

  auto b = Loki::transform<Loki::nameof_type, int, double>();
  for (auto i : b) {
    std::cout << i << std::endl;
  }
  //{
  //VariableVisitor visitor;
  //Type1 type1;
  //Base b;
  //Base *dyn = &type1;
  //dyn->Accept(visitor);
  //dyn = &b;
  //dyn->Accept(visitor);
  //}
}
