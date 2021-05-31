#include <source_location>
#include <iostream>

using SourceLoc = std::source_location;

struct NewDebugStream
{
  struct Annotated
  {
    /*IMPLICIT*/ Annotated(NewDebugStream &s,
      SourceLoc loc = SourceLoc::current())
    {
      *this << loc.file_name() << ":" << loc.line() << ": ";
    }
  };
  template<class T>
  friend Annotated operator<<(Annotated a, T msg)
  {
    std::cout << msg;
    return a;
  }
};
NewDebugStream nds;

//int main()
//{
//  nds << "Hello world! " << 42 << "\n";
//}