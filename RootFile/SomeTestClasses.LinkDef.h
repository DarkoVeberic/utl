#ifndef _SomeTestClasses_LinkDef_h_
#define _SomeTestClasses_LinkDef_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#ifdef __CINT__

#  pragma link off all globals;
#  pragma link off all classes;
#  pragma link off all functions;

#  pragma link C++ class TestClass+;
#  pragma link C++ class AnotherTestClass+;
#  pragma link C++ class Foo+;
#  pragma link C++ class Bar+;
#  pragma link C++ class Baz+;

#endif

#endif
