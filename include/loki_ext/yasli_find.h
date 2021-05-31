#include <type_traits>
#include <iterator>
#include "priority_tag.hpp"
namespace Achilles {

template<class Iter, class ValueType>
Iter conservative_find(Iter from, const Iter to, const ValueType &val)
{
  for (; from != to && !(*from == val); ++from) {
  }
  return from;
}

template<class Iter, class ValueType>
Iter mutative_find(Iter b, Iter e, const ValueType &v)
{
  using std::swap;
  if (b == e) return e;
  typename std::iterator_traits<Iter>::value_type tmp(v);
  --e;
  swap(*e, tmp);
  for (; !(*b == v); ++b) {
  }
  swap(*e, tmp);
  if (b == e && !(v == *b)) ++b;
  return b;
}

template<class Iter, class ValueType>
Iter duff_find(Iter b, const Iter e, const ValueType &v)
{
  switch ((e - b) & 7u) {
  case 0:
    while (b != e) {
      if (*b == v) return b;
      ++b;
    case 7:
      if (*b == v) return b;
      ++b;
    case 6:
      if (*b == v) return b;
      ++b;
    case 5:
      if (*b == v) return b;
      ++b;
    case 4:
      if (*b == v) return b;
      ++b;
    case 3:
      if (*b == v) return b;
      ++b;
    case 2:
      if (*b == v) return b;
      ++b;
    case 1:
      if (*b == v) return b;
      ++b;
    }
  }
  return b;
}

template<class Iter, class ValueType>
Iter find_impl(Iter from, const Iter to, const ValueType &val, priority_tag<0>)
{
  //... conservative find implementation ...
  return conservative_find(from, to, val);
}

template<class Iter, class ValueType>
Iter find_impl(Iter from, const Iter to, const ValueType &val, priority_tag<1>)
{
  return duff_find(from, to, val);
}

template<class Iter, class ValueType>
Iter find_impl(Iter from, Iter to, const ValueType &val, priority_tag<2>)
{
  return mutative_find(from, to, val);
}
template<class RanIter, unsigned int size>
struct BinaryFinder;

template<class RanIter>
struct BinaryFinder<RanIter, 1>
{
  typedef typename std::iterator_traits<RanIter>::value_type
    ValueType;
  static RanIter Run(RanIter begin, const ValueType &v)
  {
    //if (!(v < *begin)) ++begin;
    if ((v > *begin)) ++begin;
    return begin;
  }
};

template<class RanIter, unsigned int size>
struct BinaryFinder
{
  typedef typename std::iterator_traits<RanIter>::value_type
    ValueType;
  static RanIter Run(RanIter begin, const ValueType &v)
  {
    const RanIter mid = begin + (size / 2);
    if (v < *mid) {
      return BinaryFinder<RanIter,
        size / 2>::Run(begin, v);
    }
    return BinaryFinder<RanIter,
      size - size / 2>::Run(mid, v);
  }
};

}// namespace Achilles

namespace yasli {
template<class Iter, class ValType>
Iter find(Iter from, Iter to, const ValType &val)
{
  using namespace std;
  typedef typename iterator_traits<Iter>::iterator_category Category;
  enum { isBidir = std::is_same<
           Category,
           bidirectional_iterator_tag>::value };
  enum { isRand = std::is_same<
           Category,
           random_access_iterator_tag>::value };
  typedef typename iterator_traits<Iter>::pointer PointerType;
  typedef typename std::remove_pointer<PointerType>::type
    IteratedType;
  enum { isMutableSeq = !std::is_const<
           IteratedType>::value };
  typedef typename iterator_traits<Iter>::value_type ValueType;
  enum { isPod = std::is_pod<ValueType>::value };
  enum { selector =
           (isBidir || isRand) && isPod && isMutableSeq ? 2 : (isRand ? 1 : 0) };
  priority_tag<selector> sel;
  return Achilles::find_impl(from, to, val, sel);
}

template<unsigned size, class RanIter>
RanIter binary_find(RanIter begin, const typename std::iterator_traits<RanIter>::value_type &v)
{
  const RanIter mid = begin + (size / 2);
  if (v < *mid) {
    return Achilles::BinaryFinder<RanIter,
      size / 2>::Run(begin, v);
  }
  return Achilles::BinaryFinder<RanIter,
    size - size / 2>::Run(mid, v);
}

}// namespace yasli
