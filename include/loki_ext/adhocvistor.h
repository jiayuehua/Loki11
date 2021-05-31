#ifndef ADHOCVISITOR_H
#define ADHOCVISITOR_H
#include <boost/mp11.hpp>
namespace mp = boost::mp11;
namespace Loki {

//
//class Effector
//{
//    void Visit(Horse*);
//    void Visit(Dog*);
//};
//
template<class T>
class MyAdHocVisitorBase
{
public:
  virtual void Visit(T *) = 0;
};

template<class... T>
class MyAdHocVisitor : public MyAdHocVisitorBase<T>...
{
public:
  using MyAdHocVisitorBase<T>::Visit...;

  template<class H, class SomeClass>
  bool StartVisitImpl(SomeClass *p)
  {
    if (H *pFound = dynamic_cast<H *>(p)) {
      Visit(pFound);
      return true;
    } else {
      return false;
    }
  }
  template<class SomeClass>
  bool StartVisit(SomeClass *p)
  {
    return (StartVisitImpl<T>(p) || ...);
  }
};
template<class Effector, class m_list>
class MyFlexAdHocVisitor_Impl;
template<class Effector, class... H>
class MyFlexAdHocVisitor_Impl<Effector, mp::mp_list<H...>> : private Effector
{
  template<class Head, class SomeClass>
  bool StartVisitImpl(SomeClass *p)
  {
    if (Head *pFound = dynamic_cast<Head *>(p)) {
      typedef void (Effector::*PVistor)(Head *);
      [[maybe_unused]] PVistor pvistor = &Effector::Visit;//check
      Effector::Visit(pFound);
      return true;
    } else {
      return false;
    }
  }

public:
  template<class SomeClass>
  bool StartVisit(SomeClass *p)
  {
    return (StartVisitImpl<H>(p) || ...);
  }
};

template<class Effector, class... H>
class FlexAdHocVisitor : public MyFlexAdHocVisitor_Impl<Effector, mp::mp_sort<mp::mp_list<H...>, mp::mp_not_fn<std::is_base_of>::fn>>
{
};

template<template<class> class Effector, class m_list>
class MyFlexAdHocVisitorEx_Impl;
template<template<class> class Effector, class... H>
class MyFlexAdHocVisitorEx_Impl<Effector, mp::mp_list<H...>> : private Effector<H>...
{
  template<class Head, class SomeClass>
  bool StartVisitImpl(SomeClass *p)
  {
    if (Head *pFound = dynamic_cast<Head *>(p)) {
      typedef void (Effector<Head>::*PVistor)(Head *);
      [[maybe_unused]] PVistor pvistor = &Effector<Head>::Visit;//check
      Effector<Head>::Visit(pFound);
      return true;
    } else {
      return false;
    }
  }

public:
  template<class SomeClass>
  bool StartVisit(SomeClass *p)
  {
    return (StartVisitImpl<H>(p) || ...);
  }
};

template<template<class> class Effector, class... H>
class FlexAdHocVisitorEx : public MyFlexAdHocVisitorEx_Impl<Effector, mp::mp_sort<mp::mp_list<H...>, mp::mp_not_fn<std::is_base_of>::fn>>
{
};

}// namespace Loki

#endif
