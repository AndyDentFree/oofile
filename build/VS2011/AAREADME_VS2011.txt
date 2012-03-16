These solutions were built starting with Visual Studio 2011 from the Feb 29 2012 beta release.

One aim was to start experimenting with the native C++ unit testing built into that version.

CoreUnitTests is a solution unit-testing the core OOFILE logic, following the steps in 
http://msdn.microsoft.com/en-us/library/hh270864(v=vs.110).aspx

CoreUnitTests was created as a solution with the project wizard for Visual C++ - Native Unit Test Project
oofCore was created with the wizard for a Win32 Project
- DLL Style
- Export Symbols
- Security Development Lifecycle (SDL) checks