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
// The author or Addison-Wesley Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_ABSTRACTFACTORY_INC_
#define LOKI_ABSTRACTFACTORY_INC_

// $Id: AbstractFactory.h 771 2006-10-27 18:05:03Z clitte_bbt $


#include "HierarchyGenerators.h"
#include <boost/mp11.hpp>
#include <boost/hana/type.hpp>

#include <cassert>

namespace mp = boost::mp11;
namespace hana = boost::hana;

/**
 * \defgroup	FactoriesGroup Factories
 * \defgroup	AbstractFactoryGroup Abstract Factory
 * \ingroup		FactoriesGroup
 * \brief		Implements an abstract object factory.
 */

/**
 * \class		AbstractFactory
 * \ingroup		AbstractFactoryGroup
 * \brief		Implements an abstract object factory.
 */

namespace Loki {

////////////////////////////////////////////////////////////////////////////////
// class template AbstractFactoryUnit
// The building block of an Abstract Factory
////////////////////////////////////////////////////////////////////////////////

template<class T>
class AbstractFactoryUnit
{
public:
  virtual T *DoCreate(hana::type<T>) = 0;
  virtual ~AbstractFactoryUnit() {}
};

////////////////////////////////////////////////////////////////////////////////
// class template AbstractFactory
// Defines an Abstract Factory interface starting from a typelist
////////////////////////////////////////////////////////////////////////////////

template<
  template<class>
  class Unit =
    AbstractFactoryUnit,
  class... TList>
class AbstractFactory : public GenScatterHierarchy<Unit, TList...>
{
public:
  typedef mp::mp_list<TList...> ProductList;

  template<class T>
  T *Create()
  {
    Unit<T> &unit = *this;
    return unit.DoCreate(hana::type_c<T>);
  }
};

////////////////////////////////////////////////////////////////////////////////
// class template OpNewFactoryUnit
// Creates an object by invoking the new operator
////////////////////////////////////////////////////////////////////////////////

template<class ConcreteProduct, class Base>
class OpNewFactoryUnit
{
  typedef typename Base::ProductList BaseProductList;

protected:
  typedef typename BaseProductList::Tail ProductList;

public:
  typedef typename BaseProductList::Head AbstractProduct;
  ConcreteProduct *DoCreate(hana::type<Base>)
  {
    return new ConcreteProduct;
  }
};

////////////////////////////////////////////////////////////////////////////////
// class template PrototypeFactoryUnit
// Creates an object by cloning a prototype
// There is a difference between the implementation herein and the one described
//     in the book: GetPrototype and SetPrototype use the helper friend
//     functions DoGetPrototype and DoSetPrototype. The friend functions avoid
//     name hiding issues. Plus, GetPrototype takes a reference to pointer
//     instead of returning the pointer by value.
////////////////////////////////////////////////////////////////////////////////

template<class ConcreteProduct, class Base>
class PrototypeFactoryUnit : public Base
{
  typedef typename Base::ProductList BaseProductList;

protected:
  typedef typename BaseProductList::Tail ProductList;

public:
  typedef typename BaseProductList::Head AbstractProduct;

  PrototypeFactoryUnit(AbstractProduct *p = 0)
    : pPrototype_(p)
  {}

  template<class CP, class Base1>
  friend void DoGetPrototype(const PrototypeFactoryUnit<CP, Base1> &me,
    typename Base1::ProductList::Head *&pPrototype);

  template<class CP, class Base1>
  friend void DoSetPrototype(PrototypeFactoryUnit<CP, Base1> &me,
    typename Base1::ProductList::Head *pObj);

  template<class U>
  void GetPrototype(U *&p)
  {
    return DoGetPrototype(*this, p);
  }

  template<class U>
  void SetPrototype(U *pObj)
  {
    DoSetPrototype(*this, pObj);
  }

  AbstractProduct *DoCreate(hana::type<AbstractProduct>)
  {
    assert(pPrototype_);
    return pPrototype_->Clone();
  }

private:
  AbstractProduct *pPrototype_;
};

template<class CP, class Base>
inline void DoGetPrototype(const PrototypeFactoryUnit<CP, Base> &me,
  typename Base::ProductList::Head *&pPrototype)
{
  pPrototype = me.pPrototype_;
}

template<class CP, class Base>
inline void DoSetPrototype(PrototypeFactoryUnit<CP, Base> &me,
  typename Base::ProductList::Head *pObj)
{
  me.pPrototype_ = pObj;
}

////////////////////////////////////////////////////////////////////////////////
// class template ConcreteFactory
// Implements an AbstractFactory interface
////////////////////////////////////////////////////////////////////////////////

//template<class T, class IS, class A, template<class, class> class Creator>
//struct TupleImpl;
//
//template<class B, class D, std::size_t... i, template<class, class> class Creator>
//struct TupleImpl<B, D, std::index_sequence<i...>, Creator> : public Creator<mp::mp_at_c<B, i>, mp::mp_at_c<D, i>>...
//{
//};
//template<
//  class AbstractFactory,
//  template<class, class> class Creator = OpNewFactoryUnit,
//  class TList = typename AbstractFactory::ProductList>
//class MyConcreteFactory : public AbstractFactory
//  , TupleImpl<typename AbstractFactory::ProductList, TList, std::make_index_sequence<mp::mp_size<TList>::value>, Creator>
//
//{
//public:
//  typedef typename AbstractFactory::ProductList ProductList;
//  typedef TList ConcreteProductList;
//};
//
//template<
//  class AbstractFact,
//  template<class, class> class Creator = OpNewFactoryUnit,
//  class TList = typename AbstractFact::ProductList>
//class ConcreteFactory
//  : public GenLinearHierarchy<
//      typename TL::Reverse<TList>::Result,
//      Creator,
//      AbstractFact>
//{
//public:
//  typedef typename AbstractFact::ProductList ProductList;
//  typedef TList ConcreteProductList;
//};

}// namespace Loki


#endif// end file guardian
