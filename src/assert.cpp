#define DEBUG
#include "loki_ext/asserter.h"
#include <iostream>

int main()
{
  try {
    ASSERT(0);
    std::cout << "\n test 0\n";
    ASSERT<std::runtime_error>(0, "world soccer");
    std::cout << "\ntest  A\n";
  } catch (std::exception &e) {
    std::cout << "\n test C\n";
    std::cout << e.what() << std::endl;
  }
}