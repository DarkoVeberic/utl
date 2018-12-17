// $Id: RootInFile.h 1576 2018-12-17 15:26:42Z darko $
#ifndef _io_RootInFile_h_
#define _io_RootInFile_h_

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014
*/

#include <io/SaveCurrentTDirectory.h>
#include <TChain.h>
#include <TChainElement.h>
#include <TFile.h>
#include <string>
#include <limits>
#include <stdexcept>


#ifdef IO_ROOTFILE_DEBUG
#  define IO_ROOTFILE_CHECK Check()
#else
#  define IO_ROOTFILE_CHECK
#endif

namespace io {

  template<class Entry>
  class RootInFile {
  public:
    class Iterator :
      public
      std::iterator<
        std::input_iterator_tag,
        Entry,
        std::ptrdiff_t,
        Entry*,
        Entry&
      > {
    public:
      typedef Iterator self_type;
      typedef Entry value_type;
      typedef Entry& reference;
      typedef Entry* pointer;
      typedef int difference_type;
      typedef std::input_iterator_tag iterator_category;

      Iterator(RootInFile& file, const ULong64_t index) : fFile(file), fIndex(index) { }
      Iterator& operator++() { ++fIndex; return *this; } // prefix ++it
      Entry& operator*() { return fFile[fIndex]; }
      Entry* operator->() { return &operator*(); }
      bool operator==(const Iterator& it) const { IsSameFile(it); return fIndex == it.fIndex; }
      bool operator!=(const Iterator& it) const { IsSameFile(it); return !operator==(it); }
    private:
      void
      IsSameFile(const Iterator& it)
        const
      {
        if (&fFile != &it.fFile)
          throw std::logic_error("RootInFile::Iterator: mixing iterators of different RootFiles");
      }

      RootInFile& fFile;
      ULong64_t fIndex;
    };

    RootInFile(const std::string& filename,
               const std::string& treeName = "",
               const std::string& branchName = "")
    { Open(std::vector<std::string>(1, filename), treeName, branchName); }

    RootInFile(const std::vector<std::string>& filenames,
               const std::string& treeName = "",
               const std::string& branchName = "")
    { Open(filenames, treeName, branchName); }

    ~RootInFile() { Close(); }

    ULong64_t
    GetSize()
    {
      IO_ROOTFILE_CHECK;
      return fChain->GetEntries();
    }

    Entry&
    operator[](const ULong64_t i)
    {
      IO_ROOTFILE_CHECK;
      if (i != fCurrentEntryIndex) {
        if (!fChain->GetEntry(i))
          throw std::out_of_range("RootInFile::operator[]: requested entry not found in file chain");
        fCurrentEntryIndex = i;
      }
      IO_ROOTFILE_CHECK;
      return *fEntryBuffer;
    }

    Iterator
    Begin()
    {
      IO_ROOTFILE_CHECK;
      return Iterator(*this, 0);
    }

    Iterator begin() { return Begin(); }

    Iterator
    End()
    {
      IO_ROOTFILE_CHECK;
      return Iterator(*this, GetSize());
    }

    Iterator end() { return End(); }

    std::vector<Entry> ReadAll() { return std::vector<Entry>(begin(), end()); }

    template<class T>
    T&
    Get(const std::string& name = T::Class_Name())
    {
      T* obj = Find<T>(name);
      if (!obj)
        Error(("RootInFile::Get: no object '" + name + "' found in file").c_str());
      return *obj;
    }

    template<class T>
    bool Has(const std::string& name = T::Class_Name()) { return Find<T>(name); }

    void
    Close()
    {
      fCurrentEntryIndex = std::numeric_limits<decltype(fCurrentEntryIndex)>::max();
      delete fEntryBuffer;
      fEntryBuffer = nullptr;
      delete fChain;
      fChain = nullptr;
    }

    void SetBranchStatus(const std::string& branch, const bool status)
    { Check(); fChain->SetBranchStatus(branch.c_str(), status); }

  private:
    // prevent copying
    RootInFile(const RootInFile&);
    RootInFile& operator=(const RootInFile&);

    void
    Error(const char* const message)
    {
      Close();
      throw std::runtime_error(message);
    }

    void
    Open(const std::vector<std::string>& filenames,
         std::string treeName,
         std::string branchName)
    {
      const SaveCurrentTDirectory save;
      if (treeName.empty())
        treeName = std::string(Entry::Class_Name()) + "Tree";
      fChain = new TChain(treeName.c_str());
      size_t nFiles = 0;
      for (const auto& name : filenames)
        nFiles += fChain->Add(name.c_str());
      if (!nFiles)
        Error("RootInFile::Open: no files found");
      fEntryBuffer = new Entry;
      if (branchName.empty())
        branchName = Entry::Class_Name();
      fChain->SetBranchAddress(branchName.c_str(), &fEntryBuffer);
      Check();
    }

    template<class T>
    T*
    Find(const std::string& name)
    {
      TObjArray* files;
      if (!fChain || !(files = fChain->GetListOfFiles()))
        Error("RootInFile::Find: file not open");
      T* obj = nullptr;
      TIter next(files);
      for (TChainElement* c = (TChainElement*)next(); !obj && c; c = (TChainElement*)next())
        TFile(c->GetTitle()).GetObject(name.c_str(), obj);
      return obj;
    }

    void
    Check()
    {
      if (!fChain || !fEntryBuffer)
        Error("RootInFile::Check: no entry found");
    }

    TChain* fChain = nullptr;
    ULong64_t fCurrentEntryIndex = std::numeric_limits<decltype(fCurrentEntryIndex)>::max();
    Entry* fEntryBuffer = nullptr;
  };

}


#endif
