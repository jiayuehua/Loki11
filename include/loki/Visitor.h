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
#ifndef LOKI_VISITOR_INC_
#define LOKI_VISITOR_INC_

// $Id: Visitor.h 751 2006-10-17 19:50:37Z syntheticpp $


///  \defgroup VisitorGroup Visitor

#include "HierarchyGenerators.h"
#include <type_traits>

namespace Loki {

////////////////////////////////////////////////////////////////////////////////
/// \class BaseVisitor
///
/// \ingroup VisitorGroup
/// The base class of any Acyclic Visitor
////////////////////////////////////////////////////////////////////////////////

class BaseVisitor
{
public:
  virtual ~BaseVisitor() = default;
};

////////////////////////////////////////////////////////////////////////////////
/// \class Visitor
///
/// \ingroup VisitorGroup
/// The building block of Acyclic Visitor
///
/// \par Usage
///
/// Defining the visitable class:
///
/// \code
/// class RasterBitmap : public BaseVisitable<>
/// {
/// public:
///     LOKI_DEFINE_VISITABLE()
/// };
/// \endcode
///
/// Way 1 to define a visitor:
/// \code
/// class SomeVisitor :
///     public BaseVisitor // required
///     public Visitor<RasterBitmap>,
///     public Visitor<Paragraph>
/// {
/// public:
///     void Visit(RasterBitmap&); // visit a RasterBitmap
///     void Visit(Paragraph &);   // visit a Paragraph
/// };
/// \endcode
///
/// Way 2 to define the visitor:
/// \code
/// class SomeVisitor :
///     public BaseVisitor // required
///     public Visitor<LOKI_TYPELIST_2(RasterBitmap, Paragraph)>
/// {
/// public:
///     void Visit(RasterBitmap&); // visit a RasterBitmap
///     void Visit(Paragraph &);   // visit a Paragraph
/// };
/// \endcode
///
/// Way 3 to define the visitor:
/// \code
/// class SomeVisitor :
///     public BaseVisitor // required
///     public Visitor<Seq<RasterBitmap, Paragraph>::Type>
/// {
/// public:
///     void Visit(RasterBitmap&); // visit a RasterBitmap
///     void Visit(Paragraph &);   // visit a Paragraph
/// };
/// \endcode
///
/// \par Using const visit functions:
///
/// Defining the visitable class (true for const):
///
/// \code
/// class RasterBitmap : public BaseVisitable<void, DefaultCatchAll, true>
/// {
/// public:
///     LOKI_DEFINE_CONST_VISITABLE()
/// };
/// \endcode
///
/// Defining the visitor which only calls const member functions:
/// \code
/// class SomeVisitor :
///     public BaseVisitor // required
///     public Visitor<RasterBitmap, void, true>,
/// {
/// public:
///     void Visit(const RasterBitmap&); // visit a RasterBitmap by a const member function
/// };
/// \endcode
///
/// \par Example:
///
/// test/Visitor/main.cpp
////////////////////////////////////////////////////////////////////////////////

template<class T, typename R = void, bool ConstVisit = false>
struct VisitorProductBase
{
  typedef R ReturnType;
  typedef std::conditional_t<ConstVisit, const T, T> ParamType;
  virtual ~VisitorProductBase() {}
  virtual ReturnType Visit(ParamType &) = 0;
};

template<typename R, bool constparam, class... Head>
class Visitor
  : public BaseVisitor
  , public VisitorProductBase<Head, R, constparam>...
{
public:
  typedef R ReturnType;
};

//template <class Head, typename R,bool constparam>
//class Visitor<Typelist<Head, NullType>, R, bool constparam> : public Visitor<Head, R, constparam>
//{
//public:
//    typedef R ReturnType;
//    using Visitor<Head, R, constparam>::Visit;
//};


////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitorImpl
// Implements non-strict visitation (you can implement only part of the Visit
//     functions)
////////////////////////////////////////////////////////////////////////////////

//template <class TList, typename R = void> class BaseVisitorImpl;

template<class R>
struct ProductVisitorUnit
{
  template<class T, class Base>
  struct ProductVisitorImpl : public Base
  {
    virtual R Visit(T &)
    {
      return R{};
    }
  };
};
template<class VisitorRoot, class R, class... T>
using BaseVisitorImpl = GenLinearHierarchy<mp::mp_list<T...>, ProductVisitorUnit<R>::template ProductVisitorImpl, VisitorRoot>;


//template<typename R, class... Head>
//class BaseVisitorImpl
//  : public Visitor<Head, R>
//  , public BaseVisitorImpl<Tail, R>
//{
//public:
//
//  virtual R Visit(Head &)
//  {
//    return R();
//  }
//};
//
//template<class Head, typename R>
//class BaseVisitorImpl<Typelist<Head, NullType>, R>
//  : public Visitor<Head, R>
//{
//public:
//  virtual R Visit(Head &)
//  {
//    return R();
//  }
//};

////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitable
////////////////////////////////////////////////////////////////////////////////

template<typename R, typename Visited>
struct DefaultCatchAll
{
  static R OnUnknownVisitor(Visited &, BaseVisitor &)
  {
    return R();
  }
  static R OnUnknownVisitor(const Visited &, BaseVisitor &)
  {
    return R();
  }
};

////////////////////////////////////////////////////////////////////////////////
// class template BaseVisitable
////////////////////////////////////////////////////////////////////////////////

template<
  typename R = void,
  bool ConstVisitable = false,
  template<typename, class> class CatchAll = DefaultCatchAll>
class BaseVisitable;

template<typename R, template<typename, class> class CatchAll>
class BaseVisitable<R, false, CatchAll>
{
public:
  typedef R ReturnType;
  virtual ~BaseVisitable() {}
  virtual ReturnType Accept(BaseVisitor &) = 0;

protected:// give access only to the hierarchy
  template<class T>
  static ReturnType AcceptImpl(T &visited, BaseVisitor &guest)
  {
    // Apply the Acyclic Visitor
    if (VisitorProductBase<std::remove_cvref_t<T>, R> *p = dynamic_cast<VisitorProductBase<T, R> *>(&guest)) {
      return p->Visit(visited);
    }
    return CatchAll<R, T>::OnUnknownVisitor(visited, guest);
  }
};

template<typename R, template<typename, class> class CatchAll>
class BaseVisitable<R, true, CatchAll>
{
public:
  typedef R ReturnType;
  virtual ~BaseVisitable() {}
  virtual ReturnType Accept(BaseVisitor &) const = 0;

protected:// give access only to the hierarchy
  template<class T>
  static ReturnType AcceptImpl(const T &visited, BaseVisitor &guest)
  {
    // Apply the Acyclic Visitor
    if (VisitorProductBase<T, R, true> *p = dynamic_cast<VisitorProductBase<T, R, true> *>(&guest)) {
      return p->Visit(visited);
    }
    return CatchAll<R, T>::OnUnknownVisitor(visited, guest);
  }
};


////////////////////////////////////////////////////////////////////////////////
/// \def LOKI_DEFINE_VISITABLE()
/// \ingroup VisitorGroup
/// Put it in every class that you want to make visitable
/// (in addition to deriving it from BaseVisitable<R>)
////////////////////////////////////////////////////////////////////////////////

#define LOKI_DEFINE_VISITABLE()                         \
  virtual ReturnType Accept(::Loki::BaseVisitor &guest) \
  {                                                     \
    return AcceptImpl(*this, guest);                    \
  }

////////////////////////////////////////////////////////////////////////////////
/// \def LOKI_DEFINE_CONST_VISITABLE()
/// \ingroup VisitorGroup
/// Put it in every class that you want to make visitable by const member
/// functions (in addition to deriving it from BaseVisitable<R>)
////////////////////////////////////////////////////////////////////////////////

#define LOKI_DEFINE_CONST_VISITABLE()                         \
  virtual ReturnType Accept(::Loki::BaseVisitor &guest) const \
  {                                                           \
    return AcceptImpl(*this, guest);                          \
  }

////////////////////////////////////////////////////////////////////////////////
/// \class CyclicVisitor
///
/// \ingroup VisitorGroup
/// Put it in every class that you want to make visitable (in addition to
/// deriving it from BaseVisitable<R>
////////////////////////////////////////////////////////////////////////////////

template<typename R, bool ConstParam, class... TList>
class CyclicVisitor : public BaseVisitor
  , public Visitor<R, ConstParam, TList...>
{
public:
  typedef R ReturnType;
  // using Visitor<TList, R>::Visit;

  template<class Visited>
  ReturnType GenericVisit(Visited &&host)
  {
    VisitorProductBase<std::remove_cvref_t<Visited>, ReturnType> &subObj = *this;
    return subObj.Visit(std::forward<Visited>(host));
  }
};

////////////////////////////////////////////////////////////////////////////////
/// \def LOKI_DEFINE_CYCLIC_VISITABLE(SomeVisitor)
/// \ingroup VisitorGroup
/// Put it in every class that you want to make visitable by a cyclic visitor
////////////////////////////////////////////////////////////////////////////////

#define LOKI_DEFINE_CYCLIC_VISITABLE(SomeVisitor)            \
  virtual SomeVisitor::ReturnType Accept(SomeVisitor &guest) \
  {                                                          \
    return guest.GenericVisit(*this);                        \
  }

#define LOKI_DEFINE_CYCLIC_CONST_VISITABLE(SomeVisitor)            \
  virtual SomeVisitor::ReturnType Accept(const SomeVisitor &guest) \
  {                                                                \
    return guest.GenericVisit(*this);                              \
  }
}// namespace Loki


#endif// end file guardian
