#include "arrow_proxy.h"
#include <cassert>
#include <utility>
#include <vector>

template<class KeyIterator, class ValueIterator>
class zip_iterator
{
  KeyIterator kit_;
  ValueIterator vit_;

public:
  using KeyType = typename KeyIterator::value_type;
  using ValueType = typename ValueIterator::value_type;
  using KeyReference = typename KeyIterator::reference;
  using ValueReference = typename ValueIterator::reference;

  using iterator_category = std::common_type_t<
    typename std::iterator_traits<KeyIterator>::iterator_category,
    typename std::iterator_traits<ValueIterator>::iterator_category>;
  using difference_type = std::ptrdiff_t;
  using value_type = std::pair<KeyType, ValueType>;
  using reference = std::pair<KeyReference, ValueReference>;
  using pointer = arrow_proxy<reference>;

  explicit zip_iterator(KeyIterator k, ValueIterator v) : kit_(k), vit_(v) {}

  reference operator*() const { return { *kit_, *vit_ }; }

  pointer operator->() const
  {
    return pointer{ { *kit_, *vit_ } };
  }
};

void check(std::vector<int> &a, std::vector<bool> &b)
{
  zip_iterator zit(a.begin(), b.begin());
  (*zit).first = 42;
  zit->second = true;
}
#include <iostream>
template<class KeyIterator>
class iteratorWrapper
{
  KeyIterator kit_;

public:
  using KeyType = typename KeyIterator::value_type;
  using KeyReference = typename KeyIterator::reference;

  using iterator_category =
    typename std::iterator_traits<KeyIterator>::iterator_category;
  using difference_type = std::ptrdiff_t;
  using value_type = KeyType;
  using reference = KeyReference;
  using pointer = arrow_proxy<reference>;

  explicit iteratorWrapper(KeyIterator k) : kit_(k)
  {
  }

  reference operator*() const { return { *kit_ }; }

  pointer operator->() const
  {
    return pointer{ *kit_ };
  }
};
