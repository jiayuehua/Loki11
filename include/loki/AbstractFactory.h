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
  class Unit,
  class... U>
class AbstractFactoryEx : public GenScatterHierarchy<Unit, U...>
{
public:
  typedef mp::mp_list<U...> ProductList;

  template<class T, class = std::enable_if_t< mp::mp_contains<ProductList,T>::value> >
  T *Create()
  {
    Unit<T> &unit = *this;
    return unit.DoCreate(hana::type_c<T>);
  }
};

template<class... TList>
using AbstractFactory = AbstractFactoryEx<AbstractFactoryUnit, TList...>;

////////////////////////////////////////////////////////////////////////////////
// class template OpNewFactoryUnit
// Creates an object by invoking the new operator
////////////////////////////////////////////////////////////////////////////////

template<class ConcreteProduct, class Base>
class OpNewFactoryUnit:public Base
{

protected:

public:
  typedef typename Base::ProductList BaseProductList;
  typedef mp::mp_drop_c < BaseProductList,1> ProductList;
  typedef mp::mp_front<BaseProductList> AbstractProduct;
  ConcreteProduct *DoCreate(hana::type<AbstractProduct>)
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
// Implements an AbstractFacerface
////////////////////////////////////////////////////////////////////////////////


template<
  class AbstractFact,
  template<class, class> class Creator = OpNewFactoryUnit,
  class TList = typename AbstractFact::ProductList>
class ConcreteFactoryEx
  : public GenLinearHierarchy<
      mp::mp_reverse<TList>,
      Creator,
      AbstractFact>
{
public:
  typedef typename AbstractFact::ProductList ProductList;
  typedef TList ConcreteProductList;
};

template<
  class AbstractFact,
  class TList = typename AbstractFact::ProductList>
using ConcreteFactory= ConcreteFactoryEx<AbstractFact, OpNewFactoryUnit, TList>;
}// namespace Loki


#endif// end file guardian
