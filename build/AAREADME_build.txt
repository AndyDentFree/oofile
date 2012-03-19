This build area is being incrementally polished to provide a reasonable experience for
people using current compilers. That's VS2010 and XCode 4.3 as of Feb 2012
(with some experimental work on VS2011 in Mar 2012).

Windows builds use relative paths from an environment variable OOFILE_ROOT
that points to the directory containing the source, include and build directories.


General History
---------------
Older projects are being added as they are unearthed but may not be verified.
There are (Macintosh) Codewarrior projects in our archives but as that's sadly a dead 
compiler will not be added unless by request.

Older projects made heavy use of compiling OOFILE into libraries because the compilers and
computers of 20 years ago were vastly slower. We will tend toward using DLL projects due
to those being most easily used by unit test frameworks. Otherwise you are encouraged to
include OOFILE source directly into your main project.

One source of confusion may be the old SampleReports projects. These were basically standalone
projects that included enough OOFILE core for people to play with the report-writer. This
included some of the GUI classes (eg: the MFC GUI for the Windows sample). At this stage,
they must be regarded as legacy projects until new GUIs are created and they are replaced.
So, use them if you like but they will not be directly upgraded and don't necessarily reflect
recommended code organisation.

Unit Testing
------------
Our preferred unit testing environment is now Google Test aka gtest and new tests will 
be added using that API. The original sample code, which formed a manual regression test suite,
may be eventually migrated over to use gtest.

We are also exploring the new native unit testing of VS2011 introduced in the Feb 29 2012 beta.

Source file organisation
------------------------
One very big change starting in 2012 is refactoring the code out into many separate files.
The original small number of files made makefiles easier to write but with the limits of
today's editing environments makes code navigation a lot harder.

Most of OOFILE was written using the Smalltalk-style browser of Object Master which 
allowed for thinking at the class level regardless of how classes were stored in the files
and is very sadly missed - nothing in "modern" editors or IDEs compares.

The build environments used to either be able to refer to the top OOFILE directory or Windows
builds typically assumed it was installed in a root oofile directory on the same drive as the project,
so used paths like \oofile\source\core.
