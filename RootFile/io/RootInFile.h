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
#include <stdexcept>


namespace io {

template<class Entry>
class RootInFile {
public:
  class Iterator {
  public:
    typedef Iterator self_type;
    typedef Entry value_type;
    typedef Entry& reference;
    typedef Entry* pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;

    Iterator(RootInFile& file, unsigned int index) : fFile(file), fIndex(index) { }
    Iterator& operator++() { ++fIndex; return *this; } // prefix ++it
    const Entry& operator*() { return fFile[fIndex]; }
    const Entry* operator->() { return &operator*(); }
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
    unsigned int fIndex;
  };

  RootInFile(const std::string& filename) :
    fChain(0),
    fEntryBuffer(0)
  {
    Open(filename);
  }

  RootInFile(const std::vector<std::string>& filenames) :
    fChain(0),
    fEntryBuffer(0)
  {
    Open(filenames);
  }

  ~RootInFile() { Close(); }

  unsigned int
  GetSize()
  {
    Check();
    return fChain->GetEntries();
  }

  const Entry&
  operator[](const unsigned int i)
  {
    Check();
    if (!fChain->GetEntry(i))
      throw std::out_of_range("RootInFile::operator[]: requested entry not found in file chain");
    return *fEntryBuffer;
  }

  Iterator
  Begin()
  {
    Check();
    return Iterator(*this, 0);
  }

  Iterator
  End()
  {
    Check();
    return Iterator(*this, GetSize());
  }

  template<class T>
  const T&
  Get()
  {
    const T* obj = Find<T>();
    if (!obj)
      Error((std::string("RootInFile::Get: no object '") + T::Class_Name() + "' found in file").c_str());
    return *obj;
  }

  template<class T>
  bool Has() { return Find<T>(); }

  void
  Close()
  {
    delete fEntryBuffer;
    fEntryBuffer = 0;
    delete fChain;
    fChain = 0;
  }

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
  Open(const std::string& filename)
  {
    Open(std::vector<std::string>(1, filename));
  }

  void
  Open(const std::vector<std::string>& filenames)
  {
    const SaveCurrentTDirectory save;
    fChain = new TChain((std::string(Entry::Class_Name()) + "Tree").c_str());
    int nFiles = 0;
    for (std::vector<std::string>::const_iterator it = filenames.begin(),
         end = filenames.end(); it != end; ++it)
      nFiles += fChain->Add(it->c_str());
    if (!nFiles)
      Error("RootInFile::Open: no files found");
    fEntryBuffer = new Entry;
    fChain->SetBranchAddress(Entry::Class_Name(), &fEntryBuffer);
    Check();
  }

  template<class T>
  const T*
  Find()
  {
    TObjArray* files;
    if (!fChain || !(files = fChain->GetListOfFiles()))
      Error("RootInFile::Find: file not open");
    const T* obj = 0;
    TIter next(files);
    for (TChainElement* c = (TChainElement*)next(); !obj && c; c = (TChainElement*)next())
      TFile(c->GetTitle()).GetObject(T::Class_Name(), obj);
    return obj;
  }

  void
  Check()
  {
    if (!fChain || !fEntryBuffer)
      Error("RootInFile::Check: no entry found");
  }

  TChain* fChain;
  Entry* fEntryBuffer;
};

}


#endif
