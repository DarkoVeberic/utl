#ifndef _TestClass_h_
#define _TestClass_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include <Rtypes.h>
#include <vector>


struct Baz {
  Baz() : fInt(0) { }
  Baz(const int i) : fInt(i) { }
  void Set(const int i) { fInt = i; }
  int fInt;
  ClassDefNV(Baz, 1);
};


struct Bar {
  Bar() : fBaz(new Baz) { }
  Bar(const int i) : fBaz(new Baz(i)) { }
  Bar(const Bar& b) { fBaz = new Baz(b.fBaz ? b.fBaz->fInt : 0); }
  ~Bar() { delete fBaz; }
  Bar& operator=(const Bar& b) { delete fBaz; fBaz = new Baz(b.fBaz ? b.fBaz->fInt : 0); return *this; }
  void Set(const int i) { if (fBaz) fBaz->Set(i); }
  Baz* fBaz;
  ClassDefNV(Bar, 1);
};


struct Foo {
  Foo() : fBar(0) { }
  Foo(const int i) : fBar(i) { }
  void Set(const int i) { fBar.Set(i); }
  Bar fBar;
  ClassDefNV(Foo, 1);
};


struct TestClass {
  TestClass() : fFooPtr(0) { Set(0); }
  TestClass(const int i) : fFooPtr(0) { Set(i); }
  
  void
  Set(const int i)
  {
    fInt = i;
    fDouble = 3.14*i;
    fVec.assign(i, 2*i);
    fFoo.Set(4*i);
    fFooVec.assign(i, 5*i);
    delete fFooPtr;
    fFooPtr = new Foo(6*i);
  }

  int fInt;
  double fDouble;
  std::vector<int> fVec;
  Foo fFoo;
  std::vector<Foo> fFooVec;
  static const int fFooPtrSize = 1;
  Foo* fFooPtr; //->

  ClassDefNV(TestClass, 1);
};


#endif
