#include "RootFile.h"
#include "TestClass.h"
#include <vector>

using namespace std;


int
main()
{
  {
    RootFile<TestClass> file("TestClass.root", 'w');
    for (unsigned int i = 0; i < 10; ++i) {
      const TestClass tc = { int(i), double(i), vector<int>(i, i) };
      file.Fill(tc);
    }
    file.Close();
  }

  {
    RootFile<TestClass> file("TestClass.root", 'r');
    for (unsigned int i = 0, n = file.GetNEntries(); i < n; ++i) {
      const TestClass& tc = file.GetEntry(i);
      if (tc.fInt != int(i) || tc.fDouble != double(i) || tc.fVec != vector<int>(i, i))
        return 1;
    }
    unsigned int i = 0;
    for (RootFile<TestClass>::Iterator it = file.Begin(), end = file.End();
         it != end; ++it) {
      const TestClass& tc = *it;
      if (tc.fInt != int(i) || tc.fDouble != double(i) || tc.fVec != vector<int>(i, i))
        return 2;
      ++i;
    }
  }

  return 0;
}
