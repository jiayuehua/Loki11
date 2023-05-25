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
#ifndef LOKI_MULTIMETHODS_INC_
#define LOKI_MULTIMETHODS_INC_

// $Id: MultiMethods.h 751 2006-10-17 19:50:37Z syntheticpp $

#include <boost/callable_traits.hpp>
#include <boost/type_index.hpp>
#include <tuple>
#include <boost/container/flat_map.hpp>
#include <utility>
#include <functional>
#include <boost/mp11.hpp>
#include "priority_tag.h"
namespace ct = boost::callable_traits;
namespace mp = boost::mp11;
////////////////////////////////////////////////////////////////////////////////
// IMPORTANT NOTE:
// The double dispatchers implemented below differ from the excerpts shown in
// the book - they are simpler while respecting the same interface.
////////////////////////////////////////////////////////////////////////////////

namespace Loki {
  using TypeInfo= boost::typeindex::type_index;
////////////////////////////////////////////////////////////////////////////////
// class template InvocationTraits (helper)
// Helps implementing optional symmetry
////////////////////////////////////////////////////////////////////////////////

namespace Private {
  template<class SomeLhs, class SomeRhs, class Executor, typename ResultType>
  struct InvocationTraits
  {
    static ResultType
      DoDispatch(SomeLhs &lhs, SomeRhs &rhs, Executor &exec, priority_tag<0>)
    {
      return exec(lhs, rhs);
    }
    static ResultType
      DoDispatch(SomeLhs &lhs, SomeRhs &rhs, Executor &exec, priority_tag<true>)
    {
      return exec(rhs, lhs);
    }
  };
}// namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template StaticDispatcher
// Implements an automatic static double dispatcher based on two typelists
////////////////////////////////////////////////////////////////////////////////

template<
  class Executor,
  class OriginTypesLhs,
  class OriginTypesRhs = OriginTypesLhs,
  bool symmetric = false,
  typename ResultType = void>
class StaticDispatcher
{
  using TypeLhs = mp::mp_sort<OriginTypesLhs,
    mp::mp_not_fn<std::is_base_of>::fn>;
  using TypeRhs = mp::mp_sort<OriginTypesRhs,
    mp::mp_not_fn<std::is_base_of>::fn>;
  using BaseLhs = mp::mp_back<TypeLhs>;
  using BaseRhs = mp::mp_back<TypeRhs>;
  template<class T>
  using IsBaseofBaseLhs=std::is_base_of<BaseLhs,T>;
  template<class T>
  using IsBaseofBaseRhs=std::is_base_of<BaseRhs,T>;
  static_assert(mp::mp_all_of<TypeLhs, IsBaseofBaseLhs>::value);
  static_assert(mp::mp_all_of<TypeRhs, IsBaseofBaseRhs>::value);

  template<class SomeLhs>
  static ResultType DispatchRhs(SomeLhs &lhs, BaseRhs &rhs, Executor exec, mp::mp_list<>)
  {
    return exec.OnError(lhs, rhs);
  }

  template<class MpList, class SomeLhs>
  static ResultType DispatchRhs(SomeLhs &lhs, BaseRhs &rhs, Executor exec, MpList)
  {

    using Head = mp::mp_front<MpList>;
    using Tail = mp::mp_pop_front<MpList>;
    if (Head *p2 = dynamic_cast<Head *>(&rhs)) {
      priority_tag<(symmetric && int(mp::mp_find<TypeRhs, Head>::value) < int(mp::mp_find<TypeLhs, SomeLhs>::value))> i2t;

      typedef Private::InvocationTraits<
        SomeLhs,
        Head,
        Executor,
        ResultType>
        CallTraits;

      return CallTraits::DoDispatch(lhs, *p2, exec, i2t);
    }
    return DispatchRhs(lhs, rhs, exec, Tail());
  }

  static ResultType DispatchLhs(BaseLhs &lhs, BaseRhs &rhs, Executor exec, mp::mp_list<>)
  {
    return exec.OnError(lhs, rhs);
  }

  template<class MpList>
  static ResultType DispatchLhs(BaseLhs &lhs, BaseRhs &rhs, Executor exec, MpList)
  {
    using Head = mp::mp_front<MpList>;

    if (Head *p1 = dynamic_cast<Head *>(&lhs)) {
      return DispatchRhs(*p1, rhs, exec, TypeRhs());
    }
    return DispatchLhs(lhs, rhs, exec, mp::mp_pop_front<MpList>{});
  }

public:
  static ResultType invoke(Executor exec, BaseLhs &lhs, BaseRhs &rhs)
  {
    return DispatchLhs(lhs, rhs, exec, TypeLhs());
  }
};


////////////////////////////////////////////////////////////////////////////////
// class template BasicDispatcher
// Implements a logarithmic double dispatcher for functors (or functions)
// Doesn't offer automated casts or symmetry
////////////////////////////////////////////////////////////////////////////////

template<
  class BaseLhs,
  class BaseRhs = BaseLhs,
  typename ResultType = void,
  typename CallbackType = ResultType (*)(BaseLhs &, BaseRhs &)>
class BasicDispatcher
{
  typedef std::pair<TypeInfo, TypeInfo> KeyType;
  typedef CallbackType MappedType;
  typedef boost::container::flat_map<KeyType, MappedType> MapType;
  MapType callbackMap_;

  void DoAdd(TypeInfo lhs, TypeInfo rhs, CallbackType fun);
  bool DoRemove(TypeInfo lhs, TypeInfo rhs);

public:
  template<class SomeLhs, class SomeRhs>
  void Add(CallbackType fun)
  {
    DoAdd(boost::typeindex::type_id<SomeLhs>(), boost::typeindex::type_id<SomeRhs>(), fun);
  }

  template<class SomeLhs, class SomeRhs>
  bool Remove()
  {
    return DoRemove(boost::typeindex::type_id<SomeLhs>(), boost::typeindex::type_id<SomeRhs>());
  }

  ResultType Go(BaseLhs &lhs, BaseRhs &rhs);
};

// Non-inline to reduce compile time overhead...
template<class BaseLhs, class BaseRhs, typename ResultType, typename CallbackType>
void BasicDispatcher<BaseLhs, BaseRhs, ResultType, CallbackType>::DoAdd(TypeInfo lhs, TypeInfo rhs, CallbackType fun)
{
  callbackMap_[KeyType(lhs, rhs)] = fun;
}

template<class BaseLhs, class BaseRhs, typename ResultType, typename CallbackType>
bool BasicDispatcher<BaseLhs, BaseRhs, ResultType, CallbackType>::DoRemove(TypeInfo lhs, TypeInfo rhs)
{
  return callbackMap_.erase(KeyType(lhs, rhs)) == 1;
}

template<class BaseLhs, class BaseRhs, typename ResultType, typename CallbackType>
ResultType BasicDispatcher<BaseLhs, BaseRhs, ResultType, CallbackType>::Go(BaseLhs &lhs, BaseRhs &rhs)
{
  typename MapType::key_type k(boost::typeindex::type_id_runtime(lhs), boost::typeindex::type_id_runtime(rhs));
  typename MapType::iterator i = callbackMap_.find(k);
  if (i == callbackMap_.end()) {
    throw std::runtime_error("Function not found");
  }
  return (i->second)(lhs, rhs);
}

////////////////////////////////////////////////////////////////////////////////
// class template StaticCaster
// Implementation of the CastingPolicy used by FunctorDispatcher
////////////////////////////////////////////////////////////////////////////////

struct StaticCaster
{
  template<class To, class From>
  static To &Cast(From &obj)
  {
    return static_cast<To &>(obj);
  }
};

////////////////////////////////////////////////////////////////////////////////
// class template DynamicCaster
// Implementation of the CastingPolicy used by FunctorDispatcher
////////////////////////////////////////////////////////////////////////////////

struct DynamicCaster
{
  template<class To, class From>
  static To &Cast(From &obj)
  {
    return dynamic_cast<To &>(obj);
  }
};

////////////////////////////////////////////////////////////////////////////////
// class template Private::FnDispatcherHelper
// Implements trampolines and argument swapping used by FnDispatcher
////////////////////////////////////////////////////////////////////////////////

namespace Private {
  template<class BaseLhs, class BaseRhs, class CastLhs, class CastRhs, auto Callback>
  struct FnDispatcherHelper ;

  template<class BaseLhs, class BaseRhs, class CastLhs, class CastRhs, class SomeLhs, class SomeRhs, typename ResultType, ResultType (*Callback)(SomeLhs &, SomeRhs &)>
  struct FnDispatcherHelper<BaseLhs, BaseRhs, CastLhs, CastRhs, Callback>
  {
    typedef SomeLhs LHS;
    typedef SomeRhs RHS;
    static ResultType Trampoline(BaseLhs &lhs, BaseRhs &rhs)
    {
      return Callback(CastLhs::template Cast<SomeLhs>(lhs), CastRhs::template Cast<SomeRhs>(rhs));
    }
    static ResultType TrampolineR(BaseRhs &rhs, BaseLhs &lhs)
    {
      return Trampoline(lhs, rhs);
    }
  };
}// namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template FnDispatcher
// Implements an automatic logarithmic double dispatcher for functions
// Features automated conversions
////////////////////////////////////////////////////////////////////////////////

template<class BaseLhs, class BaseRhs = BaseLhs, typename ResultType = void, class CastingPolicy = DynamicCaster, template<class, class, class, class> class DispatcherBackend = BasicDispatcher>
class FnDispatcher
{
  DispatcherBackend<BaseLhs, BaseRhs, ResultType, ResultType (*)(BaseLhs &, BaseRhs &)> backEnd_;
  template<class SomeLhs, class SomeRhs>
  void AddBase(ResultType (*pFun)(BaseLhs &, BaseRhs &))
  {
    return backEnd_.template Add<SomeLhs, SomeRhs>(pFun);
  }

public:

  template<auto callback>
  void Add()
  {
    typedef Private::FnDispatcherHelper<
      BaseLhs,
      BaseRhs,
      CastingPolicy,
      CastingPolicy,
      callback>
      Local;

    AddBase<typename Local::LHS, typename Local::RHS>(&Local::Trampoline);
  }

  template<bool symmetric,auto callback>
  void Add(bool = true)// [gcc] dummy bool
  {
    typedef Private::FnDispatcherHelper<
      BaseLhs,
      BaseRhs,
      CastingPolicy,
      CastingPolicy,
      callback>
      Local;

    AddBase<typename Local::LHS, typename Local::RHS>(&Local::Trampoline);
    if (symmetric) {
      AddBase<typename Local::RHS, typename Local::LHS>(&Local::TrampolineR);
    }
  }

  template<class SomeLhs, class SomeRhs>
  void Remove()
  {
    backEnd_.template Remove<SomeLhs, SomeRhs>();
  }

  ResultType operator()(BaseLhs &lhs, BaseRhs &rhs)
  {
    return backEnd_.Go(lhs, rhs);
  }
};

////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcherAdaptor
// permits use of FunctorDispatcher under gcc.2.95.2/3
///////////////////////////////////////////////////////////////////////////////

namespace Private {
  template<class BaseLhs, class BaseRhs, class SomeLhs, class SomeRhs, typename ResultType, class CastLhs, class CastRhs, class Fun, bool SwapArgs>
  class FunctorDispatcherHelper
  {
    Fun fun_;
    ResultType Fire(BaseLhs &lhs, BaseRhs &rhs, priority_tag<0>)
    {
      return fun_(CastLhs::template Cast<SomeLhs>(lhs), CastRhs::template Cast<SomeRhs>(rhs));
    }
    ResultType Fire(BaseLhs &rhs, BaseRhs &lhs, priority_tag<1>)
    {
      return fun_(CastLhs::template Cast<SomeLhs>(lhs), CastRhs::template Cast<SomeRhs>(rhs));
    }

  public:
    FunctorDispatcherHelper(const Fun &fun) : fun_(fun) {}

    ResultType operator()(BaseLhs &lhs, BaseRhs &rhs)
    {
      return Fire(lhs, rhs, priority_tag<SwapArgs>());
    }
  };
}// namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template FunctorDispatcher
// Implements a logarithmic double dispatcher for functors
// Features automated casting
////////////////////////////////////////////////////////////////////////////////

template<class BaseLhs, class BaseRhs = BaseLhs, typename ResultType = void, class CastingPolicy = DynamicCaster, template<class, class, class, class> class DispatcherBackend = BasicDispatcher>
class FunctorDispatcher
{
  //typedef LOKI_TYPELIST_2(BaseLhs &, BaseRhs &) ArgsList;
  //typedef Functor<ResultType, ArgsList, LOKI_DEFAULT_THREADING> FunctorType;
  typedef std::function<ResultType(BaseLhs &, BaseRhs &)> FunctorType;

  DispatcherBackend<BaseLhs, BaseRhs, ResultType, FunctorType> backEnd_;

  template<class SomeLhs, class SomeRhs, class Fun>
  void AddImpl(const Fun &fun)
  {
    typedef Private::FunctorDispatcherHelper<
      BaseLhs,
      BaseRhs,
      SomeLhs,
      SomeRhs,
      ResultType,
      CastingPolicy,
      CastingPolicy,
      Fun,
      false>
      Adapter;

    backEnd_.template Add<SomeLhs, SomeRhs>(FunctorType(Adapter(fun)));
  }
  template<class SomeLhs, class SomeRhs, bool symmetric, class Fun>
  void AddImpl(const Fun &fun)
  {
    AddImpl<SomeLhs, SomeRhs>(fun);

    if (symmetric) {
      // Note: symmetry only makes sense where BaseLhs==BaseRhs
      typedef Private::FunctorDispatcherHelper<
        BaseLhs,
        BaseLhs,
        SomeLhs,
        SomeRhs,
        ResultType,
        CastingPolicy,
        CastingPolicy,
        Fun,
        true>
        AdapterR;
      backEnd_.template Add<SomeRhs, SomeLhs>(FunctorType(AdapterR(fun)));
    }
  }
public:
  template<class Fun>
  void Add(const Fun &fun)
  {
    using Args = ct::args_t<Fun>;
    static_assert(std::tuple_size_v<Args> == 2, "fun should have two params ");
    AddImpl<
      std::remove_reference_t<std::tuple_element_t<0, Args>>,
      std::remove_reference_t<std::tuple_element_t<1, Args>>>(fun);
  }
  template<bool symmetric, class Fun>
  void Add(const Fun &fun)
  {
    using Args = ct::args_t<Fun>;
    static_assert(std::tuple_size_v<Args> == 2, "fun should have two params ");
    AddImpl<
      std::remove_reference_t<std::tuple_element_t<0, Args>>,
      std::remove_reference_t<std::tuple_element_t<1, Args>>,
      symmetric>(fun);
  }

  template<class SomeLhs, class SomeRhs>
  void Remove()
  {
    backEnd_.template Remove<SomeLhs, SomeRhs>();
  }

  ResultType operator()(BaseLhs &lhs, BaseRhs &rhs)
  {
    return backEnd_.Go(lhs, rhs);
  }
};
}// namespace Loki


#endif// end file guardian
