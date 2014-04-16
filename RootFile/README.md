RootFile
===

`RootFile<T>` is a simple templated class that elegantly handles
reading/writing of root files containing only one [ROOT](http://root.cern.ch)
branch of class `T`. Since `T` can be arbitrarily complex and may contain
sub-classes, any event-like structure can be streamed into a ROOT file in this
way.

Check the `test_rootfile.cpp` file for more details.

## Usage

For example, making a root file with a collection of streamable instances of
clas `Event` (i.e. with the `ClassDef()` macro and corresponding link-def
`#pragma`s) is as easy as:
```c++
RootFile<Event> file("events.root", 'w');
for (...) {
  const Event ev = ...;
  file.Fill(ev);
}
```

Reading-out arbitray entry is similarly easy:
```c++
RootFile<Event> file("events.root", 'r');
const Event& ev = file.GetEntry(i);
```

Looping over entries can be done in two ways, random index access:
```c++
for(unsigned int i = 0, n = file.GetNEntries(); i < n; ++i) {
  const Event& ev = file.GetEntry(i);
  ...
}
```
or with iterators:
```c++
for (RootFile<Event>::Iterator it = file.Begin(), end = file.End();
     it != end; ++it) {
  const Event& ev = *it;
  ...
}
```

Note that the reading of the root file is instead of the
[`TTree`](http://root.cern.ch/root/html/TTree.html) implemented with the
[`TChain`](http://root.cern.ch/root/html/TChain.html) so globbing in file name
chains files together to give seamless collection of objects:
```c++
RootFile<Event> file("event-*.root", 'r');
```
