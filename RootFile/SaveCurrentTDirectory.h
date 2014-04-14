#ifndef _SaveCurrentDirectory_h_
#define _SaveCurrentDirectory_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include <TDirectory.h>


class SaveCurrentTDirectory {
public:
  SaveCurrentTDirectory() : fDirectory(gDirectory) { }

  ~SaveCurrentTDirectory() { fDirectory->cd(); }

private:
  TDirectory* const fDirectory;
};


#endif
