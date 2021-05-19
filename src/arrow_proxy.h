#pragma once
#include <memory>
template<class Reference>
struct arrow_proxy
{
  Reference r;
  Reference *operator->()
  {
    return std::addressof(r);
  }
};