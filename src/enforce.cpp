#define DEBUG
#include "loki_ext/enforce.h"
#include "loki_ext/yasli_find.h"
#include <iostream>
#include <vector>

void test_enforce()
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

int main()
{
  std::vector<int> v{ 1, 2, 3 };
  int d = 3;

  auto i = yasli::find(std::begin(v), std::end(v), d);
  std::cout << *i << std::endl;
}
