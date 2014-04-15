#ifndef _RootFile_h_
#define _RootFile_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include "SaveCurrentTDirectory.h"
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <string>
#include <stdexcept>


template<class Entry>
class RootFile {
public:
  class Iterator {
  public:
    typedef Iterator self_type;
    typedef Entry value_type;
    typedef Entry& reference;
    typedef Entry* pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;

    Iterator(RootFile& file, unsigned int index) : fFile(file), fIndex(index) { }
    Iterator& operator++() { ++fIndex; return *this; } // prefix ++it
    const Entry& operator*() { return fFile.GetEntry(fIndex); }
    const Entry* operator->() { return &operator*(); }
    bool operator==(const Iterator& it) const { IsSameFile(it); return fIndex == it.fIndex; }
    bool operator!=(const Iterator& it) const { IsSameFile(it); return !operator==(it); }
  private:
    void
    IsSameFile(const Iterator& it)
      const
    {
      if (&fFile != &it.fFile)
        throw std::logic_error("mixing iterators of different RootFiles");
    }

    RootFile& fFile;
    unsigned int fIndex;
  };

  RootFile(const std::string& filename, const char mode) :
    fFile(0),
    fTree(0),
    fChain(0),
    fEntryBuffer(0)
  {
    if (mode == 'r')
      OpenForRead(filename);
    else if (mode == 'w')
      OpenForWrite(filename);
    else
      Error("illegal char in 'mode'");
  }

  ~RootFile() { Close(); }

  unsigned int GetNEntries()
  { return fChain ? fChain->GetEntries() : 0; }

  const Entry&
  GetEntry(const unsigned int i)
  {
    CheckForReading();
    if (!fChain->GetEntry(i))
      Error("write only or no entry found");
    return *fEntryBuffer;
  }

  Iterator
  Begin()
  {
    CheckForReading();
    return Iterator(*this, 0);
  }

  Iterator
  End()
  {
    CheckForReading();
    return Iterator(*this, GetNEntries());
  }

  void
  Fill(const Entry& entry)
  {
    CheckForWriting();
    *fEntryBuffer = entry;
    fTree->Fill();
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
    delete fEntryBuffer;
    fEntryBuffer = 0;
    delete fChain;
    fChain = 0;
  }

private:
  // prevent copying
  RootFile(const RootFile&);
  RootFile& operator=(const RootFile&);

  void
  Error(const char* const message)
  {
    Close();
    throw std::runtime_error(message);
  }

  void
  OpenForRead(const std::string& filename)
  {
    const SaveCurrentTDirectory save;
    fChain = new TChain((std::string(Entry::Class_Name()) + "Tree").c_str());
    const int nFiles = fChain->Add(filename.c_str());
    if (!nFiles)
      Error("no files found");
    fEntryBuffer = new Entry;
    fChain->SetBranchAddress(Entry::Class_Name(), &fEntryBuffer);
    CheckForReading();
  }

  void
  OpenForWrite(const std::string& filename)
  {
    const SaveCurrentTDirectory save;
    fFile = new TFile(filename.c_str(), "recreate", "", /*compression level*/9);
    const std::string treeName = std::string(Entry::Class_Name()) + "Tree";
    fTree = new TTree(treeName.c_str(), treeName.c_str());
    fEntryBuffer = new Entry;
    fTree->Branch(Entry::Class_Name(), Entry::Class_Name(), &fEntryBuffer, 16000, 99);
    CheckForWriting();
  }

  void
  CheckForReading()
  {
    if (fFile || fTree || !fChain || !fEntryBuffer)
      Error("write only or no entry found");
  }

  void
  CheckForWriting()
  {
    const char* err = 0;
    if (!fFile || fFile->IsZombie() || !fFile->IsOpen())
      err = "file open failed";
    if (!fTree || !fEntryBuffer)
      err = "tree error";
    if (err)
      Error(err);
  }

  TFile* fFile;   // only for writing
  TTree* fTree;   // only for writing
  TChain* fChain; // only for reading
  Entry* fEntryBuffer;
};


#endif
