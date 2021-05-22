////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Peter Kümmel
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id: main.cpp 898 2008-08-09 15:35:12Z syntheticpp $


// define to test the OrderedStatic template

// define to see a runtime crash when not using OrderedStatic
//#define LOKI_CLASS_LEVEL_THREADING

#include <iostream>

#include <loki/OrderedStatic.h>

struct L1
{
  L1() { std::cout << "create L1: " << this << "\n"; }
  ~L1() { std::cout << "delete L1: " << this << " \n"; }
};

struct L2
{
  L2() { std::cout << "create L2 \n"; }
  ~L2() { std::cout << "delete L2 \n"; }
};

struct M1
{
  M1() { std::cout << "create M1 \n"; }
  ~M1() { std::cout << "delete M1 \n"; }
};

struct M2
{
  M2() { std::cout << "create M2 \n"; }
  ~M2() { std::cout << "delete M2 \n"; }
};

int f()
{
  std::cout << "f called \n";
  return 0;
}

std::string func();


//struct MemberTest
//{
//    static Loki::OrderedStatic<1,M1> m1;
//    static Loki::OrderedStatic<2,M2> m2;
//};
//Loki::OrderedStatic<1,M1> MemberTest::m1;
//Loki::OrderedStatic<2,M2> MemberTest::m2;
//

Loki::OrderedStatic<1, Loki::NewCreatePolicy, L1> l1;
Loki::OrderedStatic<2, Loki::NewCreatePolicy, L2> l2;

//Loki::OrderedStatic<1, std::string, std::string(*)() >            s1( &func );
//Loki::OrderedStatic<2, std::string, Loki::Seq<const char *> >    s2( "s2" );
//
//Loki::OrderedStatic<1, Loki::Functor<int>, Loki::Seq<int(*)()> >  f1(f);


int main()
{


  Loki::OrderedStaticManager::Instance().createObjects();


  return 0;
}
