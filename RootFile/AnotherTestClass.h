#ifndef _AnotherTestClass_h_
#define _AnotherTestClass_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include <Rtypes.h>
#include <vector>


class AnotherTestClass {
public:
  int fInt;
  double fDouble;
  std::vector<int> fVec;

  ClassDefNV(AnotherTestClass, 1);
};


#endif
