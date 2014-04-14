#ifndef _RootFile_h_
#define _RootFile_h_

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
    Iterator& operator++() { ++fIndex; return *this; } // prefix
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
    fEntryBuffer(0)
  {
    if (mode == 'r')
      OpenForRead(filename);
    else if (mode == 'w')
      OpenForWrite(filename);
    else
      throw std::invalid_argument("illegal char in 'mode'");
  }

  ~RootFile()
  {
    Close();
    delete fEntryBuffer;
  }

  bool IsOpen() const { return fFile && fFile->IsOpen(); }

  unsigned int GetNEntries() const
  { return fTree ? fTree->GetEntriesFast() : 0; }

  const Entry&
  GetEntry(const unsigned int i)
    const
  {
    CheckIsReadable();
    if (!fTree->GetEntry(i))
      throw std::logic_error("write only or no entry found");
    return *fEntryBuffer;
  }

  Iterator
  Begin()
  {
    CheckIsReadable();
    return Iterator(*this, 0);
  }

  Iterator
  End()
  {
    CheckIsReadable();
    return Iterator(*this, GetNEntries());
  }

  void
  Fill(const Entry& entry)
  {
    if (!fFile || !fFile->IsWritable())
      throw std::runtime_error("fill error");
    // NB: maybe this can be skipped
    //fEntryBuffer->Clear();
    // NB: assigment should made sure to cover all members
    *fEntryBuffer = entry;
    fTree->Fill();
  }

  void
  Close()
  {
    if (!fFile)
      return;
    if (fTree && fFile->IsWritable()) {
      fFile->cd();
      fTree->Write();
    }
    fFile->Close();
    delete fFile;
    fFile = 0;
  }

private:
  // prevent copying
  RootFile(const RootFile&);
  RootFile& operator=(const RootFile&);

  void
  OpenForRead(const std::string& filename)
  {
    const SaveCurrentTDirectory save;
    fFile = new TFile(filename.c_str(), "read");
    CheckFileIsOpen();
    fTree = (TTree*)fFile->Get((std::string(Entry::Class_Name()) + "Tree").c_str());
    if (!fTree) {
      delete fFile;
      fFile = 0;
      throw std::runtime_error("cannot find tree");
    }
    fEntryBuffer = new Entry;
    fTree->SetBranchAddress(std::string(Entry::Class_Name()).c_str(), &fEntryBuffer);
  }

  void
  OpenForWrite(const std::string& filename)
  {
    const SaveCurrentTDirectory save;
    fFile = new TFile(filename.c_str(), "recreate", "", 9);
    CheckFileIsOpen();
    //fFile->cd();
    const std::string treeName = std::string(Entry::Class_Name()) + "Tree";
    fTree = new TTree(treeName.c_str(), treeName.c_str());
    fEntryBuffer = new Entry;
    fTree->Branch(Entry::Class_Name(), Entry::Class_Name(), &fEntryBuffer, 16000, 99);
  }

  void
  CheckFileIsOpen()
  {
    if (!fFile || fFile->IsZombie() || !fFile->IsOpen()) {
      delete fFile;
      fFile = 0;
      throw std::runtime_error("file open failed");
    }
  }

  void
  CheckIsReadable()
    const
  {
    if (!fFile || fFile->IsWritable() || !fTree)
      throw std::logic_error("write only or no entry found");
  }

  TFile* fFile;
  TTree* fTree;
  Entry* fEntryBuffer;
};


#endif
