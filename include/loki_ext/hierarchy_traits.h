#ifndef HIERARCHY_TRAITS_H
#define HIERARCHY_TRAITS_H
namespace Loki{
    template <class T>
    struct HierarchyRoot
    {
      // HierarchyId is a nested class
      struct HierarchyId {};
    };
}

//
//template <class HierarchyId>
//class HierarchyTraits
//{
//  ... most general traits here ...
//};
//template <class T>
//class Traits
//  : public HierarchyTraits<T::HierarchyId>
//{
//  // empty body - inherits all symbols from base class
//};
//
//
//template <>
//class Traits<Ellipse>
//{
//  ... specialized stuff for Ellipse ...
//};
//
//
//
//class Bitmap : public Shape,
//  public HierarchyRoot<Bitmap>
//{
//  ...
//public:
//  using HierarchyRoot<Bitmap>::HierarchyId;
//};
//
//
#endif
