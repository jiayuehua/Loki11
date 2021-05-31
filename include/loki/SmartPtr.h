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
#ifndef LOKI_SMARTPTR_INC_
#define LOKI_SMARTPTR_INC_

// $Id: SmartPtr.h 903 2008-11-10 05:55:12Z rich_sposato $


///  \defgroup  SmartPointerGroup Smart pointers
///  Policy based implementation of a smart pointer
///  \defgroup  SmartPointerOwnershipGroup Ownership policies
///  \ingroup   SmartPointerGroup
///  \defgroup  SmartPointerStorageGroup Storage policies
///  \ingroup   SmartPointerGroup
///  \defgroup  SmartPointerConversionGroup Conversion policies
///  \ingroup   SmartPointerGroup
///  \defgroup  SmartPointerCheckingGroup Checking policies
///  \ingroup   SmartPointerGroup

#include "LokiExport.h"
#include "SmallObj.h"
#include "RefToValue.h"
#include "ConstPolicy.h"

#include <functional>
#include <stdexcept>
#include <cassert>
#include <string>
#include <type_traits>
#include <concepts>
#include <iostream>
#if !defined(_MSC_VER)
#if defined(__sparc__)
#include <inttypes.h>
#else
#include <stdint.h>
#endif
#endif

#if defined(_MSC_VER) || defined(__GNUC__)
// GCC>=4.1 must use -ffriend-injection due to a bug in GCC
#define LOKI_ENABLE_FRIEND_TEMPLATE_TEMPLATE_PARAMETER_WORKAROUND
#endif


namespace Loki {

////////////////////////////////////////////////////////////////////////////////
///  \class HeapStorage
///
///  \ingroup  SmartPointerStorageGroup
///  Implementation of the StoragePolicy used by SmartPtr.  Uses explicit call
///   to T's destructor followed by call to free.
////////////////////////////////////////////////////////////////////////////////


template<class T>
class HeapStorage
{
public:
  typedef T *StoredType;/// the type of the pointee_ object
  typedef T *InitPointerType;/// type used to declare OwnershipPolicy type.
  typedef T *PointerType;/// type returned by operator->
  typedef T &ReferenceType;/// type returned by operator*

  HeapStorage() : pointee_(Default())
  {}

  // The storage policy doesn't initialize the stored pointer
  //     which will be initialized by the OwnershipPolicy's Clone fn
  HeapStorage(const HeapStorage &) : pointee_(0)
  {}

  template<class U>
  HeapStorage(const HeapStorage<U> &) : pointee_(0)
  {}

  HeapStorage(const StoredType &p) : pointee_(p) {}

  PointerType operator->() const { return pointee_; }

  ReferenceType operator*() const { return *pointee_; }

  void Swap(HeapStorage &rhs)
  {
    std::swap(pointee_, rhs.pointee_);
  }

  // Accessors
  friend inline PointerType GetImpl(const HeapStorage &sp)
  {
    return sp.pointee_;
  }

  friend inline const StoredType &GetImplRef(const HeapStorage &sp)
  {
    return sp.pointee_;
  }

  friend inline StoredType &GetImplRef(HeapStorage &sp)
  {
    return sp.pointee_;
  }

protected:
  // Destroys the data stored
  // (Destruction might be taken over by the OwnershipPolicy)
  void Destroy()
  {
    if (0 != pointee_) {
      pointee_->~T();
      ::free(pointee_);
    }
  }

  // Default value to initialize the pointer
  static StoredType Default()
  {
    return 0;
  }

private:
  // Data
  StoredType pointee_;
};


////////////////////////////////////////////////////////////////////////////////
///  \class DefaultSPStorage
///
///  \ingroup  SmartPointerStorageGroup
///  Implementation of the StoragePolicy used by SmartPtr
////////////////////////////////////////////////////////////////////////////////


template<class T>
class DefaultSPStorage
{
public:
  typedef T *StoredType;// the type of the pointee_ object
  typedef T *InitPointerType;/// type used to declare OwnershipPolicy type.
  typedef T *PointerType;// type returned by operator->
  typedef T &ReferenceType;// type returned by operator*

  DefaultSPStorage() : pointee_(Default())
  {}

  // The storage policy doesn't initialize the stored pointer
  //     which will be initialized by the OwnershipPolicy's Clone fn
  DefaultSPStorage(const DefaultSPStorage &) : pointee_(0)
  {}

  template<class U>
  DefaultSPStorage(const DefaultSPStorage<U> &) : pointee_(0)
  {}

  DefaultSPStorage(const StoredType &p) : pointee_(p) {}

  PointerType operator->() const { return pointee_; }

  ReferenceType operator*() const { return *pointee_; }

  void Swap(DefaultSPStorage &rhs)
  {
    std::swap(pointee_, rhs.pointee_);
  }

  friend inline PointerType GetImpl(const DefaultSPStorage &sp)
  {
    return sp.pointee_;
  }

  friend inline const StoredType &GetImplRef(const DefaultSPStorage<T> &sp)
  {
    return sp.pointee_;
  }

  friend inline StoredType &GetImplRef(DefaultSPStorage<T> &sp)
  {
    return sp.pointee_;
  }
  // Accessors

protected:
  // Destroys the data stored
  // (Destruction might be taken over by the OwnershipPolicy)
  //
  // If your compiler gives you a warning in this area while
  // compiling the tests, it is on purpose, please ignore it.
  void Destroy()
  {
    delete pointee_;
  }

  // Default value to initialize the pointer
  static StoredType Default()
  {
    return 0;
  }

private:
  // Data
  StoredType pointee_;
};


////////////////////////////////////////////////////////////////////////////////
///  \class LockedStorage
///
///  \ingroup  SmartPointerStorageGroup
///  Implementation of the StoragePolicy used by SmartPtr.
///
///  Each call to operator-> locks the object for the duration of a call to a
///  member function of T.
///
///  \par How It Works
///  LockedStorage has a helper class called Locker, which acts as a smart
///  pointer with limited abilities.  LockedStorage::operator-> returns an
///  unnamed temporary of type Locker<T> that exists for the duration of the
///  call to a member function of T.  The unnamed temporary locks the object
///  when it is constructed by operator-> and unlocks the object when it is
///  destructed.
///
///  \note This storage policy requires class T to have member functions Lock
///  and Unlock.  If your class does not have Lock or Unlock functions, you may
///  either make a child class which does, or make a policy class similar to
///  LockedStorage which calls other functions to lock the object.
////////////////////////////////////////////////////////////////////////////////

template<class T>
class Locker
{
public:
  Locker(const T *p) : pointee_(const_cast<T *>(p))
  {
    if (pointee_ != 0)
      pointee_->Lock();
  }

  ~Locker(void)
  {
    if (pointee_ != 0)
      pointee_->Unlock();
  }

  operator T *()
  {
    return pointee_;
  }

  T *operator->()
  {
    return pointee_;
  }

private:
  Locker(void);
  Locker &operator=(const Locker &);
  T *pointee_;
};

template<class T>
class LockedStorage
{
public:
  typedef T *StoredType;/// the type of the pointee_ object
  typedef T *InitPointerType;/// type used to declare OwnershipPolicy type.
  typedef Locker<T> PointerType;/// type returned by operator->
  typedef T &ReferenceType;/// type returned by operator*

  LockedStorage() : pointee_(Default()) {}

  ~LockedStorage(void) {}

  LockedStorage(const LockedStorage &) : pointee_(0) {}

  LockedStorage(const StoredType &p) : pointee_(p) {}

  PointerType operator->()
  {
    return Locker<T>(pointee_);
  }

  void Swap(LockedStorage &rhs)
  {
    std::swap(pointee_, rhs.pointee_);
  }

  // Accessors
  friend InitPointerType GetImpl(const LockedStorage &sp);

  friend const StoredType &GetImplRef(const LockedStorage &sp);

  friend StoredType &GetImplRef(LockedStorage &sp);

protected:
  // Destroys the data stored
  // (Destruction might be taken over by the OwnershipPolicy)
  void Destroy()
  {
    delete pointee_;
  }

  // Default value to initialize the pointer
  static StoredType Default()
  {
    return 0;
  }

private:
  /// Dereference operator is not implemented.
  ReferenceType operator*() = delete;

  // Data
  StoredType pointee_;
};


////////////////////////////////////////////////////////////////////////////////
///  \class ArrayStorage
///
///  \ingroup  SmartPointerStorageGroup
///  Implementation of the ArrayStorage used by SmartPtr
////////////////////////////////////////////////////////////////////////////////


template<class T>
class ArrayStorage
{
public:
  typedef T *StoredType;// the type of the pointee_ object
  typedef T *InitPointerType;/// type used to declare OwnershipPolicy type.
  typedef T *PointerType;// type returned by operator->
  typedef T &ReferenceType;// type returned by operator*

  ArrayStorage() : pointee_(Default())
  {}

  // The storage policy doesn't initialize the stored pointer
  //     which will be initialized by the OwnershipPolicy's Clone fn
  ArrayStorage(const ArrayStorage &) : pointee_(0)
  {}

  template<class U>
  ArrayStorage(const ArrayStorage<U> &) : pointee_(0)
  {}

  ArrayStorage(const StoredType &p) : pointee_(p) {}

  PointerType operator->() const { return pointee_; }

  ReferenceType operator*() const { return *pointee_; }

  void Swap(ArrayStorage &rhs)
  {
    std::swap(pointee_, rhs.pointee_);
  }

  // Accessors
  friend PointerType GetImpl(const ArrayStorage &sp);

  friend StoredType &GetImplRef(const ArrayStorage &sp);

  friend StoredType &GetImplRef(ArrayStorage &sp);

protected:
  // Destroys the data stored
  // (Destruction might be taken over by the OwnershipPolicy)
  void Destroy()
  {
    delete[] pointee_;
  }

  // Default value to initialize the pointer
  static StoredType Default()
  {
    return 0;
  }

private:
  // Data
  StoredType pointee_;
};


////////////////////////////////////////////////////////////////////////////////
///  \class RefCounted
///
///  \ingroup  SmartPointerOwnershipGroup
///  Implementation of the OwnershipPolicy used by SmartPtr
///  Provides a classic external reference counting implementation
////////////////////////////////////////////////////////////////////////////////

template<class P>
class RefCounted
{
public:
  RefCounted()
    : pCount_(static_cast<uintptr_t *>(
      SmallObject<>::operator new(sizeof(uintptr_t))))
  {
    assert(pCount_ != 0);
    *pCount_ = 1;
  }

  RefCounted(const RefCounted &rhs)
    : pCount_(rhs.pCount_)
  {}

  // MWCW lacks template friends, hence the following kludge
  template<typename P1>
  RefCounted(const RefCounted<P1> &rhs)
    : pCount_(reinterpret_cast<const RefCounted &>(rhs).pCount_)
  {}

  P Clone(const P &val)
  {
    ++*pCount_;
    return val;
  }

  bool Release(const P &)
  {
    if (!--*pCount_) {
      SmallObject<>::operator delete(pCount_, sizeof(uintptr_t));
      pCount_ = NULL;
      return true;
    }
    return false;
  }

  void Swap(RefCounted &rhs)
  {
    std::swap(pCount_, rhs.pCount_);
  }

  enum { destructiveCopy = false };

private:
  // Data
  uintptr_t *pCount_;
};

////////////////////////////////////////////////////////////////////////////////
///  \struct RefCountedMT
///
///  \ingroup  SmartPointerOwnershipGroup
///  Implementation of the OwnershipPolicy used by SmartPtr
///  Implements external reference counting for multithreaded programs
///  Policy Usage: RefCountedMTAdj<ThreadingModel>::RefCountedMT
///
///  \par Warning
///  There could be a race condition, see bug "Race condition in RefCountedMTAdj::Release"
///  http://sourceforge.net/tracker/index.php?func=detail&aid=1408845&group_id=29557&atid=396644
///  As stated in bug 1408845, the Release function is not thread safe if a
///  SmartPtr copy-constructor tries to copy the last pointer to an object in
///  one thread, while the destructor is acting on the last pointer in another
///  thread.  The existence of a race between a copy-constructor and destructor
///  implies a design flaw at a higher level.  That race condition must be
///  fixed at a higher design level, and no change to this class could fix it.
////////////////////////////////////////////////////////////////////////////////

template<template<class, class> class ThreadingModel,
  class MX = LOKI_DEFAULT_MUTEX>
struct RefCountedMTAdj
{
  template<class P>
  class RefCountedMT : public ThreadingModel<RefCountedMT<P>, MX>
  {
    typedef ThreadingModel<RefCountedMT<P>, MX> base_type;
    typedef typename base_type::IntType CountType;
    typedef volatile CountType *CountPtrType;

  public:
    RefCountedMT()
    {
      pCount_ = static_cast<CountPtrType>(
        SmallObject<LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL>::operator new(
          sizeof(*pCount_)));
      assert(pCount_);
      //*pCount_ = 1;
      ThreadingModel<RefCountedMT, MX>::AtomicAssign(*pCount_, 1);
    }

    RefCountedMT(const RefCountedMT &rhs)
      : pCount_(rhs.pCount_)
    {}

    //MWCW lacks template friends, hence the following kludge
    template<typename P1>
    RefCountedMT(const RefCountedMT<P1> &rhs)
      : pCount_(reinterpret_cast<const RefCountedMT<P> &>(rhs).pCount_)
    {}

    P Clone(const P &val)
    {
      ThreadingModel<RefCountedMT, MX>::AtomicIncrement(*pCount_);
      return val;
    }

    bool Release(const P &)
    {
      bool isZero = false;
      ThreadingModel<RefCountedMT, MX>::AtomicDecrement(*pCount_, 0, isZero);
      if (isZero) {
        SmallObject<LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL>::operator delete(
          const_cast<CountType *>(pCount_),
          sizeof(*pCount_));
        return true;
      }
      return false;
    }

    void Swap(RefCountedMT &rhs)
    {
      std::swap(pCount_, rhs.pCount_);
    }

    enum { destructiveCopy = false };

  private:
    // Data
    CountPtrType pCount_;
  };
};

////////////////////////////////////////////////////////////////////////////////
///  \class COMRefCounted
///
///  \ingroup  SmartPointerOwnershipGroup
///  Implementation of the OwnershipPolicy used by SmartPtr
///  Adapts COM intrusive reference counting to OwnershipPolicy-specific syntax
////////////////////////////////////////////////////////////////////////////////

template<class P>
class COMRefCounted
{
public:
  COMRefCounted()
  {}

  template<class U>
  COMRefCounted(const COMRefCounted<U> &)
  {}

  static P Clone(const P &val)
  {
    if (val != 0)
      val->AddRef();
    return val;
  }

  static bool Release(const P &val)
  {
    if (val != 0)
      val->Release();
    return false;
  }

  enum { destructiveCopy = false };

  static void Swap(COMRefCounted &)
  {}
};

////////////////////////////////////////////////////////////////////////////////
///  \struct DeepCopy
///
///  \ingroup  SmartPointerOwnershipGroup
///  Implementation of the OwnershipPolicy used by SmartPtr
///  Implements deep copy semantics, assumes existence of a Clone() member
///  function of the pointee type
////////////////////////////////////////////////////////////////////////////////

template<class P>
struct DeepCopy
{
  DeepCopy()
  {}

  template<class P1>
  DeepCopy(const DeepCopy<P1> &)
  {}

  static P Clone(const P &val)
  {
    return val->Clone();
  }

  static bool Release(const P &)
  {
    return true;
  }

  static void Swap(DeepCopy &)
  {}

  enum { destructiveCopy = false };
};

////////////////////////////////////////////////////////////////////////////////
///  \class RefLinked
///
///  \ingroup  SmartPointerOwnershipGroup
///  Implementation of the OwnershipPolicy used by SmartPtr
///  Implements reference linking
////////////////////////////////////////////////////////////////////////////////

namespace Private {
  class LOKI_EXPORT RefLinkedBase
  {
  public:
    RefLinkedBase()
    {
      prev_ = next_ = this;
    }

    RefLinkedBase(const RefLinkedBase &rhs);

    bool Release();

    void Swap(RefLinkedBase &rhs);

    bool Merge(RefLinkedBase &rhs);

    enum { destructiveCopy = false };

  private:
    static unsigned int CountPrevCycle(const RefLinkedBase *pThis);
    static unsigned int CountNextCycle(const RefLinkedBase *pThis);
    bool HasPrevNode(const RefLinkedBase *p) const;
    bool HasNextNode(const RefLinkedBase *p) const;

    mutable const RefLinkedBase *prev_;
    mutable const RefLinkedBase *next_;
  };
}// namespace Private

template<class P>
class RefLinked : public Private::RefLinkedBase
{
public:
  RefLinked()
  {}

  template<class P1>
  RefLinked(const RefLinked<P1> &rhs)
    : Private::RefLinkedBase(rhs)
  {}

  static P Clone(const P &val)
  {
    return val;
  }

  bool Release(const P &)
  {
    return Private::RefLinkedBase::Release();
  }

  template<class P1>
  bool Merge(RefLinked<P1> &rhs)
  {
    return Private::RefLinkedBase::Merge(rhs);
  }
};

////////////////////////////////////////////////////////////////////////////////
///  \class DestructiveCopy
///
///  \ingroup  SmartPointerOwnershipGroup
///  Implementation of the OwnershipPolicy used by SmartPtr
///  Implements destructive copy semantics (a la std::auto_ptr)
////////////////////////////////////////////////////////////////////////////////

template<class P>
class DestructiveCopy
{
public:
  DestructiveCopy()
  {}

  template<class P1>
  DestructiveCopy(const DestructiveCopy<P1> &) requires false {};

  DestructiveCopy(const DestructiveCopy<P> &) requires false = default;

  template<class P1>
  //  requires(!std::is_same_v<std::remove_cvref_t<P1>, std::remove_cvref_t<P>>)
  DestructiveCopy(DestructiveCopy<P1> &&)
  {
    std::cout << "DestructiveCopy move other cons\n";
  };

  DestructiveCopy(DestructiveCopy<P> &&)
  {
    std::cout << "DestructiveCopy move cons\n";
  };

  template<class P1>
  static P Clone(P1 &val)
  {
    P result(val);
    val = P1();
    return result;
  }

  static bool Release(const P &)
  {
    return true;
  }

  static void Swap(DestructiveCopy &)
  {}

  enum { destructiveCopy = true };
};

////////////////////////////////////////////////////////////////////////////////
///  \class NoCopy
///
///  \ingroup  SmartPointerOwnershipGroup
///  Implementation of the OwnershipPolicy used by SmartPtr
///  Implements a policy that doesn't allow copying objects
////////////////////////////////////////////////////////////////////////////////

template<class P>
class NoCopy
{
public:
  NoCopy()
  {}

  NoCopy(const NoCopy<P> &) requires false = default;

  NoCopy(NoCopy<P> &&) requires false = default;
  template<class P1>
  requires false NoCopy(const NoCopy<P1> &) = delete;

  template<class P1>
  requires false NoCopy(NoCopy<P1> &&);
  static P Clone(const P &)
  {
    // Make it depended on template parameter
    static const bool DependedFalse = sizeof(P *) == 0;

    static_assert(DependedFalse, " This_Policy_Disallows_Value_Copying");
  }

  static bool Release(const P &)
  {
    return true;
  }

  static void Swap(NoCopy &)
  {}

  enum { destructiveCopy = false };
};

////////////////////////////////////////////////////////////////////////////////
///  \struct AllowConversion
///
///  \ingroup  SmartPointerConversionGroup
///  Allows implicit conversion from SmartPtr to the pointee type
////////////////////////////////////////////////////////////////////////////////

struct AllowConversion
{
  enum { allow = true };

  void Swap(AllowConversion &)
  {}
};

////////////////////////////////////////////////////////////////////////////////
///  \struct DisallowConversion
///
///  \ingroup  SmartPointerConversionGroup
///  Does not allow implicit conversion from SmartPtr to the pointee type
///  You can initialize a DisallowConversion with an AllowConversion
////////////////////////////////////////////////////////////////////////////////

struct DisallowConversion
{
  DisallowConversion()
  {}

  DisallowConversion(const AllowConversion &)
  {}

  enum { allow = false };

  void Swap(DisallowConversion &)
  {}
};

////////////////////////////////////////////////////////////////////////////////
///  \struct NoCheck
///
///  \ingroup  SmartPointerCheckingGroup
///  Implementation of the CheckingPolicy used by SmartPtr
///  Well, it's clear what it does :o)
////////////////////////////////////////////////////////////////////////////////

template<class P>
struct NoCheck
{
  NoCheck()
  {}

  template<class P1>
  NoCheck(const NoCheck<P1> &)
  {}

  static void OnDefault(const P &)
  {}

  static void OnInit(const P &)
  {}

  static void OnDereference(const P &)
  {}

  static void Swap(NoCheck &)
  {}
};


////////////////////////////////////////////////////////////////////////////////
///  \struct AssertCheck
///
///  \ingroup  SmartPointerCheckingGroup
///  Implementation of the CheckingPolicy used by SmartPtr
///  Checks the pointer before dereference
////////////////////////////////////////////////////////////////////////////////

template<class P>
struct AssertCheck
{
  AssertCheck()
  {}

  template<class P1>
  AssertCheck(const AssertCheck<P1> &)
  {}

  template<class P1>
  AssertCheck(const NoCheck<P1> &)
  {}

  static void OnDefault(const P &)
  {}

  static void OnInit(const P &)
  {}

  static void OnDereference(P val)
  {
    assert(val);
    (void)val;
  }

  static void Swap(AssertCheck &)
  {}
};

////////////////////////////////////////////////////////////////////////////////
///  \struct AssertCheckStrict
///
///  \ingroup  SmartPointerCheckingGroup
///  Implementation of the CheckingPolicy used by SmartPtr
///  Checks the pointer against zero upon initialization and before dereference
///  You can initialize an AssertCheckStrict with an AssertCheck
////////////////////////////////////////////////////////////////////////////////

template<class P>
struct AssertCheckStrict
{
  AssertCheckStrict()
  {}

  template<class U>
  AssertCheckStrict(const AssertCheckStrict<U> &)
  {}

  template<class U>
  AssertCheckStrict(const AssertCheck<U> &)
  {}

  template<class P1>
  AssertCheckStrict(const NoCheck<P1> &)
  {}

  static void OnDefault(P val)
  {
    assert(val);
  }

  static void OnInit(P val)
  {
    assert(val);
  }

  static void OnDereference(P val)
  {
    assert(val);
  }

  static void Swap(AssertCheckStrict &)
  {}
};

////////////////////////////////////////////////////////////////////////////////
///  \struct NullPointerException
///
///  \ingroup SmartPointerGroup
///  Used by some implementations of the CheckingPolicy used by SmartPtr
////////////////////////////////////////////////////////////////////////////////

struct NullPointerException : public std::runtime_error
{
  NullPointerException() : std::runtime_error(std::string(""))
  {}
  const char *what() const throw()
  {
    return "Null Pointer Exception";
  }
};

////////////////////////////////////////////////////////////////////////////////
///  \struct RejectNullStatic
///
///  \ingroup  SmartPointerCheckingGroup
///  Implementation of the CheckingPolicy used by SmartPtr
///  Checks the pointer upon initialization and before dereference
////////////////////////////////////////////////////////////////////////////////

template<class P>
struct RejectNullStatic
{
  RejectNullStatic()
  {}

  template<class P1>
  RejectNullStatic(const RejectNullStatic<P1> &)
  {}

  template<class P1>
  RejectNullStatic(const NoCheck<P1> &)
  {}

  template<class P1>
  RejectNullStatic(const AssertCheck<P1> &)
  {}

  template<class P1>
  RejectNullStatic(const AssertCheckStrict<P1> &)
  {}

  static void OnDefault(const P &)
  {
    // Make it depended on template parameter
    static const bool DependedFalse = sizeof(P *) == 0;

    static_assert(DependedFalse, " ERROR_This_Policy_Does_Not_Allow_Default_Initialization");
  }

  static void OnInit(const P &val)
  {
    if (!val) throw NullPointerException();
  }

  static void OnDereference(const P &val)
  {
    if (!val) throw NullPointerException();
  }

  static void Swap(RejectNullStatic &)
  {}
};

////////////////////////////////////////////////////////////////////////////////
///  \struct RejectNull
///
///  \ingroup  SmartPointerCheckingGroup
///  Implementation of the CheckingPolicy used by SmartPtr
///  Checks the pointer before dereference
////////////////////////////////////////////////////////////////////////////////

template<class P>
struct RejectNull
{
  RejectNull()
  {}

  template<class P1>
  RejectNull(const RejectNull<P1> &)
  {}

  static void OnInit(P)
  {}

  static void OnDefault(P)
  {}

  void OnDereference(P val)
  {
    if (!val) throw NullPointerException();
  }

  void OnDereference(P val) const
  {
    if (!val) throw NullPointerException();
  }

  void Swap(RejectNull &)
  {}
};

////////////////////////////////////////////////////////////////////////////////
///  \struct RejectNullStrict
///
///  \ingroup  SmartPointerCheckingGroup
///  Implementation of the CheckingPolicy used by SmartPtr
///  Checks the pointer upon initialization and before dereference
////////////////////////////////////////////////////////////////////////////////

template<class P>
struct RejectNullStrict
{
  RejectNullStrict()
  {}

  template<class P1>
  RejectNullStrict(const RejectNullStrict<P1> &)
  {}

  template<class P1>
  RejectNullStrict(const RejectNull<P1> &)
  {}

  static void OnInit(P val)
  {
    if (!val) throw NullPointerException();
  }

  void OnDereference(P val)
  {
    OnInit(val);
  }

  void OnDereference(P val) const
  {
    OnInit(val);
  }

  void Swap(RejectNullStrict &)
  {}
};


////////////////////////////////////////////////////////////////////////////////
// class template SmartPtr (declaration)
// The reason for all the fuss above
////////////////////////////////////////////////////////////////////////////////

template<
  typename T,
  template<class> class OwnershipPolicy = RefCounted,
  template<class> class CheckingPolicy = AssertCheck,
  template<class> class StoragePolicy = DefaultSPStorage,
  template<class> class ConstnessPolicy = LOKI_DEFAULT_CONSTNESS>
class SmartPtr;

////////////////////////////////////////////////////////////////////////////////
// class template SmartPtrDef (definition)
// this class added to unify the usage of SmartPtr
// instead of writing SmartPtr<T,OP,CP,KP,SP> write SmartPtrDef<T,OP,CP,KP,SP>::type
////////////////////////////////////////////////////////////////////////////////

template<
  typename T,
  template<class> class OwnershipPolicy = RefCounted,
  template<class> class CheckingPolicy = AssertCheck,
  template<class> class StoragePolicy = DefaultSPStorage,
  template<class> class ConstnessPolicy = LOKI_DEFAULT_CONSTNESS>
struct SmartPtrDef
{
  typedef SmartPtr<
    T,
    OwnershipPolicy,
    CheckingPolicy,
    StoragePolicy,
    ConstnessPolicy>
    type;
};

////////////////////////////////////////////////////////////////////////////////
///  \class SmartPtr
///
///  \ingroup SmartPointerGroup
///
///  \param OwnershipPolicy  default =  RefCounted,
///  \param CheckingPolicy default = AssertCheck,
///  \param StoragePolicy default = DefaultSPStorage
///  \param ConstnessPolicy default = LOKI_DEFAULT_CONSTNESS
///
///  \par IMPORTANT NOTE
///  Due to threading issues, the OwnershipPolicy has been changed as follows:
///
///     - Release() returns a boolean saying if that was the last release
///        so the pointer can be deleted by the StoragePolicy
///     - IsUnique() was removed
////////////////////////////////////////////////////////////////////////////////

template<
  typename T,
  template<class>
  class OwnershipPolicy,
  template<class>
  class CheckingPolicy,
  template<class>
  class StoragePolicy,
  template<class>
  class ConstnessPolicy>
class SmartPtr
  : public StoragePolicy<T>
  , public OwnershipPolicy<typename StoragePolicy<T>::InitPointerType>
  , public CheckingPolicy<typename StoragePolicy<T>::StoredType>
{
  typedef StoragePolicy<T> SP;
  typedef OwnershipPolicy<typename StoragePolicy<T>::InitPointerType> OP;
  typedef CheckingPolicy<typename StoragePolicy<T>::StoredType> KP;

public:
  typedef typename ConstnessPolicy<T>::Type *ConstPointerType;
  typedef typename ConstnessPolicy<T>::Type &ConstReferenceType;

  typedef typename SP::PointerType PointerType;
  typedef typename SP::StoredType StoredType;
  typedef typename SP::ReferenceType ReferenceType;

  typedef typename std::conditional<OP::destructiveCopy, SmartPtr, const SmartPtr>::type
    CopyArg;

private:
  struct NeverMatched
  {
  };

#ifdef LOKI_SMARTPTR_CONVERSION_CONSTRUCTOR_POLICY
  //typedef typename std::conditional<CP::allow, const StoredType &, NeverMatched>::type ImplicitArg;
  //typedef typename std::conditional<!CP::allow, const StoredType &, NeverMatched>::type ExplicitArg;
#else
  typedef const StoredType &ExplicitArg;
  //typedef typename std::conditional<false, const StoredType &, NeverMatched>::type ExplicitArg;
#endif

public:
  SmartPtr()
  {
    KP::OnDefault(GetImpl(*this));
  }

  explicit SmartPtr(ExplicitArg p) : SP(p)
  {
    KP::OnInit(GetImpl(*this));
  }


  SmartPtr(const CopyArg &rhs) requires std::is_copy_constructible_v<OP>
    : SP(rhs)
    , OP(rhs)
    , KP(rhs)
  {
    GetImplRef(*this) = OP::Clone(GetImplRef(rhs));
  }


  SmartPtr(SmartPtr &&rhs) requires(std::is_move_constructible_v<OP>)
    : SP(rhs), OP(std::move(rhs)), KP(rhs)
  {
    std::cout << "SP move construc\n";
    GetImplRef(*this) = OP::Clone(GetImplRef(rhs));
  }
  template<
    typename T1>
  SmartPtr(SmartPtr<T1> &&rhs) requires(!std::same_as<T, T1> && std::is_move_constructible_v<OP>)
    : SP(rhs), OP(std::move(rhs)), KP(rhs)
  {
    GetImplRef(*this) = OP::Clone(GetImplRef(rhs));
  }


  SmartPtr &operator=(SmartPtr &&rhs)
  {
    SmartPtr temp(std::move(rhs));
    return temp;
  }

  template<
    typename T1>
  SmartPtr &operator=(const SmartPtr<T1, OwnershipPolicy, CheckingPolicy, StoragePolicy, ConstnessPolicy> &rhs)
  {
    SmartPtr temp(rhs);
    temp.Swap(*this);
    return *this;
  }

  template<typename T1>
  SmartPtr &operator=(SmartPtr<T1, OwnershipPolicy, CheckingPolicy, StoragePolicy, ConstnessPolicy> &&rhs)
  {
    SmartPtr temp(std::move(rhs));
    return temp;
  }

  void Swap(SmartPtr &rhs)
  {
    OP::Swap(rhs);
    KP::Swap(rhs);
    SP::Swap(rhs);
  }

  ~SmartPtr()
  {
    if (OP::Release(GetImpl(*static_cast<SP *>(this)))) {
      SP::Destroy();
    }
  }

#ifdef LOKI_ENABLE_FRIEND_TEMPLATE_TEMPLATE_PARAMETER_WORKAROUND

  // old non standard in class definition of friends
  friend inline typename SP::StoredType Release(SmartPtr &sp)
  {
    typename SP::StoredType p = GetImplRef(sp);
    GetImplRef(sp) = SP::Default();
    return p;
  }

  friend inline void Reset(SmartPtr &sp, typename SP::StoredType p)
  {
    SmartPtr(p).Swap(sp);
  }

#else

#endif


  PointerType operator->()
  {
    KP::OnDereference(GetImplRef(*this));
    return SP::operator->();
  }

  ConstPointerType operator->() const
  {
    KP::OnDereference(GetImplRef(*this));
    return SP::operator->();
  }

  ReferenceType operator*()
  {
    KP::OnDereference(GetImplRef(*this));
    return SP::operator*();
  }

  ConstReferenceType operator*() const
  {
    KP::OnDereference(GetImplRef(*this));
    return SP::operator*();
  }

  bool operator!() const// Enables "if (!sp) ..."
  {
    return GetImpl(*this) == nullptr;
  }

  static inline T *GetPointer(const SmartPtr &sp)
  {
    return GetImpl(sp);
  }

  // Ambiguity buster
  template<typename T1>
  bool operator==(const SmartPtr<T1, OwnershipPolicy, CheckingPolicy, StoragePolicy, ConstnessPolicy> &rhs) const
  {
    return GetImpl(*this) == GetImpl(rhs);
  }

  // Ambiguity buster
  template<
    typename T1>
  auto operator<=>(const SmartPtr<T1, OwnershipPolicy, CheckingPolicy, StoragePolicy, ConstnessPolicy> &rhs) const
  {
    return GetImpl(*this) <=> GetImpl(rhs);
  }

  template<typename T1, class U>
  bool operator==(const U *rhs) const
  {
    return GetImpl(*this) == rhs;
  }

  // Ambiguity buster
  template<
    typename T1,
    class U>
  auto operator<=>(const U *rhs) const
  {
    return GetImpl(*this) <=> rhs;
  }

private:
  // Helper for enabling 'if (sp)'
  struct Tester
  {
    Tester(int) {}
    void dummy() {}
  };

  typedef void (Tester::*unspecified_boolean_type)();


public:
  // enable 'if (sp)'
  operator unspecified_boolean_type() const
  {
    return !*this ? 0 : &Tester::dummy;
  }
};


}// namespace Loki

////////////////////////////////////////////////////////////////////////////////
///  specialization of std::less for SmartPtr
///  \ingroup SmartPointerGroup
////////////////////////////////////////////////////////////////////////////////

namespace std {
template<
  typename T,
  template<class>
  class OP,
  template<class>
  class KP,
  template<class>
  class SP,
  template<class>
  class CNP>
struct less<Loki::SmartPtr<T, OP, KP, SP, CNP>>
{
  bool operator()(const Loki::SmartPtr<T, OP, KP, SP, CNP> &lhs,
    const Loki::SmartPtr<T, OP, KP, SP, CNP> &rhs) const
  {
    return less<T *>()(GetImpl(lhs), GetImpl(rhs));
  }
};
}// namespace std

#endif// end file guardian
