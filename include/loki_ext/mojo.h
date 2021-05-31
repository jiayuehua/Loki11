
// Mojo uses a unique type to get the compiler to pick the right constructor.
//////////////////////////////////////////////////////////////////////////////
// MOJO: Moving Joint Objects
// Created by Andrei Alexandrescu
//
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this software
//     for any purpose. It is provided "as is"
//     without express or implied warranty.
//
// Change Log:
//      28 Oct 2002 Gary Powell added operator-> to constant and temporary and
//                              fnresult. Also templated conversion operators to
//                              mojo::enabled.
////////////////////////////////////////////////////////////////////////////////

#ifndef MOJO_H_
#define MOJO_H_
#include <iterator>
#include <type_traits>
namespace mojo {
template<class T>
class constant// type sugar for constants
{
  const T *data_;

public:
  explicit constant(const T &obj) : data_(&obj)
  {}

  template<class S>
  constant(constant<S> const &rhs)
    : data_(rhs.data_)
  {}

  const T &get() const
  {
    return *data_;
  }

  T const *operator->() const
  {
    return data_;
  }

private:
  constant &operator=(constant const &);
};

// forward declaration.
template<class T>
class Fnresult;

template<class T>
class Temporary : private constant<T>// type sugar for temporaries
{
  template<class S>
  friend class Temporary;

public:
  explicit Temporary(T &obj)
    : constant<T>(obj)
  {}

  template<class S>
  Temporary(Temporary<S> &rhs)
    : constant<T>(rhs)
  {}

  template<class S>
  Temporary(Fnresult<S> const &rhs)
    : constant<T>(rhs.get())
  {}

  T &get() const
  {
    return const_cast<T &>(constant<T>::get());
  }

  T const *operator->() const
  {
    return constant<T>::operator->();
  }

  T *operator->()
  {
    return const_cast<T *>(constant<T>::operator->());
  }
};

template<class T>
class Fnresult
{
  T m_T;

public:
  explicit Fnresult(T &rhs)
    : m_T(Temporary<T>(rhs))
  {}

  // The cast's below is valid given that nobody ever really creates a
  // const fnresult object
  Fnresult(const Fnresult &rhs)
    : m_T(Temporary<T>(const_cast<Fnresult &>(rhs)))
  {}

  template<class S>
  Fnresult(const Fnresult<S> &rhs)
    : m_T(Temporary<T>(const_cast<Fnresult<S> &>(rhs)))
  {}

  T &get()
  {
    return const_cast<T &>(m_T);
  }

  T const &get() const
  {
    return m_T;
  }

  T const *operator->() const
  {
    return &m_T;
  }

  T *operator->()
  {
    return &m_T;
  }

private:
  Fnresult &operator=(Fnresult const &);
};

template<class T>
struct Enabled
{
  operator Temporary<T>()
  {
    return Temporary<T>(static_cast<T &>(*this));
  }
  template<class S>
  operator Temporary<S>()
  {
    return Temporary<S>(static_cast<S &>(*this));
  }

  operator constant<T>() const
  {
    return constant<T>(static_cast<const T &>(*this));
  }
  template<class S>
  operator constant<S>() const
  {
    return constant<S>(static_cast<const S &>(*this));
  }

  operator Fnresult<T>()
  {
    return Fnresult<T>(static_cast<T &>(*this));
  }
  template<class S>
  operator Fnresult<S>()
  {
    return Fnresult<S>(static_cast<S &>(*this));
  }

protected:
  Enabled() {}// intended to be derived from
};

template<class T>
struct traits
{
  enum { enabled = std::is_base_of_v<Enabled<T>, T> && !std::is_same_v<Enabled<T>, T> };

  typedef typename std::conditional<
    enabled,
    Temporary<T>,
    T &>::type
    temporary;

  typedef typename std::conditional<
    enabled,
    Fnresult<T>,
    T>::type
    fnresult;
};

template<class T>
inline typename traits<T>::temporary as_temporary(T &src)
{
  typedef typename traits<T>::temporary temp;
  return temp(src);
}

template<class Iter1, class Iter2>
Iter2 move(Iter1 begin, Iter1 end, Iter2 dest)
{
  for (; begin != end; ++begin, ++dest) {
    *dest = as_temporary(*begin);
  }
  return dest;
}

template<class Iter1, class Iter2>
Iter2 uninitialized_move_impl(Iter1 begin, Iter1 end, Iter2 dest, std::output_iterator_tag)
{
  typedef typename std::iterator_traits<Iter2>::value_type T;

  for (; begin != end; ++begin, ++dest) {
    new (*dest) T(as_temporary(*begin));
  }
  return dest;
}
template<class Iter1, class Iter2, class Iter_tag>
Iter2 uninitialized_move_impl(Iter1 begin, Iter1 end, Iter2 dest, Iter_tag)
{
  typedef typename std::iterator_traits<Iter2>::value_type T;
  Iter2 built = dest;
  try {
    for (; begin != end; ++begin, ++built) {
      new (&*built) T(as_temporary(*begin));
    }
  } catch (...) {
    for (; dest != built; ++dest) {
      dest->~T();
    }
    throw;
  }
  return built;
}
template<class Iter1, class Iter2>
Iter2 uninitialized_move(Iter1 begin, Iter1 end, Iter2 dest)
{
  return uninitialized_move_impl(begin, end, dest, std::iterator_traits<Iter2>::iterator_category());
}
}// namespace mojo

#endif// MOJO_H_
//
//// ------------------- testpointer.h --------
//#if !defined(_TESTPTR_H)
//#define _TESTPTR_H
//
//#include <iostream>
//#include <string>
//
//typedef enum PtrStates { A_PTR = 0xFFFF,
//                 DESTROYED_PTR = 0xDCDC,
//                 UNINITED_PTR = 0xDEADC0DE,
//                 ZERO = 0};
//
//// class to watch for misuse of the pointers
//class TestPtr {
//   PtrStates m_ptr;
//   const ::std::string m_className;
//
//public:
//   TestPtr()
//   : m_ptr(UNINITED_PTR)
//   , m_className("UnIdentified")
//   {}
//
//   TestPtr(::std::string const &name)
//   : m_ptr(UNINITED_PTR)
//   , m_className(name)
//   {}
//
//   TestPtr(PtrStates rhs, ::std::string const &name)
//   : m_ptr(rhs)
//   , m_className(name)
//   {}
//
//   TestPtr(TestPtr const &rhs)
//   : m_ptr(rhs.m_ptr)
//   , m_className(rhs.m_className)
//   {
//       using ::std::cout;
//
//       if (m_ptr == UNINITED_PTR) {
//          cout << __FILE__ << '[' << __LINE__ << ']'
//               << "WARNING CONSTRUCTIONG unitialized ptr from "
//               << m_className << " unitialized ptr\n";
//       }
//       else if (m_ptr == ZERO) {
//           cout << __FILE__ << '[' << __LINE__ << ']'
//                << "WARNING! initialized from "
//                << m_className << " dead temporary.\n";
//       }
//       else if (m_ptr == DESTROYED_PTR) {
//           cout << __FILE__ << '[' << __LINE__ << ']'
//                << "ERROR! initialized from "
//                << m_className << " deleted value!\n";
//       }
//   }
//
//
//   ~TestPtr()
//   {
//       using ::std::cout;
//       if (m_ptr == UNINITED_PTR) {
//          cout << __FILE__ << '[' << __LINE__ << ']'
//               << "WARNING! DELETEING unitialized "
//               << m_className << "ptr at "
//               << this << '\n';
//       }
//       else if (m_ptr == DESTROYED_PTR) {
//          cout << __FILE__ << '[' << __LINE__ << ']'
//               << "ERROR Double delete of "
//               << m_className << " ptr at "
//               << this << '\n';
//       }
//
//       m_ptr = DESTROYED_PTR;
//   }
//
//   TestPtr &operator=(TestPtr const &rhs)
//   {
//       using ::std::cout;
//       m_ptr = rhs.m_ptr;
//       if (m_ptr == UNINITED_PTR) {
//          cout << __FILE__ << '[' << __LINE__ << ']'
//               << "WARNING assigning from "
//               << m_className << " const unitialized ptr\n";
//       }
//       else if (m_ptr == ZERO) {
//           cout << __FILE__ << '[' << __LINE__ << ']'
//                << "WARNING! assigning from "
//                << m_className << " const dead temporary.\n";
//       }
//       else if (m_ptr == DESTROYED_PTR) {
//           cout << __FILE__ << '[' << __LINE__ << ']'
//                << "ERROR! assigning from "
//                << m_className << " const deleted value!\n";
//       }
//       return *this;
//    }
//
//    TestPtr &operator=(PtrStates rhs)
//    {
//       using ::std::cout;
//       m_ptr = rhs;
//       if (m_ptr == UNINITED_PTR) {
//          cout << __FILE__ << '[' << __LINE__ << ']'
//               << "WARNING assigning from "
//               << m_className << " unitialized ptr\n";
//       }
//       else if (m_ptr == ZERO) {
//           cout << __FILE__ << '[' << __LINE__ << ']'
//                << "WARNING! assigning from "
//                << m_className << " dead temporary.\n";
//       }
//       else if (m_ptr == DESTROYED_PTR) {
//           cout << __FILE__ << '[' << __LINE__ << ']'
//                << "ERROR! assigning from "
//                << m_className << " deleted value!\n";
//       }
//       return *this;
//    }
//
//    void reset()
//    {
//        m_ptr = ZERO;
//    }
//
//    void validate() const
//    {
//       using ::std::cout;
//       if (m_ptr == UNINITED_PTR) {
//          cout << __FILE__ << '[' << __LINE__ << ']'
//               << "INVALID DATA"
//               << m_className << " unitialized ptr\n";
//       }
//       else if (m_ptr == ZERO) {
//           cout << __FILE__ << '[' << __LINE__ << ']'
//                << "INVALID DATA"
//                << m_className << " dead temporary.\n";
//       }
//       else if (m_ptr == DESTROYED_PTR) {
//           cout << __FILE__ << '[' << __LINE__ << ']'
//                << "INVALID DATA"
//                << m_className << " deleted value!\n";
//       }
//    }
//
//    void swap(TestPtr &rhs)
//    {
//        PtrStates tmp(m_ptr);
//        m_ptr = rhs.m_ptr;
//        rhs = tmp;
//    }
//};
//
//#endif
//
//// ------------------ testclass.h ------------
//
//#if !defined(_TESTCLASS_H)
//#define _TESTCLASS_H
//
//#include <algorithm>
//
//#include "testptr.h"
//#include "mojo.h"
//
//class Y : public ::mojo::enabled<Y>
//{
//    TestPtr m_ptr;
//public:
//    Y() // default constructor
//    : m_ptr("Y")
//    {}
//
//    Y(PtrStates ptr)
//    : m_ptr(ptr, "Y")
//    {}
//
//     Y(const Y& rhs) // source is a const value
//    : m_ptr(rhs.m_ptr)
//    {}
//
//    Y(::mojo::Temporary<Y> src) // source is a temporary
//        : m_ptr(src->m_ptr)
//    {
//        src->release();
//    }
//
//    Y(::mojo::fnresult<Y> src) // source is a fn result
//        : m_ptr(src->m_ptr)
//    {
//        src->release();
//    }
//
//    ~Y()
//    {}
//
//    void swap( Y&rhs)
//    {
//        using ::std::swap;
//        swap(rhs.m_ptr, m_ptr);
//    }
//
//    Y &operator=(Y const & rhs) // source is a non const lvalue
//    {
//        using ::std::swap;
//
//        Y tmp(rhs);
//        tmp.swap(*this);
//        return *this;
//    }
//
//    Y &operator=(::mojo::Temporary<Y> src) // source is a temporary
//    {
//        src->m_ptr.swap(m_ptr);
//        src->release();
//
//        return *this;
//    }
//
//    Y &operator=(::mojo::fnresult<Y> src) // source is a fn result
//    {
//        return operator=(::mojo::Temporary<Y>(src));
//    }
//
//    void validate() const
//    {
//        m_ptr.validate();
//    }
//
//private:
//    void release() {
//        m_ptr.reset();
//    }
//};
//
//// test with binary operator +
//::mojo::fnresult<Y> operator +(Y lhs, Y const &rhs) {
//    return lhs;
//}
//
//class Z : public Y
//{
//     TestPtr m_ptr;
//public:
//    Z() // default constructor
//    : m_ptr("Z")
//    {}
//
//    Z(PtrStates ptr)
//    : Y(ptr)
//    , m_ptr(ptr, "Z")
//    {}
//
//     Z(Z const& rhs)
//    : Y(rhs)// source is a const value
//    , m_ptr(rhs.m_ptr)
//    {}
//
//    Z(::mojo::Temporary<Z> src)
//    : Y(::mojo::Temporary<Y>(src) )
//    , m_ptr(src->m_ptr)             // source is a temporary
//    {
//        src->release();
//    }
//
//    Z(::mojo::fnresult<Z> src)
//    : Y(::mojo::Temporary<Y>(src.get()) )
//    , m_ptr(src->m_ptr)             // source is a fn result
//    {
//        src->release();
//    }
//
//    ~Z()
//    {}
//
//    void swap(Z &rhs)
//    {
//        using ::std::swap;
//
//        Y::swap(rhs);
//        swap(rhs.m_ptr, m_ptr);
//    }
//
//    Z &operator=(Z const &rhs) // source is a non const lvalue
//    {
//        using ::std::swap;
//
//        Z tmp(rhs);
//        tmp.swap(*this);
//
//        return *this;
//    }
//
//    Z &operator=(::mojo::Temporary<Z> src) // source is a temporary
//    {
//        using ::std::swap;
//
//        Y::operator=(::mojo::Temporary<Y>(src) );
//        src->m_ptr.swap(m_ptr);
//        src->release();
//        return *this;
//    }
//
//    Z &operator=(::mojo::fnresult<Z> src) // source is a fn result
//    {
//        return operator=(::mojo::Temporary<Z>(src));
//    }
//
//    void validate() const
//    {
//        Y::validate();
//        m_ptr.validate();
//    }
//private:
//    void release() {
//        m_ptr.reset();
//    }
//};
//
//class W : public ::mojo::enabled<W>
//{
//    TestPtr m_ptr;
//public:
//    W() // default constructor
//    : m_ptr("W")
//    {}
//
//    W(PtrStates ptr)
//    : m_ptr(ptr, "W")
//    {}
//
//     W(::mojo::fnresult<W> src) // source is a temporary
//        : m_ptr(src->m_ptr)
//    {
//        src->release();
//    }
//
//    W(::mojo::Temporary<W> src) // source is a temporary
//        : m_ptr(src->m_ptr)
//    {
//        src->release();
//    }
//
//    ~W()
//    {}
//
//    void swap( W&rhs)
//    {
//        using ::std::swap;
//        swap(rhs.m_ptr, m_ptr);
//    }
//
//    W &operator=(::mojo::Temporary<W> src) // source is a temporary
//    {
//        src->m_ptr.swap(m_ptr);
//        src->release();
//        return *this;
//    }
//
//    W &operator=(::mojo::fnresult<W> src) // source is a fnresult temporary
//    {
//        return operator=(::mojo::Temporary<W>(src) );
//    }
//
//    void validate() const
//    {
//        m_ptr.validate();
//    }
//
//private:
//    void release() {
//        m_ptr.reset();
//    }
//
//    W(const W& rhs); // No copy construction!
//    W &operator=(W const & rhs); // No assignment
//};
//
//// test with no data members.
//class X : public ::mojo::enabled<X>
//{
//public:
//    X() // default constructor
//    {}
//
//    X(::mojo::fnresult<X> src) // source is a temporary
//    {
//        src->release();
//    }
//
//    X(::mojo::Temporary<X> src) // source is a temporary
//    {
//        src->release();
//    }
//
//    ~X()
//    {}
//
//    void swap( X&rhs)
//    {}
//
//    X &operator=(::mojo::Temporary<X> src) // source is a temporary
//    {
//        src->release();
//        return *this;
//    }
//
//    X &operator=(::mojo::fnresult<X> src) // source is a fnresult temporary
//    {
//        return operator=(::mojo::Temporary<X>(src) );
//    }
//
//    void validate() const
//    {}
//
//private:
//    void release() {
//        using ::std::cout;
//    }
//
//    X(const X& rhs); // No copy construction!
//    X &operator=(X const & rhs); // No assignment
//};
//
//class V : virtual public Y, public ::mojo::enabled<V>
//{
//     TestPtr m_ptr;
//public:
//    V() // default constructor
//    : m_ptr("V")
//    {}
//
//    V(PtrStates ptr)
//    : Y(ptr)
//    , m_ptr(ptr,"V")
//    {}
//
//    V(V const& rhs)
//    : Y(rhs)// source is a const value
//    , m_ptr(rhs.m_ptr)
//    {}
//
//    V(::mojo::Temporary<V> src)
//    : Y(::mojo::Temporary<Y>(src) )
//    , m_ptr(src->m_ptr)             // source is a temporary
//    {
//        src->release();
//    }
//
//    V(::mojo::fnresult<V> src)
//    : Y(::mojo::Temporary<Y>(src.get()))
//    , m_ptr(src->m_ptr)            // source is a temporary
//    {
//        src->release();
//    }
//
//    virtual ~V()
//    {}
//
//    void swap(V &rhs)
//    {
//        using ::std::swap;
//
//        Y::swap(rhs);
//        swap(rhs.m_ptr, m_ptr);
//    }
//
//    V &operator=(V const &rhs) // source is a non const lvalue
//    {
//        V tmp(rhs);
//        tmp.swap(*this);
//
//        return *this;
//    }
//
//    V &operator=(::mojo::Temporary<V> src) // source is a temporary
//    {
//        Y::operator=(::mojo::Temporary<Y>(src) );
//        src->m_ptr.swap(m_ptr);
//        src->release();
//        return *this;
//    }
//
//    V &operator=(::mojo::fnresult<V> src) // source is a temporary
//    {
//        return operator=(::mojo::Temporary<V>(src) );
//    }
//
//    void validate() const
//    {
//        Y::validate();
//        m_ptr.validate();
//    }
//private:
//    void release() {
//        m_ptr.reset();
//    }
//};
//
//class U : virtual public Y, virtual public V, public ::mojo::enabled<U>
//{
//     TestPtr m_ptr;
//public:
//    U() // default constructor
//    : m_ptr("U")
//    {}
//
//    U(PtrStates ptr) // default constructor
//    : Y(ptr)
//    , V(ptr)
//    , m_ptr(ptr, "U")
//    {}
//
//    U(U const& rhs)
//    : Y(rhs)// source is a const value
//    , V(rhs)
//    , m_ptr(rhs.m_ptr)
//    {}
//
//    U(::mojo::Temporary<U> src)
//    : Y(::mojo::Temporary<Y>(src) )
//    , V(::mojo::Temporary<V>(src) )
//    , m_ptr(src->m_ptr)             // source is a temporary
//    {
//        src->release();
//    }
//
//    U(::mojo::fnresult<U> src)
//    : Y(::mojo::Temporary<Y>(src.get()))
//    , V(::mojo::Temporary<V>(src.get()))
//    , m_ptr(src->m_ptr)            // source is a temporary
//    {
//        src->release();
//    }
//
//    virtual ~U()
//    { }
//
//    void swap(U &rhs)
//    {
//        //Y::swap(rhs);
//        V::swap(rhs);
//        rhs.m_ptr.swap(m_ptr);
//    }
//
//    U &operator=(U const &rhs) // source is a non const lvalue
//    {
//        using ::std::swap;
//
//        U tmp(rhs);
//        tmp.swap(*this);
//
//        return *this;
//    }
//
//    U &operator=(::mojo::Temporary<U> src) // source is a temporary
//    {
//        //Y::operator=(::mojo::Temporary<Y>(src) );
//        V::operator=(::mojo::Temporary<V>(src) );
//        src->m_ptr.swap(m_ptr);
//        src->release();
//        return *this;
//    }
//
//    U &operator=(::mojo::fnresult<U> src) // source is a temporary
//    {
//        return operator=(::mojo::Temporary<U>(src) );
//    }
//
//    void validate() const
//    {
//        Y::validate();
//        V::validate();
//        m_ptr.validate();
//    }
//private:
//    void release() {
//        m_ptr.reset();
//    }
//};
//#endif
//
//// ----------------- main.cpp ----------------
//#include "mojo.h"
//#include <iostream>
//#include <vector>
//#include <cassert>
//
//#include "testclass.h"
//using namespace std;
//
//// stupid macro to print the line, and then execute it.
//#define DO(x)  cout << (#x) << "\n"; x ; cout << '\n';
//
//
//const Y MakeConstY()
//{
//    DO(return Y(A_PTR));
//}
//
//::mojo::fnresult<Y> MakeY()
//{
//    DO(Y x(A_PTR));
//    DO(return x);
//}
//
//void TakeConstY(const Y&)
//{
//}
//
//void TakeY(Y&)
//{
//}
//
//const Z MakeConstZ()
//{
//    DO(return Z(A_PTR));
//}
//
//
//::mojo::fnresult<Z> MakeZ()
//{
//    DO(Z x(A_PTR));
//    DO(return x);
//}
//
//void TakeConstZ( Z const & rhs)
//{
//}
//
//void TakeZ( Z & rhs)
//{
//}
//
//::mojo::fnresult<W> MakeW()
//{
//    DO(W x(A_PTR));
//    DO(return x);
//}
//
//::mojo::fnresult<U> MakeU()
//{
//    DO(U x(A_PTR));
//    DO(return x);
//}
//
//::mojo::fnresult<V> MakeV()
//{
//    DO(V x(A_PTR));
//    DO(return x);
//}
//
//void Discriminate(Y&) {}
//void Discriminate(mojo::Temporary<Y>) {}
//void Discriminate(mojo::constant<Y>) {}
//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////
//
//int main()
//{
//    DO(Y nonConstLValue(A_PTR));
//    DO(const Y constLValue(A_PTR));
//
//    DO(Y y1(constLValue));
//    DO(Y y2(nonConstLValue));
//    DO(Y y3(MakeConstY()));
//    DO(Y y4(MakeY()));
//
//    DO(y4.validate() );
//
//    //TakeConstY(Y());
//    //TakeConstY(MakeY());
//
//    //TakeY(Y());
//    //TakeY(MakeY());
//    {
//        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
//        DO(Discriminate(Y(A_PTR))); // calls Discriminate(mojo::Temporary<Y>)
//        DO(Discriminate(MakeY())); // calls Discriminate(mojo::Temporary<Y>)
//        DO(Discriminate(constLValue)); // calls Discriminate(mojo::constant<Y>)
//        DO(Discriminate(nonConstLValue)); // calls Discriminate(Y&)
//        cout << "<><><><><><><><><><><><><><><><><><><><>" << endl;
//    }
//    {
//        cout << "=======================================" << endl;
//        DO(y1 = constLValue);
//        DO(y2 = nonConstLValue);
//        DO(y3 = MakeConstY());
//        DO(y4 = MakeY());
//        DO(y4.validate() );
//        cout << "---------------------------------------" << endl;
//    }
//
//    #if 1
//    {
//        cout << "+++++++++++++++++++++++++++++++++++++++" << endl;
//        DO(Z nonConstZLValue(A_PTR));
//        DO(const Z constZLValue(A_PTR));
//
//        DO(Z z1(constZLValue));
//        DO(Z z2(nonConstZLValue));
//        DO(Z z3(MakeConstZ()));
//        DO(Z z4(MakeZ()));
//        DO(z4.validate() );
//        cout << "#######################################" << endl;
//
//        {
//            cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
//            DO(z1 = constZLValue);
//            DO(z2 = nonConstZLValue);
//            DO(z3 = MakeConstZ());
//            DO(z4 = MakeZ());
//            DO(z4.validate() );
//            cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
//        }
//    }
//    #endif
//
//    {
//        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
//        DO(y1 = y2 + y3);
//        DO(y1.validate() );
//
//        DO(y1 = y2 + y3 + y4);
//        DO(y1.validate() );
//
//        DO(y1 = Y(A_PTR) + y3 + y4);
//        DO(y1.validate() );
//
//        DO(y1 = MakeY() + MakeY() + MakeY());
//        DO(y1.validate() );
//        cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
//
//    }
//
//
//    {
//        cout << "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT" << endl;
//        DO(Y y1(A_PTR));
//        DO( TakeY(y1) );
//        DO(TakeConstY(Y(A_PTR) ));
//        DO(Z z1(A_PTR));
//        DO(TakeZ( z1 ));
//        DO(TakeConstZ(Z(A_PTR) ));
//        cout << "000000000000000000000000000000000000000" << endl;
//    }
//    cout << "sizeof(Y) = " << sizeof (Y) << '\n';
//    cout << "sizeof(Z) = " << sizeof (Z) << '\n';
//    cout << "sizeof(TestPtr) = " << sizeof (TestPtr) << '\n';
//    {
//        DO(W w1(A_PTR));
//        DO(W w2(MakeW())); // ok construction from temporary.
//        DO(w1 = MakeW()); // ok assignment from fnresult temporary.
//        //DO(W w3(w1)); // fails to link.
//        //DO(w1 = w2); // fails to link.
//    }
//    {
//        cout << "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG" << endl;
//        typedef ::std::vector<Y> Y_Vec_t;
//
//        DO(Y_Vec_t y_vec);
//
//        DO(y_vec.push_back(MakeY() ));
//
//        DO(y_vec[0].validate());
//        cout << "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH" << endl;
//    }
//
//    {
//        cout << "IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII" << endl;
//
//        DO( V v1(A_PTR));
//        DO( v1 = MakeV());
//        DO( V v2(MakeV()));
//        cout << "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR" << endl;
//    }
//    {
//        cout << "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << endl;
//
//        DO( U u1(A_PTR));
//        DO( u1 = MakeU());
//        DO( U u2(MakeU()));
//
//        cout << "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR" << endl;
//    }
//
//    DO(cout << sizeof(Y) << " ");
//    DO(cout << sizeof(Z) << " ");
//    DO(cout << sizeof(W) << " ");
//    DO(cout << sizeof(X) << " ");
//    DO(cout << sizeof(TestPtr) << "\n");
//}