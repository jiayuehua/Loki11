#include <windows.h>
#include "auto_any.h"// return an auto_hfile by value
// a generic close policy that uses a ptr to a function
template<typename Fn, Fn Pfn>
struct close_fun
{
  template<typename T>
  static void close(T t) { Pfn(t); }
};// close_handle calls the CloseHandle() API
typedef close_fun<BOOL(__stdcall *)(HANDLE), CloseHandle> close_handle;
template<typename T, class close_policy, class invalid_value = null_t, int unique = 0>
class auto_any;
template<typename T, class close_policy, class invalid_value = null_t, int unique = 0>
class shared_any;
template<typename T, class close_policy, class invalid_value = null_t, int unique = 0>
class scoped_any;
// value_const is a general invalid_value policy that evaluates to a
// literal constant.
template<typename T, T value = T(0)>
struct value_const
{
  operator T const() const { return value; }
};
typedef value_const<HANDLE, INVALID_HANDLE_VALUE> invalid_handle_t;
auto_hfile open_file(char const *szFileName)
{
  // auto_hfile is a typedef for
  // auto_any<HANDLE,close_handle,invalid_handle_t>
  return auto_hfile(CreateFile(szFileName, /*...*/));
}
int main()
{
  auto_hfile hFile = open_file("my_file.txt");
  if (hFile) {// File open succeeded, write something to it
    WriteFile(get(hFile), /*...*/);
  }// File is closed automatically when hFile is destructed
  return 0;
}
#using < mscorlib.dll>
#include <windows.h>
#include "auto_any.h"
__gc struct MyObject : System::Object
  , System::IDisposable
{
  virtual ~MyObject() { System::Console::WriteLine(S "~MyObject!!!"); }
  virtual void Dispose() { System::Console::WriteLine(S "Dispose!!!"); }
  void Hello() { System::Console::WriteLine(S "Hello, world!"); }
};
int main()
{// This object is automatically disposed
  auto_any<MyObject *, close_dispose> pO1(new MyObject);// This object is automatically deleted
  auto_any<MyObject *, close_delete> pO2(new MyObject);// Just like any smart pointer, this does what you expect
  pO2->Hello();
  return 0;
}