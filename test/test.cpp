

  struct MediumLarge {
    char * data_;
    size_t size_;
    size_t capacity_;

    size_t capacity() const {
      return capacity_ & 1;
    }
  };
#include <iostream>
  int main()
  {
	  std::cout<<sizeof(MediumLarge)<<std::endl;
  }