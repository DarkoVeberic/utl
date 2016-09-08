/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include <io/RootFile.h>
#include "TestClass.h"
#include "AnotherTestClass.h"
#include <vector>
#include <iostream>

using namespace io;
using namespace std;


inline
bool
operator!=(const Baz& b1, const Baz& b2)
{
  return b1.fInt != b2.fInt;
}


inline
bool
operator!=(const Bar& b1, const Bar& b2)
{
  return
    bool(b1.fBaz) != bool(b2.fBaz) ||
    *b1.fBaz != *b2.fBaz;
}


inline
bool
operator!=(const Foo& f1, const Foo& f2)
{
  return f1.fBar != f2.fBar;
}


inline
bool
operator==(const Foo& f1, const Foo& f2)
{
  return !(f1.fBar != f2.fBar);
}


inline
bool
operator!=(const TestClass& t1, const TestClass& t2)
{
  return
    t1.fInt != t2.fInt ||
    t1.fDouble != t2.fDouble ||
    t1.fVec != t2.fVec ||
    t1.fFoo != t2.fFoo ||
    t1.fFooVec != t2.fFooVec ||
    bool(t1.fFooPtr) != bool(t2.fFooPtr) ||
    *t1.fFooPtr != *t2.fFooPtr;
}


inline
bool
operator!=(const AnotherTestClass& t1, const AnotherTestClass& t2)
{
  return
    t1.fInt != t2.fInt ||
    t1.fDouble != t2.fDouble ||
    t1.fVec != t2.fVec;
}


int
main()
{
  cerr << "test 1... " << flush;
  unsigned int count = 0;
  {
    // write root files
    for (unsigned int i = 0; i < 5; ++i) {
      RootOutFile<TestClass> file(string("TestFile-") + string(1, char('0'+i)) + ".root");
      if (!i) {
        const AnotherTestClass atc = { 13, 13, vector<int>(13, 13) };
        file.Write(atc);
      }
      for (unsigned int i = 0; i < 10; ++i) {
        const TestClass tc(count);
        file << tc;
        ++count;
      }
    }
  }

  cerr << "pass\ntest 2... " << flush;

  {
    // read root files, random access
    RootInFile<TestClass> file("TestFile-*.root");
    const unsigned int n = file.GetSize();
    if (n != count)
      return 1;
    {
      const AnotherTestClass& atc = file.Get<AnotherTestClass>();
      const AnotherTestClass att = { 13, 13, vector<int>(13, 13) };
      if (atc != att)
        return 5;
    }
    for (unsigned int i = 0; i < n; ++i) {
      const TestClass& tc = file[i];
      const TestClass tt(i);
      if (tc != tt)
        return 2;
    }
  }

  cerr << "pass\ntest 3... " << flush;

  {
    // read root files, forward iterator
    RootInFile<TestClass> file("TestFile-*.root");
    {
      const AnotherTestClass& atc = file.Get<AnotherTestClass>();
      const AnotherTestClass att = { 13, 13, vector<int>(13, 13) };
      if (atc != att)
        return 6;
    }
    unsigned int i = 0;
    for (RootInFile<TestClass>::Iterator it = file.Begin(), end = file.End();
         it != end; ++it) {
      const TestClass& tc = *it;
      const TestClass tt(i);
      if (tc != tt)
        return 3;
      ++i;
    }
    if (i != count)
      return 4;
  }

  cerr << "pass" << endl;

  return 0;
}
