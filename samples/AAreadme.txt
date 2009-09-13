AAreadme.txt
-----
It is part of A.D. Software's software engineering standards to
place an AAreadme.txt file in every directory of any importance.
If a file is missing it is to be considered a bug, unless the
directory is covered by a file in a parent directory.
-----

About samples
(last mod 99/04/03)
(last mod 00/09/16)

The samples directory contains our regression test suite of small
programs used in console-mode testing.

They demonstrate one or more features of the core OOFILE database
engine and are indexed as such in the features.htm index.

The folder "server support" contains special samples for testing  
the faircom server support code. Note further testing samples can be found in the 
oofile:oofileCode:codeWarrior: folder. 

//=-=-=-=-=-=-=-=-=-=-=-
	DIR STRUCTURE AND FILES
//=-=-=-=-=-=-=-=-=-=-=-
sample:				(This Folder)
	ooftst01.cpp
	ooftst01.h
	ooftst02.cpp
	ooftst02.h
	ooftst03.cpp
	ooftst04.cpp
	ooftst05.cpp
	ooftst06.cpp
	ooftst07.cpp
	ooftst07.h
	ooftst08.cpp
	ooftst09.cpp
	ooftst10.cpp
	
	ooftst11.cpp
	ooftst12.cpp
	ooftst13.cpp
	ooftst14.cpp
	ooftst15.cpp
	ooftst16.cpp
	ooftst16.h
	ooftst17.cpp
	ooftst18.cpp
	ooftst19.cpp
	ooftst20.cpp
	
	ooftst21.cpp
	ooftst22.cpp
	ooftst23.cpp
	ooftst24.cpp
	ooftst25.cpp
	ooftst26.cpp
	ooftst27.cpp
	ooftst28.cpp
	ooftst29.cpp
	ooftst30.cpp
	
	ooftst31.cpp
	ooftst32.cpp
	ooftst33.cpp
	ooftst34.cpp
	ooftst35.cpp
	ooftst36.cpp
	ooftst37.cpp
	ooftst38.cpp
	ooftst39.cpp
	ooftst40.cpp
	
	ooftst41.cpp
	ooftst42.cpp
	ooftst42.h
	ooftst43.cpp
	ooftst44.cpp
	ooftst45.cpp
	ooftst46.cpp
	ooftst46.h
	ooftst47.cpp
	ooftst048.h
	ooftst48.cpp
	(ooftst49, ooftsts62-64, ooftst69, ooftst71 - 79 currenlty name is unused)
	ooftst50.cpp
	
	ooftst51.h
	oftst055.h
	ooftst55.cpp
	ooftst058.h
	ooftst58.cpp
	ooftst059.h
	ooftst059.cpp
	ooftst60.cpp
	
	ooftst60.h
	oofTest61.cpp		(Test harness that tests the xmltext<-->asciiText routines and objects)
	ooftst66.cpp		(server interface test see below)
	ooftst67.cpp		(This sample is used to check the functioning of the dbUserObject)
	ooftst68.cpp		(Bbased on ooftest44 but adds functionality to test creation and moving between directories)
	ooftst70.cpp		(This sample tests the functioning of the openOrCreateConnection method)
	oofts80.cpp			(this tests connection to an existing ctree data base and building schema from doda)
	ooftst81.cpp		


	SrvSamples: 	Faircom server support specific examples)
   
	    ooftst01srv.cpp (server interface cversion of ooftst01)
	    ooftst65.h.sit
	    ooftst65srv.cpp	(similar yo oftst11 but uses contacts data base)
	    ooftst65srv.h
	    ooftst66.cpp	(this sample is used to obtain a llock within a transaction on a database and
						hold it until told to release it, it's purpose is to check exception handling  on other apps)

