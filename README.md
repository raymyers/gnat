# GNAT - Graphical AI Tracer

A Graph editor and tracer.

## Building

The following have been tested on Linux and Windows. OSX would likely work as well.

* Download the latest Qt SDK.

* run "qmake -o Makefile gnat.pro" to create the Makefile

* run "make" to build in debug mode

* run "make release" to build in release mode

For Windows: If not built statically, gnat.exe will require several dll files in the same directory. 
These can be found in the Qt SDK installation. The path to the dlls looks like "C:\QtSDK\Desktop\Qt\4.7.4\mingw\bin".

## Authors

Designed by [Dr. Daniel Tauritz](http://web.mst.edu/~tauritzd/).

Written by (in chronological order) Bob Buehler, Matt Dissinger, Kyle Owen and [Ray Myers](http://cadrlife.com).

This program uses the Qt library, which is licensed under the LGPL.
