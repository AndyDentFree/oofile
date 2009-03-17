AAreadme.txt
-----
It is part of A.D. Software's software engineering standards to
place an AAreadme.txt file in every directory of any importance.
If a file is missing it is to be considered a bug, unless the
directory is covered by a file in a parent directory.
-----

About 'source'
(last mod 99/04/03)

This is the root directory of the OOFILE source code.

Depending on the package you purchase you may not have all
these directories

DIRECTORY STRUCTURE
===================
source:
	core:
		platform:
			Mac:
			MSWin:
	ctree:
	dbase:
	graphs:
		MSWin:
		PPlant:
	GUI:
		Mac:
			PPlant:
				Extras:
		MSWin:
			MFC:
				DLLStuff:
					res:
	hpp:
	rep_char:
	reports:
		Mac:
			PPlant:
		MSWin:
			MFC:
			res:
	utils:


DESCRIPTION OF MAIN DIRECTORIES
===============================

core
 main OOFILE database engine, array, string and
 file-handling classes
 
ctree
 support for Faircom's c-tree Plus ISAM engine
 (sold as part of OOFILE Professional)
  
dbase
 our own dBaseIII+ and dBase-IV compatible backend
 (sold as part of OOFILE Personal & OOFILE Professional)
 
graphs
 Mac and Windows graphing engine (common code)
 
GUI
 Mac and Windows GUI framework integration
 
hpp
 collection of old .hpp headers that forward to the .h
  headers we use nowadays (legacy support for early clients)
  
rep_char
 old report-writer providing simple columnar output to text
 and HTML (including wrapped-text and tables)
 
reports
 Mac and Windows GUI reports plus RTF, HTML and XML output
 
utils
 rendering utilities used by graphs and reports
 

PACKAGES
========

OOFPer - OOFILE Personal DB
 core
 dbase

OOFPro - OOFILE Professional DB
 core
 ctree
 dbase
 rep_char

OOFPP - OOFILE PowerPlant GUI Integration
 GUI/Mac

OOFMFC - OOFILE MFC GUI Integration
 GUI/MSWin
 
OOFGraph - OOFILE Graphing Standalone
 core
 graphs
 utils
 
OOFRepMac - OOFILE Mac Report Writer
 core
 graphs
 rep_char
 reports/Mac
 utils
 
OOFRepWin - OOFILE Windows Report Writer
 core
 graphs
 rep_char
 reports/MSWin
 utils
 
OOFRepGUI - OOFILE Mac/Win Report Writer
 core
 graphs
 rep_char
 reports
 utils
 
OOFMacBundle - OOFILE Mac Platform Bundle
 core
 ctree
 dbase
 graphs
 GUI/Mac
 rep_char
 reports/Mac
 utils
 
OOFWinBundle - OOFILE Windows Platform Bundle
 core
 ctree
 dbase
 graphs
 GUI/MSWin
 rep_char
 reports/MSWin
 utils
 
OOFGUIBundle - OOFILE Mac/Win Platform Bundle
 core
 ctree
 dbase
 graphs
 GUI
 rep_char
 reports
 utils
 

