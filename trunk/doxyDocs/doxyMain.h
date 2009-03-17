/**
@file doxyMain.h
Pure documentation header for Doxygen.
*/

/**
	\mainpage <a href="http://www.oofile.com.au/">OOFILE</a> REFERENCE MANUAL
	Updated to include every class as of November 2001.

	See the <a href="modules.html">modules page</a> for a structured breakdown of all the packages in
	OOFILE which includes every class. 
	The  <a href="annotated.html">Compound List</a> lists every class with their brief descriptions.
	\section core Core Database
	dbConnect, dbTable and dbField form the persistent hierarchy.
	dbView is often used in presenting lists of records.
	dbRelRefBase and dbRelationship are used to create relationships 
	between dbTables. 
	\par
	\ref standaloneFields_anchor "Standalone fields" allow you to declare
	local, non-persistent variables used with interfaces that take dbField.
	\par
	The \ref database_anchor "oofDatabase module" lists other database classes.

	\section GUI GUI Forms Interface
	Windows for entry and display of OOFILE data are managed by dbGUI.
	It uses helpers such as dbAbstractEditHelper that manage lists of
	dbFieldLink objects linking onscreen controls to database dbField's.
	Lists of records are managed by dbAbstractBrowseHelper.
	\par
	 The \ref gui_anchor "OOFILE GUI forms interface module" lists other GUI classes.


	\section allpresent Presentation
	\ref presentation_anchor "Presentation" frameworks provide display
	and printing of OOFILE data.
	
	\subsection rep Report Writer
	The banding report writer is an oofRep.
	It is rendered in different oofRepEnvironment's by a family of
	oofRepRender objects. 
	Individual reports are configured by oofRepSettings and the defaults
	by oofRepDefaultSettings.

	\subsection charRep Simple Character Report Writer
	A family of simple character-mode report writers descend from dbRep.
	They are most useful for quick dumps of one database table to HTML.

	\subsection graph Graphing
	The graphing engine is oofGraph which draws series of data contained
	in oofSeriesSet using oofGeo objects for actual drawing.
	Individual graphs are configured by oofGraphSettings and the defaults
	by oofGraphDefaultSettings.
*/
