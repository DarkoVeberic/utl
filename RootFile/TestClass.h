#ifndef _TestClass_h_
#define _TestClass_h_

#include <Rtypes.h>
#include <vector>


class TestClass {
public:
  int fInt;
  double fDouble;
  std::vector<int> fVec;

  // the only required method
  void
  Clear()
  {
    fInt = 0;
    fDouble = 0;
    fVec.clear();
  }

  ClassDefNV(TestClass, 1);
};


#endif
