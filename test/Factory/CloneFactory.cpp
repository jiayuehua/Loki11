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

class Shape
{
public:
  virtual void draw() = 0;
  virtual ~Shape() = default;
};

class Circle : public Shape
{

public:
  virtual void draw() { cout << "Circle" << endl; }
};

class Square : public Shape
{

public:
  virtual void draw() { cout << "Square" << endl; }
};

int main()
{
  using CF= CloneFactory<Shape>;
  CF cf;
  cf.Register<Square>();
  cf.Register<Circle>();
  Square s;
  auto r = cf.CreateObject(&s);
  r->draw();
  Circle c;
  auto rc = cf.CreateObject(&c);
  rc->draw();
}
