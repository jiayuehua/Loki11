# Loki11 lib
update andrei's loki lib to C++11
主要改进
- 接口改为使用variadic templates，不再是typelist，实现中用到typelist的地方改为使用boost mp11库的mp_list
- typelist算法改为使用mp11的算法。
- 将typetraits部分改为直接使用C++11的type_traits
- 将functor删除，改为使用C++11的std::function
- 将TypeInfo，改为使用boost::typeindex::type_index

简要对比loki11和loki
以factory为例说明，Loki11工厂创建抽象对象只有一个成员模板

    template<class ID, class... Arg>
    std::unique_ptr<AbstractProduct> CreateObject(const ID &id, Arg &&...arg)

 而loki的factory有许多成员

    AbstractProduct* CreateObject(const IdentifierType& id)
    AbstractProduct* CreateObject(const IdentifierType& id, Parm1 p1)
    AbstractProduct* CreateObject(const IdentifierType& id, Parm1 p1, Parm2 p2)
    ...
    
 分别用于表示创建方法不带参数，带一个参数，带两个参数等等。可以看到借助于variadic template我们用一个模板替换了以前的n个成员函数。

<!--TOC-->
  - [Factory](#factory)
  - [visitor](#visitor)
  - [AbstractFactory](#abstractfactory)
  - [CloneFactory](#clonefactory)
  - [SmartPtr](#smartptr)
  - [MultiMethod](#multimethod)
<!--/TOC-->


 ## Factory

 Factory模板位于loki/Factory.h
 声明

    template<
      typename IdentifierType,
      template<typename, class>
      class FactoryErrorPolicy,
      class AbstractProduct,
      class... Param>
    class Factory ;

如果你想为某一继承体系提供object factories,上述声明中的AbstractProduct 应指定为该继承体系的base class.

IdentifierType 用来标识继承体系中的某个型别。IdentifierType必须是有序型别(ordered Type)，能作为std::map的key类型，常用的类型如int, std::string等

FactoryErrorPolicy用于指定错误处理的策略。

Factory实作出以下基本操作

    bool Register(const IdentifierType &id, const std::function<std::unique_ptr<AbstractProduct> (Param...)>& creator)
creator 是用来创建对象的functor, 能接受参数Param...，返回类型为std::unique_ptr\<AbstractProduct>。


      template<class ID, class... Arg>
      std::unique_ptr<AbstractProduct> CreateObject(const ID &id, Arg &&...arg);

以上函数查询id是否注册过，如果注册过的话，调用id对应的creator方法，参数为arg...来创建对象，返回结果。
例子：

    #include <iostream>
    #include <string>
    #include <loki/Factory.h>


    using namespace Loki;
    using std::cout;
    using std::endl;


    ////////////////////////////////////////////
    // Object to create: Product
    // Constructor with 0 and 2 arguments
    ////////////////////////////////////////////

    class AbstractProduct
    {
    public:
      virtual int get_x() const = 0;
      virtual int get_y() const = 0;
      virtual ~AbstractProduct() = default;
    };

    class Product : public AbstractProduct
    {
      int x = 0;
      int y = 0;

    public:
      Product() = default;
      Product(int xa, int ya) : x(std::move(xa)), y(std::move(ya)) {}
      int get_x() const
      {
        return x;
      }
      int get_y() const
      {
        return y;
      }
    };

    ///////////////////////////////////////////////////////////
    // Factory for creation a Product object without parameters
    ///////////////////////////////////////////////////////////

    typedef Factory<int, DefaultFactoryError, AbstractProduct>
      PFactoryNull;

    /////////////////////////////////////////////////////////////
    // Factory for creation a Product object with 2 parameters
    /////////////////////////////////////////////////////////////

    typedef Factory<int, DefaultFactoryError, AbstractProduct, int, int>
      PFactory;

    ////////////////////////////////////////////////////
    // Creator functions with different names
    ////////////////////////////////////////////////////

    std::unique_ptr<Product >createProductNull()
    {
      cout << "createProductNull()" << endl;
      return std::make_unique<Product>();
    }
    auto createProductParm(int a, int b)
    {
      cout << "createProductParm( int a, int b ) " << endl;
      return std::make_unique<Product>(a,b);
    }

    ///////////////////////////////////////////////////
    // Overloaded creator functions
    ///////////////////////////////////////////////////
    auto createProductNullOver()
    {
      cout << "createProductOver()" << endl;
      return std::make_unique<Product>();
    }
    auto createProductParmOver(int a, int b)
    {
      cout << "createProductOver( int a, int b )" << endl;
      return std::make_unique<Product>(a,b);
    }


    void testFactoryNull()
    {
      PFactoryNull pFactoryNull;
      bool const ok1 = pFactoryNull.Register(1, createProductNull);
      if (ok1) {
        std::unique_ptr<AbstractProduct> pObject(pFactoryNull.CreateObject(1));
        if (pObject.get()) {
          cout << "pObject->get_x() = " << pObject->get_x() << endl;
          cout << "pObject->get_y() = " << pObject->get_y() << endl;
        }
      }
    }

    void testFactoryBinary()
    {
      PFactory pFactoryNull;
      bool const ok1 = pFactoryNull.Register(1, createProductParm);
      if (ok1) {
        std::unique_ptr<AbstractProduct> pObject(pFactoryNull.CreateObject(1, 5, 5));
        if (pObject) {
          cout << "pObject->get_x() = " << pObject->get_x() << endl;
          cout << "pObject->get_y() = " << pObject->get_y() << endl;
        }
      }
    }

    int main()
    {
      testFactoryNull();
      testFactoryBinary();
    }

    
 输出：

      createProductNull()
      pObject->get_x() = 0
      pObject->get_y() = 0
      createProductParm( int a, int b ) 
      pObject->get_x() = 5
      pObject->get_y() = 5
    
## visitor

 visitor模式是一种设计模式，它可以在不改变数据结构的情况下，为数据结构中的元素添加新的操作。visitor模式的目的是将数据结构与数据操作分离。visitor模式的优点是增加新的操作很容易，因为增加新的操作就意味着增加一个新的visitor。

若要实作Acyclic Visitor，请使用 Basevisitor作为"strawman" base class及 visitor和visitable:

    class BaseVisitor;

    template<typename R, bool constparam, class... Head>
    class Visitor;

    template<
      typename R = void,
      bool ConstVisitable = false,
      template<typename, class> class CatchAll = DefaultCatchAll>
    class BaseVisitable;

Visitor和BaseVisitable的第一个template参数分别是成员函数Visit()的返回值类型和BaseVisitable的返回值类型。第二个参数指定Visit()的参数是否为const引用。
BaseVisitable的第三个template参数是个policy，用来处理catch-all问题

从BaseVisitable派生出你的继承体系的root class,然后在这个继承体系的每一个class中使用宏DEFINE_VISITABLE()。

从Visitor<R,ConstVisitable ,Product... >中派生出你的concrete visitor classes，这里的Product是你的产品继承体系里的所有的类型，并针对继承体系中的每一个类实作出成员函数 Visit:

    class VariableVisitor : public Loki::Visitor < void
      , false,Shape, Circle > {
     public:
      void Visit(Shape&) { std::cout << "void Visit(Shape&)\n"; }
      void Visit(Circle&) { std::cout << "void Visit(Circle&)\n"; }
    };


 例子:
 
    #include <loki/Visitor.h>
    #include <iostream>

    class Shape : public Loki::BaseVisitable<>
    {
     public:
      LOKI_DEFINE_VISITABLE()
    };

    class Circle : public Shape
    {
     public:
      LOKI_DEFINE_VISITABLE()
    };

    class VariableVisitor : public Loki::Visitor < void
      , false,Shape, Circle > {
     public:
      void Visit(Shape&) { std::cout << "void Visit(Shape&)\n"; }
      void Visit(Circle&) { std::cout << "void Visit(Circle&)\n"; }
    };

    class CShape : public Loki::BaseVisitable<void,true>
    {
     public:
      LOKI_DEFINE_CONST_VISITABLE()
    };

    class CCircle : public CShape
    {
     public:
      LOKI_DEFINE_CONST_VISITABLE()
    };

    class CVariableVisitor : public Loki::Visitor<void,true, CShape,CCircle>
    {
     public:
      void Visit(const CShape&) { std::cout << "void Visit(CShape&)\n"; }
      void Visit(const CCircle&) { std::cout << "void Visit(CCircle&)\n"; }
    };

    int main()
    {
      VariableVisitor visitor;
      Circle           circle;
      Shape*           dyn = &circle;
      Shape           shape ;
      dyn->Accept(visitor);
      dyn = &shape;
      dyn->Accept(visitor);

      CVariableVisitor cvisitor;
      CCircle           ccircle;
      CShape*           cdyn = &ccircle;
      cdyn->Accept(cvisitor);
      CShape cshape;
      cdyn = &cshape;
      cdyn->Accept(cvisitor);
    }

## AbstractFactory


    template<class... U>
    using AbstractFactory = ...;

其中U...参数包，用来指定这个Factory将要生成的abstract products。例如

using AbstractEnemyFactory=AbstractFactory<Soldier, Monster>;

AbstractFactory提供一个名为Create()的成员函数，可以用abstract products中的一个类型来实例化，用于创建一个abstract product。例如

    AbstractEnemyFactory factory;
    auto soldier = factory.Create<Soldier>();
    auto monster = factory.Create<Monster>();

  为实现AbstractFactory所定义的接口，Loki提供了一个concreteFactory template，大致如下:

    template<class AbstractFact, 
    class TList = typename AbstractFact::ProductList>
      using ConcreteFactory = ...;
    
   其中AbstractFact是将被实作出来之AbstractFactory模板的实例，如上文的AbstractEnemyFactory，TList是concrete products typelist，如mp::mp_list<SillySoldier,SillyMonster>。

   例子:

    #include <iostream>
    #include <string>
    #include <memory>
    #include <loki/AbstractFactory.h>


    using namespace Loki;
    using std::cout;
    using std::endl;

    struct Soldier
    {
    public:
      virtual ~Soldier(){};

      virtual void print() const {
      
        cout << "Soldier\n";
      }
    };

    struct SillySoldier:public Soldier
    {
    public:
      virtual void print() const {
        cout << "SillySoldier\n";
      }
    };

    struct Monster
    {
    public:
      virtual ~Monster() =default;
      virtual void print() const = 0;
    };

    struct SillyMonster:public Monster
    {
    public:
      virtual void print() const {
        cout << "SillyMonster\n";
      }
    };
    using AbstractEnemyFactory=AbstractFactory<Soldier, Monster>;

    using ConcreteEnemyFactory= ConcreteFactory<AbstractEnemyFactory, mp::mp_list<SillySoldier,SillyMonster>> ;
    int main() {
      ConcreteEnemyFactory cef;
      AbstractEnemyFactory *pA = &cef;
      auto pS = pA->Create<Soldier>();
      std::unique_ptr<Soldier> p(pS);
      p->print();
    }

## CloneFactory
CloneFactory的声明如下:

    template<
      class AbstractProduct,
      class ProductCreator =
        std::unique_ptr<AbstractProduct> (*)(const AbstractProduct *),
      template<typename, class>
      class FactoryErrorPolicy = DefaultFactoryError>
    class CloneFactory

其中AbstractProduct是继承体系要被clone的产品的基类，

ProductCreator的角色是复制经由参数传入的对象，返回复制的对象的unique_ptr指针。

CloneFactory实作出以下基本操作:

      template<class T>
      bool Register();
 
 T是AbstractProduct的一个derived class，这个函数将T类型的clone功能注册到CloneFactory中，以便CloneFactory可以复制T的对象。


      template<class T>
      bool Unregister();

 T是AbstractProduct的一个derived class，这个函数将取消CloneFacotry相应的T类型clone功能的注册。

      std::unique_ptr<AbstractProduct> CreateObject(const AbstractProduct *model);
 
 input参数model是一个AbstractProduct的对象，这个函数在内部map中查找model的动态型别，如果找到将复制model的动态型别，返回复制的对象的unique_ptr指针。如果没找到，将调用FactoryErrorPolicy\<boost::typeindex::type_index, AbstractProduct>的OnUnknownType()函数。

例子:

    #include <iostream>
    #include <string>
    #include <loki/Factory.h>


    using namespace Loki;
    using std::cout;
    using std::endl;


    ////////////////////////////////////////////
    // Object to clone: Product
    ////////////////////////////////////////////

    class Shape
    {
    public:
      virtual void draw() = 0;
      virtual ~Shape() = default;
    };

    class Circle : public Shape
    {

    public:
      virtual void draw() { cout << "Circle" << endl; }
    };

    class Square : public Shape
    {

    public:
      virtual void draw() { cout << "Square" << endl; }
    };

    int main()
    {
      using CF= CloneFactory<Shape>;
      CF cf;
      cf.Register<Square>();
      cf.Register<Circle>();
      Square s;
      auto r = cf.CreateObject(&s);
      r->draw();
      Circle c;
      auto rc = cf.CreateObject(&c);
      rc->draw();
    }

## SmartPtr

SmartPtr声明如下:

    template<
      typename T,
      template<class> class OwnershipPolicy = RefCounted,
      template<class> class CheckingPolicy = AssertCheck,
      template<class> class StoragePolicy = DefaultSPStorage,
      template<class> class ConstnessPolicy = DontPropagateConst>
    class SmartPtr;

T是SmartPtr所指类型

OwnerShipPolicy 所有权管理策略,可以是DeepCopy,RefCounted,RefCountedAtomic,RefLinked,Unique,NoCopy中的一个

CheckingPolicy检查SmartPtr默认构造，用裸指针构造和提领前检查pointee。可以是NoCheck,AssertCheck,AssertCheckStrict,RejectNullStatic,RejectNull,RejectNullStrict中的一个.

StoragePolicy 指明如何存储和访问pointee对象，可以是DefaultStorage,ArrayStorage和LockedStorage。

ConstnessPolicy指明是否传播const语义，可以是DontPropagateConst或ProgagateConst

例子:

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

## MultiMethod

StaticDispatcher使用

    template<
      class Executor,
      class OriginTypesLhs,
      class OriginTypesRhs = OriginTypesLhs,
      bool symmetric = false,
      typename ResultType = void>
    class StaticDispatcher;
    
OriginTypesLhs是Executor中仿函数左侧的参数的继承体系中的所有类,OriginTypesRhs是右侧的所有类,symmetric指明是否交换参数，这样Executor可以在左右继承体系一样时不必要实现所有组合。


FnDispatcher用于添加函数指针,BaseLhs是左侧参数的基类，BaseRhs是右侧参数的基类

    template<class BaseLhs,
     class BaseRhs = BaseLhs, typename ResultType = void,
     class CastingPolicy = DynamicCaster,
     template<class, class, class, class> class DispatcherBackend = BasicDispatcher>
    class FnDispatcher;


FunctorDispatcher用于添加仿函数或函数指针,BaseLhs是左侧参数的基类，BaseRhs是右侧参数的基类

    template<class BaseLhs, 
        class BaseRhs = BaseLhs,
        typename ResultType = void,
        class CastingPolicy = DynamicCaster, template<class, class, class, class> class DispatcherBackend = BasicDispatcher>
    class FunctorDispatcher;

例子:

    #include <iostream>
    #include <string>
    #include <loki/MultiMethods.h>
    struct First
    {
      virtual ~First() = default;
    };
    struct Second : public First
    {
    };
    struct Snd : public First
    {
    };
    struct Executor
    {
      void operator()(First, First) const
      {
        std::cout << "first first";
        std::cout << std::endl;
      }
      void operator()(Second, First) const
      {
        std::cout << "Second first";
        std::cout << std::endl;
      }
      void operator()(Second, Second) const
      {
        std::cout << "Second second";
        std::cout << std::endl;
      }
      void operator()(Snd, First) const
      {
        std::cout << "Snd first";
        std::cout << std::endl;
      }
      void operator()(Snd, Second) const
      {
        std::cout << "Snd second";
        std::cout << std::endl;
      }
      void operator()(Snd, Snd) const
      {
        std::cout << "Snd snd";
        std::cout << std::endl;
      }
      void OnError(First, First) const
      {
        std::cout << " On error";
        std::cout << std::endl;
      }
    };
    void firstfirst(First &, First &)
    {
      std::cout << "first first";
      std::cout << std::endl;
    }
    void firstsecond(First &, Second &)
    {
      std::cout << "first second";
      std::cout << std::endl;
    }
    void secondfirst(Second &, First &)
    {
      std::cout << "Second first";
      std::cout << std::endl;
    }
    void secondsecond(Second &, Second &)
    {
      std::cout << "Second second";
      std::cout << std::endl;
    }
    namespace mp = boost::mp11;
    int main()
    {
      {
        Loki::StaticDispatcher<Executor, mp::mp_list<First, Second, Snd>, mp::mp_list<First, Second, Snd>,true> dis;
        Executor e;
        First f;
        Second s;
        Snd snd;
        dis.invoke(e, f, f);
        dis.invoke(e, s, f);
        dis.invoke(e, snd, f);
        dis.invoke(e, f, s);
        dis.invoke(e, s, s);
        dis.invoke(e, snd, s);
        dis.invoke(e, f, snd);
        dis.invoke(e, s, snd);
        dis.invoke(e, snd, snd);
      }
      {
        First f;
        Second s;
          std::cout << "start FnDispatcher\n";
        Loki::FnDispatcher<First> dis;
        dis.Add<true,&firstsecond>();
        dis(f, s);
        dis(s, f);
          std::cout << "end FnDispatcher\n";
      }
      {
        auto firstfirst = [](First &, First &) {
          std::cout << "first first";
          std::cout << std::endl;
        };
        auto firstsecond = [](First &, Second &) {
          std::cout << "first second";
          std::cout << std::endl;
        };
        Loki::FunctorDispatcher<First> func;
        func.Add(firstfirst);
        func.Add<true>(firstsecond);
        func.Add(&secondsecond);
        First f;
        Second s;
        func(f,f);
        func(f,s);
        func(s,f);
        func(s,s);
      }
    }

