RootFile
===

`RootInFile<T>` and `RootOutFile<T>` are two simple templated classes that
elegantly handle reading/writing of [ROOT](http://root.cern.ch) files
containing only one branch of class `T`. Since `T` can be arbitrarily complex
and may contain sub-classes, any event-like structure can be streamed into a
ROOT file in this way.

Check the `test_rootfile.cpp` file for more details.



## Description

The `RootInFile<T>` and `RootOutFile<T>` are best described as persistent
version of the `std::vector<T>` container using the ROOT streaming machinery.
This should be obvious from the usage example given below.



## Usage

For example, making a root file with a collection of streamable instances of
class `Event` (i.e. with the `ClassDef()` macro and corresponding link-def
`#pragma`) is as easy as:
```c++
RootOutFile<Event> ofile("events.root");
for (...) {
  const Event ev = ...;
  ofile << ev;
}
```

Reading-out arbitray entry is similarly easy:
```c++
RootInFile<Event> ifile("events.root");
const Event& ev = ifile[i];
```

Looping over entries can be done in two ways, random index access:
```c++
for (unsigned int i = 0, n = ifile.GetNEntries(); i < n; ++i) {
  const Event& ev = ifile[i];
  ...
}
```
or with iterators:
```c++
for (RootInFile<Event>::Iterator it = ifile.Begin(), end = ifile.End();
     it != end; ++it) {
  const Event& ev = *it;
  ...
}
```
or in C++11:
```c++
for (const auto& event : ifile) {
  ...
}
```

Note that the reading of the root file is instead of the
[`TTree`](http://root.cern.ch/root/html/TTree.html) implemented with the
[`TChain`](http://root.cern.ch/root/html/TChain.html) so globbing in file name
chains files together to give seamless collection of objects:
```c++
RootInFile<Event> ifile("event-*.root");
```



## License

Released under dual licence: the GPL version 2 and the two-clause BSD license.
