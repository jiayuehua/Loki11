#include <functional>
#include <iostream>
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
  int nb = 200;
  auto i = std::ref(n);
  i.get() = 50;
  ++i;
  std::cout << n << "\n";
  i = nb;
  ++i;

  std::cout << n << "\n";
  std::cout << nb << "\n";
  auto k = std::ref(bar);
  k();


  return 0;
  std::cout << "---origin--\n";
  foo(a);
  std::cout << "-----ref---\n";
  foo(std::ref(a));
}
