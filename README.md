# Loki11 lib
update andrei's loki lib to C++11
主要改进
- 接口改为使用variadic templates，不再是typelist，实现中用到typelist的地方改为使用boost mp11库的mp_list
- typelist算法改为使用mp11的算法。
- 将typetraits部分改为直接使用C++11的type_traits
- 将functor删除，改为使用C++11的std::function

简要对比loki11和loki
以factory为例说明，Loki11工厂创建抽象对象只有一个成员模板

    template<class ID, class... Arg>
    AbstractProduct *CreateObject(const ID &id, Arg &&...arg)

 而loki的factory有许多成员

    AbstractProduct* CreateObject(const IdentifierType& id)
    AbstractProduct* CreateObject(const IdentifierType& id, Parm1 p1)
    AbstractProduct* CreateObject(const IdentifierType& id, Parm1 p1, Parm2 p2)
    ...
    
 分别用于表示创建方法不带参数，带一个参数，带两个参数等等。可以看到借助于variadic template我们用一个模板替换了以前的n个成员函数。

 ## Factory使用方法

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

    bool Register(const IdentifierType &id, const std::function<AbstractProduct *(Param...)>& creator)
creator 是用来创建对象的functor, 能接受参数Param...，返回类型为AbstractProduct指针。


      template<class ID, class... Arg>
      AbstractProduct *CreateObject(const ID &id, Arg &&...arg);

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
      Product(int xa, int ya) : x(xa), y(ya) {}
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

    Product *createProductNull()
    {
      cout << "createProductNull()" << endl;
      return new Product;
    }
    Product *createProductParm(int a, int b)
    {
      cout << "createProductParm( int a, int b ) " << endl;
      return new Product(a, b);
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
    
   

  
