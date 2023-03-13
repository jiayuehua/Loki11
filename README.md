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








