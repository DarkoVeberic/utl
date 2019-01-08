// $Id: RootOutFile.h 1570 2018-12-05 12:30:48Z darko $
#ifndef _io_RootOutFile_h_
#define _io_RootOutFile_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include <io/SaveCurrentTDirectory.h>
#include <TFile.h>
#include <TTree.h>
#include <string>
#include <stdexcept>


#ifdef IO_ROOTFILE_DEBUG
#  define IO_ROOTFILE_CHECK Check()
#else
#  define IO_ROOTFILE_CHECK
#endif

namespace io {

  template<class Entry>
  class RootOutFile {
  public:
    RootOutFile(const std::string& filename, const int compression = 1)
    {
      Open(filename, compression);
    }

    ~RootOutFile() { Close(); }

    void
    Fill(const Entry& entry)
    {
      fEntryPtr = &entry;
      IO_ROOTFILE_CHECK;
      fTree->Fill();
    }

    void Fill(const std::vector<Entry>& entries)
    { for (const Entry& e : entries) Fill(e); }

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

    void SetMaxTreeSize(const Long64_t size) { fTree->SetMaxTreeSize(size); }

    TFile& GetTFile() { IO_ROOTFILE_CHECK; return *fFile; }

    TTree& GetTTree() { IO_ROOTFILE_CHECK; return *fTree; }

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
      if (!fFile || fFile->IsZombie() || !fFile->IsOpen())
        Error("file open failed");
      if (!fTree || !fEntryPtr)
        Error("tree error");
    }

    TFile* fFile = nullptr;
    TTree* fTree = nullptr;
    const Entry* fEntryPtr = nullptr;
  };

}


#endif
