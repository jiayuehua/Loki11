#include <iostream>
#include <tuple>
#include "loki_ext/adhocvistor.h"
#include "loki/Factory.h"
//#include "loki/Sequence.h"
int *create(int a)
{
  return new int(a);
}
void factory()
{
  Loki::Factory<int, int, std::tuple<int>> f;
  int *p = f.CreateObject(1, 1);
  std::cout << *p << std::endl;
}

int *createInt(const int *)
{
  return new int{ 1 };
}
struct SInfo
{
};

void clonefactory()
{
  Loki::CloneFactory<int> f;
  f.Register<int>(createInt);
  f.Register<SInfo>(createInt);
  f.Unregister<int>();
  int q = 1;
  int *pa = &q;
  int *p = f.CreateObject(pa);
  std::cout << *p << std::endl;
}


struct DocElement
{
  virtual ~DocElement() = default;
  std::string name() const
  {
    return "DocElementEx";
  }
};
struct TextArea : public DocElement
{
  std::string name() const
  {
    return "TextAreaEx";
  }
};
struct VectorGraphics : public DocElement
{
  std::string name() const
  {
    return "VectorGraphicsEx";
  }
};

struct Bitmap : public DocElement
{
  std::string name() const
  {
    return "BitmapEx";
  }
};
template<class T>
class EffectorEx
{
public:
  void Visit(T *t)
  {
    std::cout << t->name() << std::endl;
  }
};

class Effector
{
public:
  void Visit(DocElement *)
  {
    std::cout << "DocElement\n";
  }
  void Visit(TextArea *)
  {
    std::cout << "TextArea\n";
  }
  void Visit(VectorGraphics *)
  {
    std::cout << "VectorGraphics\n";
  }
  void Visit(Bitmap *)
  {
    std::cout << "Bitmap\n";
  }
};

//struct ConcreteVisitor : Loki::MyAdHocVisitor<TextArea, VectorGraphics, Bitmap>
//{
//  void Visit(TextArea *)
//  {
//    std::cout << "TextArea\n";
//  }
//  void Visit(VectorGraphics *)
//  {
//    std::cout << "VectorGraphics\n";
//  }
//  void Visit(Bitmap *)
//  {
//    std::cout << "Bitmap\n";
//  }
//};
int main()
{
  Loki::FlexAdHocVisitor<Effector, DocElement, Bitmap, VectorGraphics, TextArea> v;

  Loki::FlexAdHocVisitorEx<EffectorEx, DocElement, Bitmap, VectorGraphics, TextArea> vex;

  TextArea ta;
  v.StartVisit(&ta);
  vex.StartVisit(&ta);


  //factory();


  //Bitmap b;
  //DocElement *p = &b;
  //ConcreteVisitor visitor;
  //visitor.StartVisit(p);
}
