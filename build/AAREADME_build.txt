This build area is being incrementally polished to provide a reasonable experience for
people using current compilers. That's VS2010 and XCode 4.3 as of Feb 2012
(with some experimental work on VS2011 in Mar 2012).

Older projects are being added as they are unearthed but may not be verified.
There are (Macintosh) Codewarrior projects in our archives but as that's sadly a dead 
compiler will not be added unless by request.

Older projects made heavy use of compiling OOFILE into libraries because the compilers and
computers of 20 years ago were vastly slower. We may create a DLL project if needed for
deployment reasons but currently will just have one big project for all of OOFILE.

One source of confusion may be the SampleReports projects. These were basically standalone
projects that included enough OOFILE core for people to play with the report-writer. This
included some of the GUI classes (eg: the MFC GUI for the Windows sample).

Our preferred unit testing environment is now Google Test aka gtest and new tests will 
be added using that API. The original sample code, which formed a manual regression test suite,
may be eventually migrated over to use gtest.

One very big change starting in 2012 is refactoring the code out into many separate files.
The original small number of files made makefiles easier to write but with the limits of
today's editing environments makes code navigation a lot harder.

Most of OOFILE was written using the Smalltalk-style browser of Object Master which 
allowed for thinking at the class level regardless of how classes were stored in the files
and is very sadly missed - nothing in "modern" editors or IDEs compares.