#define DEBUG
#include "loki_ext/enforce.h"
#include <iostream>

int main()
{
  try {
    ENFORCE(0 == 1)
    ("fail enforce");
    std::pair<int, int> a{ 1, 1 };
    auto *p = &a;
    std::pair<int, int> *pb = ENFORCE(p);
    pb->second = 2;
    //   ENFORCE(p)->first = 2;

    std::cout << "\ntest  A\n";
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}