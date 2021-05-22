#include <vector>
#include <iostream>
#include "loki/MultiMethods.h"
struct LB
{
  virtual ~LB(){};
};

struct LD : LB
{
};
struct RB
{
  virtual ~RB(){};
};

struct RD : RB
{
};
struct Excutor
{

  void operator()(LB &l, RB &r)
  {
    std::cout << "LBError,RBError\n";
  }
  void operator()(LB &l, RD &r)
  {
    std::cout << "LB,RD\n";
  }
  void operator()(LD &l, RB &r)
  {
    std::cout << "LD,RB\n";
  }
  void operator()(LD &l, RD &r)
  {
    std::cout << "LD,RD\n";
  }
  void OnError(LB &l, RB &r)
  {
    std::cout << "OnError\n";
  }
};
//void fbb(LB &l, RB &r)
//{
//  std::cout << "LB,RB\n";
//}
//void fbd(LB &l, RD &r)
//{
//  std::cout << "LB,RD\n";
//}
//void fdb(LD &l, RB &r)
//{
//  std::cout << "LD,RB\n";
//}
//void fdd(LD &l, RD &r)
//{
//  std::cout << "LD,RD\n";
//}
int main()
{
  {
    LD ld;
    RD rd;
    Excutor f;
    Loki::StaticDispatcher<Excutor, LB, mp::mp_list<LB, LD>, RB, mp::mp_list<RB, RD>> multimethod;
    multimethod.invoke(f, ld, rd);
  }
  //{
  //  Loki::FnDispatcher<LB, RB> m;
  //  m.Add<&fbb>();
  //  m.Add<&fbd>();
  //  m.Add<&fdb>();
  //  m.Add<&fdd>();
  //  LD ld;
  //  RD rd;
  //  m(ld, rd);
  //  LB lb;
  //  m(lb, rd);
  //}
  //{
  //  Loki::FunctorDispatcher<LB, RB> m;
  //  auto fbb = [](LB &l, RB &r) {
  //    std::cout << "LB,RB\n";
  //  };
  //  auto fbd = [](LB &l, RD &r) {
  //    std::cout << "LB,RD\n";
  //  };
  //  auto fdb = [](LD &l, RB &r) {
  //    std::cout << "LD,RB\n";
  //  };
  //  auto fdd = [](LD &l, RD &r) {
  //    std::cout << "LD,RD\n";
  //  };
  //  m.Add(fbb);
  //  m.Add(fbd);
  //  m.Add(fdb);
  //  m.Add(fdd);
  //  LD ld;
  //  RD rd;
  //  m(ld, rd);
  //  LB lb;
  //  m(lb, rd);
  //}
}
