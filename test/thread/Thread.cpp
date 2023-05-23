////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 by Peter Kuemmel
//
// Code covered by the MIT License
// The authors make no representations about the suitability of this software
// for any purpose. It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id: Factory.cpp 823 2007-05-08 10:48:40Z lfittl $


// #define CLASS_LEVEL_THERADING


#include <iostream>
#include <string>
#include <loki/ThreadModel.h>
#include <mutex>
#include <thread>


using namespace Loki;
using std::cout;
using std::endl;


////////////////////////////////////////////
// Object to create: Product
// Constructor with 0 and 2 arguments
////////////////////////////////////////////

struct ObjLock : public Loki::ObjectLevelLockable<ObjLock>
{
  void foo() const
  {
    Lock l(this);
    for (int i = 0; i < 5; ++i) {
      cout << std::this_thread::get_id() << endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      cout << " ObjLock::foo() " << endl;
    }
  }
};

struct MyObjLock : public Loki::ClassLevelLockable<MyObjLock>
{
  void foo() const
  {
    Lock l(this);
    for (int i = 0; i < 5; ++i) {
      cout << std::this_thread::get_id() << endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      cout << " ObjLock::foo() " << endl;
    }
  }
};
int main()
{
  //ObjLock o;
  //o.foo();
  //std::jthread j(
  //  [po = &o] {
  //    po->foo();
  //  });
  MyObjLock a;
  std::jthread j(
    [] {
  MyObjLock a;
      a.foo();
    });
  a.foo();
}
