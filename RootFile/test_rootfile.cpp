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
  unsigned int count = 0;
  {
    // write root files
    for (unsigned int i = 0; i < 5; ++i) {
      RootFile<TestClass> file(string("TestClass-") + string(1, char('0'+i)) + ".root", 'w');
      for (unsigned int i = 0; i < 10; ++i) {
        const TestClass tc = { int(count), double(count), vector<int>(count, count) };
        file.Fill(tc);
        ++count;
      }
    }
  }

  {
    // read root files, random access
    RootFile<TestClass> file("TestClass-*.root", 'r');
    const unsigned int n = file.GetNEntries();
    if (n != count)
      return 1;
    for (unsigned int i = 0; i < n; ++i) {
      const TestClass& tc = file.GetEntry(i);
      const TestClass tt = { int(i), double(i), vector<int>(i, i) };
      if (tc != tt)
        return 2;
    }
  }

  {
    // read root files, forward iterator
    RootFile<TestClass> file("TestClass-*.root", 'r');
    unsigned int i = 0;
    for (RootFile<TestClass>::Iterator it = file.Begin(), end = file.End();
         it != end; ++it) {
      const TestClass& tc = *it;
      const TestClass tt = { int(i), double(i), vector<int>(i, i) };
      if (tc != tt)
        return 3;
      ++i;
    }
    if (i != count)
      return 4;
  }

  return 0;
}
