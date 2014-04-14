/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include "RootFile.h"
#include "TestClass.h"
#include <vector>

using namespace std;


inline
bool
operator!=(const TestClass& t1, const TestClass& t2)
{
  return t1.fInt != t2.fInt || t1.fDouble != t2.fDouble || t1.fVec != t2.fVec;
}


int
main()
{
  {
    // write a root file
    RootFile<TestClass> file("TestClass.root", 'w');
    for (unsigned int i = 0; i < 10; ++i) {
      const TestClass tc = { int(i), double(i), vector<int>(i, i) };
      file.Fill(tc);
    }
    file.Close();
  }

  {
    // read it, random access
    RootFile<TestClass> file("TestClass.root", 'r');
    for (unsigned int i = 0, n = file.GetNEntries(); i < n; ++i) {
      const TestClass& tc = file.GetEntry(i);
      const TestClass tt = { int(i), double(i), vector<int>(i, i) };
      if (tc != tt)
        return 1;
    }
    // read it, forward iterator
    unsigned int i = 0;
    for (RootFile<TestClass>::Iterator it = file.Begin(), end = file.End();
         it != end; ++it) {
      const TestClass& tc = *it;
      const TestClass tt = { int(i), double(i), vector<int>(i, i) };
      if (tc != tt)
        return 2;
      ++i;
    }
  }

  return 0;
}
