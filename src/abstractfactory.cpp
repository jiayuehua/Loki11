#include <iostream>
#include <vector>
#include <boost/mp11.hpp>
#include <type_traits>
#include "loki/AbstractFactory.h"
#include <loki/yasli/yasli_vector.h>
#include <loki/Register.h>
//#include <loki_ext/mojo2.h>
#include <loki_ext/yasli_find.h>

namespace mp = boost::mp11;

int *Create()
{
  return new int{ 42 };
}

//struct B
//{
//  virtual int f() = 0;
//};
//struct C
//{
//  int f(){};
//};
//struct D : public B
//  , C
//{
//  using C::f;
//};
template<class T, class B>
class Handler : B
{
public:
  virtual void Event(T &obj) const {}
};
int main()
{
  Loki::
    GenLinearHierarchy<mp::mp_list<int>, Handler>
      a;
  int b[] = { 1 };
  //  yasli::find(std::begin(b), std::end(b), 2);

  //  typedef Loki::AbstractFactory<Loki::AbstractFactoryUnit, int> Af;
  //Af *paf;

  //Loki::MyConcreteFactory<Af> c;
  //paf = &c;
  //int *p = paf->Create<int>();
}