////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter Kümmel
// Copyright (c) 2023 Jia yuehua
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id: main.cpp 759 2006-10-17 20:27:29Z syntheticpp $

#include <loki/Visitor.h>
#include <iostream>

class Base : public Loki::BaseVisitable<>
{
 public:
  LOKI_DEFINE_VISITABLE()
};

class Type1 : public Base
{
 public:
  LOKI_DEFINE_VISITABLE()
};

class VariableVisitor : public Loki::Visitor < void
  , false,Base, Type1 > {
 public:
  void Visit(Base&) { std::cout << "void Visit(Base&)\n"; }
  void Visit(Type1&) { std::cout << "void Visit(Type1&)\n"; }
};

class CBase : public Loki::BaseVisitable<void,true>
{
 public:
  LOKI_DEFINE_CONST_VISITABLE()
};

class CType1 : public CBase
{
 public:
  LOKI_DEFINE_CONST_VISITABLE()
};

class CVariableVisitor : public Loki::Visitor<void,true, CBase,CType1>
{
 public:
  void Visit(const CBase&) { std::cout << "void Visit(CBase&)\n"; }
  void Visit(const CType1&) { std::cout << "void Visit(CType1&)\n"; }
};

int main()
{
  VariableVisitor visitor;
  Type1           type1;
  Base*           dyn = &type1;
  Base           base ;
  dyn->Accept(visitor);
  dyn = &base;
  dyn->Accept(visitor);

  CVariableVisitor cvisitor;
  CType1           ctype1;
  CBase*           cdyn = &ctype1;
  cdyn->Accept(cvisitor);
  CBase cbase;
  cdyn = &cbase;
  cdyn->Accept(cvisitor);
}
