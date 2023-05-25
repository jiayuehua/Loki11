#include <iostream>
#include <string>
#include <loki/SmartPtr.h>
#include <mutex>
#include <thread>


using namespace Loki;
using std::cout;
using std::endl;

struct Lock
{
public:
  std::mutex m_;
  int i_;
  void lock()
  {
    cout << "lock\n";
    m_.lock();
  }
  void unlock() {
    cout << "unlock\n";
    m_.unlock();
  }
  void set(int i)
  {
    i_=i;
  }
  int get() const {
    return i_;
  }
};

int main()
{
  {
    SmartPtr<int, Loki::RefCountedAtomic> s(new int(3));
    auto s2 = s;
    cout << *s << std::endl;
    cout << *s2 << std::endl;
  }
  {
    SmartPtr<int, Unique> s(new int(3));
    auto s2 = std::move(s);
    cout << *s2 << std::endl;
    if (!s) {
      cout << "source moved!" << std::endl;
    }
  }
  {
    SmartPtr<int, NoCopy> s(new int(3));
    if (!s) {
      cout << "source moved!" << std::endl;
    }
  }
  {
    SmartPtr<int, DeepCopy> s(new int(3));
    auto b(s);
    if (b) {
      cout << "source copyed!" << *b << std::endl;
    }
  }
  {
    struct B
    {
      virtual B *Clone() const = 0;
      virtual ~B() = default;
    };
    struct D : B
    {
      virtual D *Clone() const
      {
        cout << "cloned\n";
        return new D(*this);
      }
    };
    SmartPtr<B, DeepCopy> s(new D);
    auto d(s);
  }
  {
    struct B
    {
      B()
      {
        cout << "B()\n";
      }
      ~B()
      {
        cout << "~B()\n";
      }
      B(const B &)
      {
        cout << "B(const B)\n";
      }
      B &operator=(const B &)
      {
        cout << "B(const B)\n";
        return *this;
      }
    };
    SmartPtr<B, RefLinked> s(new B);
    {

      cout << "==B==\n";
      auto d(s);
    }
    cout << "==C==\n";
  }
  {
    SmartPtr<int, RefLinked, AssertCheck> s(nullptr);
    {
    }
  }
  {
    SmartPtr<int, RefLinked, AssertCheck, ArrayStorage> s(new int[3]{1,2,3});
    cout << "array"<<s[2] << endl;
  }
  {
    SmartPtr<Lock, RefLinked, AssertCheck, LockedStorage> s(new Lock );
    cout << "before deref\n";
    s->i_=3;
    cout << "after deref\n";
  }
  {
    const SmartPtr<int, RefLinked, AssertCheck,DefaultSPStorage> s(new int{1});
    cout << "DontPropagateConst"<<*s << endl;
    *s=4;
  }
  {
    const SmartPtr<int, RefLinked, AssertCheck,DefaultSPStorage,PropagateConst> s(new int{1});
    cout << "PropagateConst"<<*s << endl;
    cout<<*s<<endl;
  }
  {
    SmartPtr<int, RefLinked, AssertCheck,DefaultSPStorage> s(new int{1});
    SmartPtr<int, RefLinked, AssertCheck,DefaultSPStorage> sb(new int{1});
    cout << "DontPropagateConst"<<(s==sb) << endl;
    cout << "DontPropagateConst"<<(s<sb) << endl;
  }
  {
    SmartPtr<int, RefLinked, AssertCheck,DefaultSPStorage> s(new int{1});
    SmartPtr<int, RefLinked, AssertCheck,DefaultSPStorage> sb(new int{1});
    cout << "DontPropagateConst"<<(!s) << endl;
    cout << "DontPropagateConst"<<GetPointer(s) << endl;
  }
}
