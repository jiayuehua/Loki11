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
#include <loki/MultiMethods.h>
struct First
{
  virtual ~First() = default;
};
struct Second : public First
{
};
struct Snd : public First
{
};
struct Executor
{
  void operator()(First, First) const
  {
    std::cout << "first first";
    std::cout << std::endl;
  }
  void operator()(First, Second) const
  {
    std::cout << "first second";
    std::cout << std::endl;
  }
  void operator()(First, Snd) const
  {
    std::cout << "first snd";
    std::cout << std::endl;
  }
  void operator()(Second, First) const
  {
    std::cout << "Second first";
    std::cout << std::endl;
  }
  void operator()(Second, Second) const
  {
    std::cout << "Second second";
    std::cout << std::endl;
  }
  void operator()(Second, Snd) const
  {
    std::cout << "Second snd";
    std::cout << std::endl;
  }
  void operator()(Snd, First) const
  {
    std::cout << "Snd first";
    std::cout << std::endl;
  }
  void operator()(Snd, Second) const
  {
    std::cout << "Snd second";
    std::cout << std::endl;
  }
  void operator()(Snd, Snd) const
  {
    std::cout << "Snd snd";
    std::cout << std::endl;
  }
  void OnError(First, First) const
  {
    std::cout << " On error";
    std::cout << std::endl;
  }
};
void firstfirst(First &, First &)
{
  std::cout << "first first";
  std::cout << std::endl;
}
void firstsecond(First &, Second &)
{
  std::cout << "first second";
  std::cout << std::endl;
}
void secondfirst(Second &, First &)
{
  std::cout << "Second first";
  std::cout << std::endl;
}
void secondsecond(Second &, Second &)
{
  std::cout << "Second second";
  std::cout << std::endl;
}
namespace mp = boost::mp11;
int main()
{
  {
    Loki::StaticDispatcher<Executor, mp::mp_list<First, Second, Snd>, mp::mp_list<First, Second, Snd>> dis;
    Executor e;
    First f;
    Second s;
    Snd snd;
    dis.invoke(e, f, f);
    dis.invoke(e, s, f);
    dis.invoke(e, snd, f);
    dis.invoke(e, f, s);
    dis.invoke(e, s, s);
    dis.invoke(e, snd, s);
    dis.invoke(e, f, snd);
    dis.invoke(e, s, snd);
    dis.invoke(e, snd, snd);
  }
  {
    First f;
    Second s;
    Loki::FnDispatcher<First> dis;
    dis.Add<&firstfirst>();
    dis.Add<&firstsecond>();
    dis.Add<&secondsecond>();
    dis.Add<&secondfirst>();
    dis(f, f);
    dis(f, s);
    dis(s, f);
    dis(s, s);
  }
  {
    auto firstfirst = [](First &, First &) {
      std::cout << "first first";
      std::cout << std::endl;
    };
    auto firstsecond = [](First &, Second &) {
      std::cout << "first second";
      std::cout << std::endl;
    };
    auto secondfirst = [](Second &, First &) {
      std::cout << "Second first";
      std::cout << std::endl;
    };
    Loki::FunctorDispatcher<First> func;
    func.Add(firstfirst);
    func.Add<true>(firstsecond);
    func.Add(&secondsecond);
    First f;
    Second s;
    func(f,f);
    func(f,s);
    func(s,f);
    func(s,s);
  }
}
