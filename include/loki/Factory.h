////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// Copyright (c) 2005 by Peter Kuemmel
// Copyright (c) 2023 by Jia yuehua
// This code DOES NOT accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
// Code covered by the MIT License
// The authors make no representations about the suitability of this software
// for any purpose. It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
//#ifndef LOKI_FACTORYPARM_INC_
//#define LOKI_FACTORYPARM_INC_
#pragma once

// $Id: Factory.h 788 2006-11-24 22:30:54Z clitte_bbt $


#include "LokiTypeInfo.h"
#include <functional>
#include <boost/container/flat_map.hpp>
#include "SmallObj.h"
#include <utility>
#include <type_traits>
#include <map>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
//unreachable code if OnUnknownType throws an exception
#endif

/**
 * \defgroup	FactoriesGroup Factories
 * \defgroup	FactoryGroup Factory
 * \ingroup		FactoriesGroup
 * \brief		Implements a generic object factory.
 * 
 * <i>The Factory Method pattern is an object-oriented design pattern.
 * Like other creational patterns, it deals with the problem of creating objects
 * (products) without specifying the exact class of object that will be created.
 * Factory Method, one of the patterns from the Design Patterns book, handles
 * this problem by defining a separate method for creating the objects, which
 * subclasses can then override to specify the derived type of product that will
 * be created.
 * <br>
 * More generally, the term Factory Method is often used to refer to any method
 * whose main purpose is creation of objects.</i>
 * <div ALIGN="RIGHT"><a href="http://en.wikipedia.org/wiki/Factory_method_pattern">
 * Wikipedia</a></div>
 * 
 * Loki proposes a generic version of the Factory. Here is a typical use.<br>
 * <code><br>
 * 1. Factory< AbstractProduct, int > aFactory;<br>
 * 2. aFactory.Register( 1, createProductNull );<br>
 * 3. aFactory.CreateObject( 1 ); <br>
 * </code><br>
 * <br>
 * - 1. The declaration<br>
 * You want a Factory that produces AbstractProduct.<br>
 * The client will refer to a creation method through an int.<br>
 * - 2.The registration<br>
 * The code that will contribute to the Factory will now need to declare its
 * ProductCreator by registering them into the Factory.<br>
 * A ProductCreator is a just a function that will return the right object. ie <br>
 * <code>
 * Product* createProductNull()<br>             
 * {<br>
 *     return new Product<br>
 * }<br>
 * </code><br>
 * - 3. The use<br>
 * Now the client can create object by calling the Factory's CreateObject method
 * with the right identifier. If the ProductCreator were to have arguments
 * (<i>ie :Product* createProductParm( int a, int b )</i>)
 */

namespace Loki {

/**
 * \defgroup	FactoryErrorPoliciesGroup Factory Error Policies
 * \ingroup		FactoryGroup
 * \brief		Manages the "Unknown Type" error in an object factory
 * 
 * \class DefaultFactoryError
 * \ingroup		FactoryErrorPoliciesGroup
 * \brief		Default policy that throws an exception		
 * 
 */

template<typename IdentifierType, class AbstractProduct>
struct DefaultFactoryError
{
  struct Exception : public std::exception
  {
    const char *what() const throw() { return "Unknown Type"; }
  };

  static AbstractProduct *OnUnknownType(const IdentifierType &)
  {
    throw Exception();
  }
};


#define LOKI_ENABLE_NEW_FACTORY_CODE
#ifdef LOKI_ENABLE_NEW_FACTORY_CODE


////////////////////////////////////////////////////////////////////////////////
// class template FunctorImpl
////////////////////////////////////////////////////////////////////////////////

#endif//LOKI_DISABLE_TYPELIST_MACROS


////////////////////////////////////////////////////////////////////////////////
///  \class Factory
///
///  \ingroup FactoryGroup
///  Implements a generic object factory.
///
///  Create functions can have up to 15 parameters.
///
///  \par Singleton lifetime when used with Loki::SingletonHolder
///  Because Factory uses internally Functors which inherits from
///  SmallObject you must use the singleton lifetime
///  \code Loki::LongevityLifetime::DieAsSmallObjectChild \endcode
///  Alternatively you could suppress for Functor the inheritance
///  from SmallObject by defining the macro:
/// \code LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT \endcode
////////////////////////////////////////////////////////////////////////////////
template<
  typename IdentifierType,
  template<typename, class>
  class FactoryErrorPolicy,
  class AbstractProduct,
  class... Param>
class Factory : public FactoryErrorPolicy<IdentifierType, AbstractProduct>
{
  typedef std::function<AbstractProduct *(Param...)> ProductCreator;

  typedef std::map<IdentifierType, ProductCreator> IdToProductMap;

  IdToProductMap associations_;

public:
  template<class ID, class... Arg>
  std::enable_if_t<std::conjunction_v<std::is_convertible<ID,IdentifierType>,
  std::is_constructible<ProductCreator,Arg&&...>>, bool> 
  Register( ID &&id, Arg&&... arg)
  {
    return associations_.try_emplace(
                          std::forward<IdentifierType>(id), std::forward<Arg>(arg)...).second ;
  }


  template<class ID>
  std::enable_if_t<std::is_convertible_v<ID,IdentifierType>,bool> 
    Unregister(ID &&id)
  {
    return associations_.erase(id) != 0;
  }

  template<class ID, class... Arg>
  std::enable_if_t<sizeof...(Arg)==sizeof...(Param)&&std::conjunction_v<std::is_convertible<ID,IdentifierType>,
  std::is_convertible< Arg&&,Param>...>, AbstractProduct> *CreateObject(ID &&id, Arg &&...arg)const
  {
    typename IdToProductMap::const_iterator i = associations_.find(id);
    if (i != associations_.end()) {
      return (i->second)(std::forward<Arg>(arg)...);
    }
    return this->OnUnknownType(std::forward<ID>(id));
  }
};


/**
 *   \defgroup	CloneFactoryGroup Clone Factory
 *   \ingroup	FactoriesGroup
 *   \brief		Creates a copy from a polymorphic object.
 *
 *   \class		CloneFactory
 *   \ingroup	CloneFactoryGroup
 *   \brief		Creates a copy from a polymorphic object.
 */

template<
  class AbstractProduct,
  class ProductCreator =
    AbstractProduct *(*)(const AbstractProduct *),
  template<typename, class>
  class FactoryErrorPolicy = DefaultFactoryError>
class CloneFactory
  : public FactoryErrorPolicy<TypeInfo, AbstractProduct>
{
public:
  template<class T>
  bool Register(ProductCreator creator)
  {
    return associations_.insert(
                          typename IdToProductMap::value_type(typeid(T), creator))
             .second
           != 0;
  }

  template<class T>
  bool Unregister()
  {
    return associations_.erase(typeid(T)) != 0;
  }

  AbstractProduct *CreateObject(const AbstractProduct *model)
  {
    if (model == NULL) {
      return NULL;
    }

    typename IdToProductMap::iterator i =
      associations_.find(typeid(*model));

    if (i != associations_.end()) {
      return (i->second)(model);
    }
    return this->OnUnknownType(typeid(*model));
  }

private:
  typedef std::map<TypeInfo, ProductCreator> IdToProductMap;
  IdToProductMap associations_;
};

}// namespace Loki


#ifdef _MSC_VER
#pragma warning(pop)
#endif
