////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_REGISTER_INC_
#define LOKI_REGISTER_INC_

// $Id: Register.h 776 2006-11-09 13:12:57Z syntheticpp $

#include <type_traits>
#include "HierarchyGenerators.h"

///  \defgroup RegisterGroup Register

namespace Loki {

////////////////////////////////////////////////////////////////////////////////
//
//  Helper classes/functions for RegisterByCreateSet
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///  \ingroup RegisterGroup
///  Must be specialized be the user
////////////////////////////////////////////////////////////////////////////////
template<class t>
bool RegisterFunction();

////////////////////////////////////////////////////////////////////////////////
///  \ingroup RegisterGroup
///  Must be specialized be the user
////////////////////////////////////////////////////////////////////////////////
template<class t>
bool UnRegisterFunction();

namespace Private {
  template<class T>
  struct RegisterOnCreate
  {
    RegisterOnCreate() { RegisterFunction<T>(); }
  };

  template<class T>
  struct UnRegisterOnDelete
  {
    ~UnRegisterOnDelete() { UnRegisterFunction<T>(); }
  };

  template<class T>
  struct RegisterOnCreateElement
  {
    RegisterOnCreate<T> registerObj;
  };

  template<class T>
  struct UnRegisterOnDeleteElement
  {
    UnRegisterOnDelete<T> unregisterObj;
  };
}// namespace Private

////////////////////////////////////////////////////////////////////////////////
///  \class RegisterOnCreateSet
///
///  \ingroup RegisterGroup
///  Implements a generic register class which registers classes of a typelist
///
///  \par Usage
///  see test/Register
////////////////////////////////////////////////////////////////////////////////

template<typename... ElementList>
struct RegisterOnCreateSet
  : GenScatterHierarchy<Private::RegisterOnCreateElement, ElementList...>
{
};

////////////////////////////////////////////////////////////////////////////////
///  \class UnRegisterOnDeleteSet
///
///  \ingroup RegisterGroup
///  Implements a generic register class which unregisters classes of a typelist
///
///  \par Usage
///  see test/Register
////////////////////////////////////////////////////////////////////////////////
template<typename... ElementList>
struct UnRegisterOnDeleteSet
  : GenScatterHierarchy<Private::UnRegisterOnDeleteElement, ElementList...>
{
};


}// namespace Loki


#endif// end file guardian
