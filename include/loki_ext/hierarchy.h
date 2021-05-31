template <class T>
struct HierarchyRoot
{
  // HierarchyId is a nested class
  struct HierarchyId {};
};


//
//class Shape : private HierarchyRoot<Shape>
//{
//  ...
//public:
//  using HierarchyRoot<Shape>::HierarchyId;
//};
//
//

template <class T>
struct HierarchyTraits
{
    //...
};

