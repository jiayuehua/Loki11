//#include "fold_wrapper.hpp"
//#include <tuple>
//#include <utility>
//#include <iostream>
//int main()
//{
//  std::tuple<int> a{ 1 };
//  std::tuple<float> b{ 1.2 };
//  auto r = fold_left([](auto &&x, auto &&y) {
//    return std::tuple_cat(std::forward<decltype(x)>(x),
//      std::forward<decltype(y)>(y));
//  },
//    a,
//    b);
//  std::cout << std::get<1>(r) << std::endl;
//}
#include <functional>
#include <iostream>
#include <concepts>
void iCon(std::integral auto i)
{
  std::cout << "i\n";
  ++i;
}
//void iCon(std::signed_integral auto i)
//{
//  std::cout << "si\n";
//  ++i;
//}
void iCon(std::unsigned_integral auto i)
{
  std::cout << "ui\n";
  ++i;
}
class A
{
public:
  A() {}
  A(const A &)
  {
    std::cout << "Copy construct \n";
  }
};


template<class T>
void foo(T t)
{
}
void bar()
{
  std::cout << "call bar"
            << "\n";
}
int main()
{
  A a;
  int n = 100;
  iCon(n);
  iCon(10u);
  int nb = 200;
  auto i = std::ref(n);
  i.get() = 50;
  ++i;
  std::cout << n << "\n";
  i = nb;
  ++i;

  std::cout << n << "\n";
  std::cout << nb << "\n";
  //i();
  auto k = std::ref(bar);
  k();


  return 0;
  std::cout << "---origin--\n";
  foo(a);
  std::cout << "-----ref---\n";
  foo(std::ref(a));
}
