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
#include <memory>
#include <loki/AbstractFactory.h>


using namespace Loki;
using std::cout;
using std::endl;

struct Soldier
{
public:
  virtual ~Soldier(){};

  virtual void print() const {
  
    cout << "Soldier\n";
  }
};

struct SillySoldier:public Soldier
{
public:
  virtual void print() const {
    cout << "SillySoldier\n";
  }
};

struct Monster
{
public:
  virtual ~Monster() =default;
  virtual void print() const = 0;
};

struct SillyMonster:public Monster
{
public:
  virtual void print() const {
    cout << "SillyMonster\n";
  }
};
using AbstractEnemyFactory=AbstractFactory<Soldier, Monster>;

using ConcreteEnemyFactory= ConcreteFactory<AbstractEnemyFactory, OpNewFactoryUnit,mp::mp_list<SillySoldier,SillyMonster>> ;
int main() {
  ConcreteEnemyFactory cef;
  AbstractEnemyFactory *pA = &cef;
  auto pS = pA->Create<Soldier>();
  std::unique_ptr<Soldier> p(pS);
  p->print();
}

