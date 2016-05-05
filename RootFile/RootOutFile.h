#ifndef _RootOutFile_h_
#define _RootOutFile_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include "SaveCurrentTDirectory.h"
#include <TFile.h>
#include <TTree.h>
#include <string>
#include <stdexcept>


template<class Entry>
class RootOutFile {
public:
  RootOutFile(const std::string& filename, const int compression = 1) :
    fFile(0),
    fTree(0),
    fBranch(0),
    fEntryPtr(0)
  {
    Open(filename, compression);
  }

  ~RootOutFile() { Close(); }

  void
  Fill(const Entry& entry)
  {
    fEntryPtr = &entry;
    Check();
    fTree->Fill();
  }

  void operator<<(const Entry& entry) { Fill(entry); }

  template<class T>
  void
  Write(const T& obj)
  {
    if (!fFile)
      Error("file not open");
    fFile->WriteObject(&obj, obj.Class_Name());
  }

  void
  Close()
  {
    if (fFile && fFile->IsWritable() && fTree) {
      const SaveCurrentTDirectory save;
      fFile->cd();
      fTree->Write();
      fFile->Close();
      delete fFile;
      fFile = 0;
      //delete fTree;  // broken ROOT memory management
      fTree = 0;
    }
    fEntryPtr = 0;
  }

private:
  // prevent copying
  RootOutFile(const RootOutFile&);
  RootOutFile& operator=(const RootOutFile&);

  void
  Error(const char* const message)
  {
    Close();
    throw std::runtime_error(message);
  }

  void
  Open(const std::string& filename, const int compression)
  {
    const SaveCurrentTDirectory save;
    fFile = new TFile(filename.c_str(), "recreate", "", compression);
    const std::string treeName = std::string(Entry::Class_Name()) + "Tree";
    fTree = new TTree(treeName.c_str(), treeName.c_str());
    fEntryPtr = 0;
    fTree->Branch(Entry::Class_Name(), Entry::Class_Name(), &fEntryPtr, 900000);
    Check();
  }

  void
  Check()
  {
    const char* err = 0;
    if (!fFile || fFile->IsZombie() || !fFile->IsOpen())
      err = "file open failed";
    if (!fTree || !fEntryPtr)
      err = "tree error";
    if (err)
      Error(err);
  }

  TFile* fFile;
  TTree* fTree;
  TBranch* fBranch;
  const Entry* fEntryPtr;
};


#endif
