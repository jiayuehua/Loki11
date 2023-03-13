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

#define USE_SEQUENCE

#include <iostream>
#include <string>
#include <loki/Factory.h>


using namespace Loki;
using std::cout;
using std::endl;


////////////////////////////////////////////
// Object to create: Product
// Constructor with 0 and 2 arguments
////////////////////////////////////////////

class AbstractProduct
{
public:
  virtual int get_x() const = 0;
  virtual int get_y() const = 0;
  virtual ~AbstractProduct() = default;
};

class Product : public AbstractProduct
{
  int x = 0;
  int y = 0;

public:
  Product() = default;
  Product(int xa, int ya) : x(xa), y(ya) {}
  int get_x() const
  {
    return x;
  }
  int get_y() const
  {
    return y;
  }
};

///////////////////////////////////////////////////////////
// Factory for creation a Product object without parameters
///////////////////////////////////////////////////////////

typedef Factory<int, DefaultFactoryError, AbstractProduct>
  PFactoryNull;

/////////////////////////////////////////////////////////////
// Factory for creation a Product object with 2 parameters
/////////////////////////////////////////////////////////////

typedef Factory<int, DefaultFactoryError, AbstractProduct, int, int>
  PFactory;

////////////////////////////////////////////////////
// Creator functions with different names
////////////////////////////////////////////////////

Product *createProductNull()
{
  cout << "createProductNull()" << endl;
  return new Product;
}
Product *createProductParm(int a, int b)
{
  cout << "createProductParm( int a, int b ) " << endl;
  return new Product(a, b);
}

///////////////////////////////////////////////////
// Overloaded creator functions
///////////////////////////////////////////////////
Product *createProductNullOver()
{
  cout << "createProductOver()" << endl;
  return new Product;
}
Product *createProductParmOver(int a, int b)
{
  cout << "createProductOver( int a, int b )" << endl;
  return new Product(a, b);
}


void testFactoryNull()
{
  PFactoryNull pFactoryNull;
  bool const ok1 = pFactoryNull.Register(1, createProductNull);
  if (ok1) {
    std::unique_ptr<AbstractProduct> pObject(pFactoryNull.CreateObject(1));
    if (pObject.get()) {
      cout << "pObject->get_x() = " << pObject->get_x() << endl;
      cout << "pObject->get_y() = " << pObject->get_y() << endl;
    }
  }
}

void testFactoryBinary()
{
  PFactory pFactoryNull;
  bool const ok1 = pFactoryNull.Register(1, createProductParm);
  if (ok1) {
    std::unique_ptr<AbstractProduct> pObject(pFactoryNull.CreateObject(1, 5, 5));
    if (pObject) {
      cout << "pObject->get_x() = " << pObject->get_x() << endl;
      cout << "pObject->get_y() = " << pObject->get_y() << endl;
    }
  }
}

int main()
{
  testFactoryNull();
  testFactoryBinary();
}
