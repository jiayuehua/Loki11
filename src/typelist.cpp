#define DEBUG
#include <boost/mp11.hpp>
#include <type_traits>
namespace mp = boost::mp11;
struct Base
{
};
struct La : Base
{
};
struct Lb : La
{
};
struct Lc : La
{
};
struct Ra : Base
{
};
struct Rb : Ra
{
};
int main()
{

  typedef mp::mp_list<
    Base,
    Ra,
    Lb,
    La,
    Rb,
    Lc>
    ArithTypesImpl;
  using ArithTypes2 = mp::mp_sort<ArithTypesImpl, mp::mp_not_fn<std::is_base_of>::fn>;
  static_assert(std::is_same_v<mp::mp_list<Base>, ArithTypes2>);
}