RootFile
===

`RootFile<T>` is a simple templated class that elegantly handles
reading/writing of root files containing only one [ROOT](http://root.cern.ch)
`TTree` with only one branch of class `T`. Since `T` can be arbitrarily complex
and may contain sub-classes, any event-like structure can be streamed into a
ROOT file in this way.

Check the `test_rootfile.cpp` file for more details.
