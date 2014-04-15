#ifndef _TestClass_h_
#define _TestClass_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include <Rtypes.h>
#include <vector>


class TestClass {
public:
  int fInt;
  double fDouble;
  std::vector<int> fVec;

  ClassDefNV(TestClass, 1);
};


#endif
