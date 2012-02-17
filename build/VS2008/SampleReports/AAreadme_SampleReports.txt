************** UNFINISHED - NEEDS OOFILE SOURCE ADDING BACK IN *******************


AAreadme.txt
-----
It is part of A.D. Software's software engineering standards to
place an AAreadme.txt file in every directory of any importance.
If a file is missing it is to be considered a bug, unless the
directory is covered by a file in a parent directory.
-----

About 'Sample Reports'
(last mod 2012/02/17)

This project contains targets to build an OOFILE sample showing most reports, using an MFC GUI.

It was back-ported from the SampleReportsStandalone.dsw project converted to Visual Studio 2008.


WARNING
If you get an assert at startup, check that the oofmfcre.dll is
in the same folder as the executable, or in your system.

It is a minor limitation of OOFILE currently that the GUI resources
must be in a DLL. If the OOFILE GUI library has built a DLL, the
resources will be included. If a static build of the GUI classes
are used then they try to load the DLL at startup.