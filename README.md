MatlabFile
==========

About
-----

So MATLAB 2010+ can read and write to HDF5 files.  The preferences must be set to file format version 7.3 for files
to be in HDF5 format.  One can use the C syntax of HDF to read in the contents of a file written by Matlab.  However,
when a file is written via HDF5 from C/C++, it is __not__ readable in Matlab.  Why?  Well, because Matlab expects a
header that starts like so.

    MATLAB 7.3 MAT-file

This class provides a wrapper for HDF5 to read and write files from your C++ applications such that Matlab will be
none the wiser.

Usage
-----

I've tried to create a naming convention consistent with Nvidia's CUDA APIs for single / double precision and
real / complex functions.

    MatlabFile file* = new MatlabFile();
    file->open("file.mat");

    file->readR(...) / writeR(...);  // single precision real
    file->readC(...) / writeC(...);  // single precision complex
    file->readD(...) / writeD(...);  // double precision real
    file->readZ(...) / writeZ(...);  // double precision complex

    file->close;
    delete file;

Clearly, HDF5 is required but the types are tied to CUDA's 'vector_types.h' so CUDA is also necessary, however
you may simply redefine the types by adjusting the top of 'MatlabFile.hpp'.

Example
-------

See the example folder for matlab / example C++ application.  You will need to build the example application
which requires cmake.  Then see the MatlabFileTest.m which will guide you through generating data, writing it
to file, reading it into the C++ application, mutating the data, writing it back out, then finally reading it
back into Matlab. 

License
-------

MIT License, see LICENSE.txt
