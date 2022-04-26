//#include <iostream>
//#include <vector>
//#include <loki/SmartPtr.h>
//#include <mutex>
//struct S
//{
//  std::mutex m;
//  int i = 5;
//  void Lock()
//  {
//    std::cout << "lock\n";
//    m.lock();
//  }
//  void Unlock()
//  {
//    std::cout << "unlock\n";
//    m.unlock();
//  }
//};
//
//typedef Loki::SmartPtr<int, Loki::DestructiveCopy, Loki::AssertCheck, Loki::DefaultSPStorage, Loki::PropagateConst> SPtr;
//int main()
//{
//
//  SPtr s(new int{ 5 });
//  if (s) {
//    std::cout << *s << std::endl;
//    //static_assert(std::is_same_v<decltype(*s), const int &>);
//  }
//  {
//    SPtr sb(new int{ 5 });
//    if (sb) {}
//    //  SPtr sc(sb);
//    // if (sc) {}
//    std::cout << "B \n";
//    Loki::DestructiveCopy<int> d;
//    ///  Loki::DestructiveCopy<int> db(d);
//  }
//  {
//    Loki::SmartPtr<S, Loki::RefLinked, Loki::AssertCheck> sb(new S);
//    if (sb) {
//      S *s = GetImpl(sb);
//      std::cout << sb->i << std::endl;
//      // std::cout << (*sb).i << std::endl;
//    }
//  }
//}
