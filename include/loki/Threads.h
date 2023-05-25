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
#ifndef LOKI_THREADS_INC_
#define LOKI_THREADS_INC_

// $Id: Threads.h 902 2008-11-10 05:47:06Z rich_sposato $


///  @defgroup  ThreadingGroup Threading
///  Policies to for the threading model:
///
///  - SingleThreaded
///  - ObjectLevelLockable
///  - ClassLevelLockable
///
///  All classes in Loki have configurable threading model.
///
///  The macro LOKI_DEFAULT_THREADING selects the default
///  threading model for certain components of Loki
///  (it affects only default template arguments)
///
///  \par Usage:
///
///  To use a specific threading model define
///
///  - nothing, single-theading is default
///  - LOKI_OBJECT_LEVEL_THREADING for object-level-threading
///  - LOKI_CLASS_LEVEL_THREADING for class-level-threading
///
///  \par Supported platfroms:
///
///  - Windows (windows.h)
///  - POSIX (pthread.h):
///    No recursive mutex support with pthread.
///    This means: calling Lock() on a Loki::Mutex twice from the
///    same thread before unlocking the mutex deadlocks the system.
///    To avoid this redesign your synchronization. See also:
///    http://sourceforge.net/tracker/index.php?func=detail&aid=1516182&group_id=29557&atid=396647


#include <cassert>


namespace Loki {

////////////////////////////////////////////////////////////////////////////////
///  \class Mutex
//
///  \ingroup ThreadingGroup
///  A simple and portable Mutex.  A default policy class for locking objects.
////////////////////////////////////////////////////////////////////////////////

class Mutex
{
public:
  Mutex() = default;
  ~Mutex() = default;
  void Lock()
  {
  }
  void Unlock()
  {
  }

private:
  /// Copy-constructor not implemented.
  Mutex(const Mutex &) = delete;
  /// Copy-assignement operator not implemented.
  Mutex &operator=(const Mutex &) = delete;
};


////////////////////////////////////////////////////////////////////////////////
///  \class SingleThreaded
///
///  \ingroup ThreadingGroup
///  Implementation of the ThreadingModel policy used by various classes
///  Implements a single-threaded model; no synchronization
////////////////////////////////////////////////////////////////////////////////
template<class Host, class MutexPolicy = Mutex>
class SingleThreaded
{
public:
  /// \struct Lock
  /// Dummy Lock class
  struct Lock
  {
    Lock() {}
    explicit Lock(const SingleThreaded &) {}
    explicit Lock(const SingleThreaded *) {}
  };

  typedef Host VolatileType;

  typedef int IntType;

  static IntType AtomicAdd(IntType &lval, const IntType val)
  {
    return lval += val;
  }

  static IntType AtomicSubtract(IntType &lval, const IntType val)
  {
    return lval -= val;
  }

  static IntType AtomicMultiply(IntType &lval, const IntType val)
  {
    return lval *= val;
  }

  static IntType AtomicDivide(IntType &lval, const IntType val)
  {
    return lval /= val;
  }

  static IntType AtomicIncrement(IntType &lval)
  {
    return ++lval;
  }

  static IntType AtomicDecrement(IntType &lval)
  {
    return --lval;
  }

  static void AtomicAssign(IntType &lval, const IntType val)
  {
    lval = val;
  }

  static void AtomicAssign(IntType &lval, IntType &val)
  {
    lval = val;
  }

  static IntType AtomicAdd(IntType &lval, const IntType val, const IntType compare, bool &matches)
  {
    lval += val;
    matches = (lval == compare);
    return lval;
  }

  static IntType AtomicSubtract(IntType &lval, const IntType val, const IntType compare, bool &matches)
  {
    lval -= val;
    matches = (lval == compare);
    return lval;
  }

  static IntType AtomicMultiply(IntType &lval, const IntType val, const IntType compare, bool &matches)
  {
    lval *= val;
    matches = (lval == compare);
    return lval;
  }

  static IntType AtomicDivide(IntType &lval, const IntType val, const IntType compare, bool &matches)
  {
    lval /= val;
    matches = (lval == compare);
    return lval;
  }

  static IntType AtomicIncrement(IntType &lval, const IntType compare, bool &matches)
  {
    ++lval;
    matches = (lval == compare);
    return lval;
  }

  static IntType AtomicDecrement(IntType &lval, const IntType compare, bool &matches)
  {
    --lval;
    matches = (lval == compare);
    return lval;
  }
};

using LOKI_DEFAULT_MUTEX = Mutex;

}// namespace Loki


#endif// end file guardian
