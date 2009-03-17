// COPYRIGHT 2001 A.D. Software, All rights reserved

/**
	\file doxyoof.h  Documentation file for Doxygen.
	define macros here to force doxygen to include in generated docs
	as of 1.2.9.1 Doxygen doesn't handle #ifndef when parsing 
	and gives warnings for \\def directives for #defines that
	are not defined in the source. Most of the following are defined
	conditionally based on external settings or are defined by 
	the programmer to control how OOFILE is built.
	
	Also includes quick class documentation for classes that show up
	as bases so the annotated.html list never has undocumented classes.
	
	\warning you should never include this file in your source
*/
#error this is a documentation file only, do not include in compilation

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


// GROUP DEFINITIONS so Doxygen will provide separate index pages to packages
// other groups may be defined in more suitable headers for specific areas
// This file is encountered before others so some groups are here 
// to make sure top-level groups are in the right order.

/**
	\defgroup appAuthor Application Authoring
	Classes to assist in writing applications.
	Often provide cross-platform abstractions or common utilities which are
	not always provided by earlier compiler versions on the 15 or more platforms
	used with OOFILE.
*/


/**
	\defgroup oofDatabase OOFILE Database package
	\anchor database_anchor
	The complete database API.	
	Included in the Open Source OOFILE.
	\par
	Macros DECLARE_REF, DECLARE_SET and OOFILE_METHODS are the most
	important used to declare persistent databases.
*/


// put this here so on the Module page it's immediately below the oofDatabase group
/**
	\defgroup oofGUI OOFILE GUI forms interface.
	\anchor gui_anchor
	Forms interface to MFC and PowerPlant application frameworks.
	Included in the Open Source OOFILE.
*/


/**
	\defgroup Presentation Presentation for OOFILE Databases
	\anchor presentation_anchor
	Packages used to display or provide some formatted presentation of data.
	\par
	Future additions may include specialist web output, apart from the 
	existing HTML and XML rendering in the report writer.
*/


/**
	\defgroup oofInternals OOFILE Implementation Classes
	Separately grouped so you can quickly see what are private classes.
	Unlike experimental, will stay around for OOFILE versions so safe to
	use to implement OOFILE but not public classes. Often will have private
	constructors or be declared nested to limit their use.
	Included in the Open Source OOFILE.
*/


/**
	\defgroup oofUtils OOFILE utility classes and functions.
	Included in the Open Source OOFILE.
	\ingroup appAuthor
*/


/**
	\defgroup oofMacros OOFILE macros
	Collection of all important macro definitions regardless of package.
	Other macros are under the subgroup DLL Export macros.
	\ingroup appAuthor
*/

/**
	\defgroup oofXML OOFILE XML support.
	Includes tools for reading and writing XML in general
	and generating XML Schema descriptions and reading and  writing OOFILE
	databases as XML.
	Included in the Open Source OOFILE.
	\ingroup appAuthor
*/


/**	
	\defgroup experimental Experimental code, may change considerably.
	Used to mark code during development and which sometimes has to be
	left in release versions, but which may change interfaces or implementation.
	Very much a "use at own risk".
*/



/**	
	\defgroup oofMacSpecific Mac Specific
	User code will often wrap in #ifdef _Macintosh
	Only ever used on Macintosh.
	There should be very few entries in this group as they are explicitly to
	be avoided. Printing and previewing support functions for platform-specific
	frameworks are the main area in which these items occur.
*/



/**	
	\defgroup oofWinSpecific MS Windows Specific
	User code will often wrap in #ifdef _Windows
	Only ever used on MS Windows.
	There should be very few entries in this group as they are explicitly to
	be avoided. Printing and previewing support functions for platform-specific
	frameworks are the main area in which these items occur.
*/


/**	
	\defgroup oofUnixSpecific Unix Specific
	User code will often wrap in #ifdef _Unix
	Only ever used on Unix.
	There should be very few entries in this group as they are explicitly to
	be avoided. File handling is the main area in which these items occur.
*/


/**	
	\defgroup yetToDoc Not Yet Documented
	Identified as to be documented ASAP but haven't finished the descriptions.
*/

// read comment at top of file to see why these are defined here!
#define OOFILE_CREATE_DLL
#define OOFREP_CREATE_DLL
#define OOFGRAPH_CREATE_DLL
#define OOFMFC_STATIC
#define OOF_EXCEPTIONS_AVAILABLE
#define OOF_SUPPRESS_EXCEPTIONS
#define OOF_USING_EXCEPTIONS
#define OOF_NO_STDLIB
#define OOF_READ_XML
#define OOF_READ_REPORTS
#define OOF_RTTI_ENABLED
#define OOF_HAS_BOOL
#define OOF_DEBUG_LOG_NAME
#define OOF_PP17_TEXTEDIT 
#define OOF_PP18_TEXTEDIT 
#define OOF_MacStyleEnums 
#define OOF_PRECARBON_PP_PRINTING
#define OOF_REP_NO_GUI

#define OOF_MEM_DEBUG_LAST_INCLUDE
#define OOF_MEM_DEBUG_FORCE_POOL_CHECK
#define OOF_MEM_DEBUG_REPORT_LEAKS
#define OOF_MEM_DEBUG_FORGET_LEAKS
#define OOF_INVARIANT_DUMP_NAME



/**
	\def OOF_MEM_DEBUG_LAST_INCLUDE
	Define as a file name to have a file included that is used for memory debugging.
	We rely on the programmer to make sure this include is the last one.
	\see oofDebugNew.h oofSmartheap.h
	\ingroup oofMacros
*/


/**
	\def OOF_DEBUG_FORCE_POOL_CHECK
	Define that's expanded in hundreds of places through OOFILE code to check the pool.
	If defined, this should invoke an assert, eg: from oofSmartheap.h
	#define OOF_DEBUG_FORCE_POOL_CHECK assert(MemPoolCheck(MemDefaultPool));
	\ingroup oofMacros
*/


/**
	\def OOF_MEM_DEBUG_REPORT_LEAKS
	Report all accumulated leaks (defined initially for DebugNew).
	Mechanism for reporting will vary depending on debug environment.
	\see oofDebugNew.h
	\ingroup oofMacros
*/


/**
	\def OOF_MEM_DEBUG_FORGET_LEAKS
	Wipe out any accumulated leaks (defined initially for DebugNew).
	\see oofDebugNew.h
	\ingroup oofMacros
*/


/**
	\def OOF_INVARIANT_DUMP_NAME
	Define this as a valid filename to cause detailed logging.
	Causes OOF_simpleRecordBackend::Invariant to call OOF_simpleRecordBackend::InvariantDump.
	Which results in the dump being called on entry to and exit from most OOFILE backend
	methods that affect the database.
	\ingroup oofMacros
*/


/**
	\class expatppNesting
	Public domain expat wrapper with easy sub parser definition.
	Written for the OOFILE Report Writer to make it much easier
	to break up the parsing logic. Automatically returns to calling
	parser when tag completes.
	\par
	See OOF_XMLrepParser::startElement and the virtual methods it
	calls such as OOF_XMLrepParser::startElem_report for how simple
	it is to make a new parser active.
	\ingroup oofRepRead
*/


/**
	\class Rect
	Macintosh structure for a rectangle.
*/


//=============== M F C  C l a s s e s ==============
/**
	\class CButton
	MFC base class for a button or checkbox.
*/


/**
	\class CComboBox
	MFC base class for a popup menu.
*/


/**
	\class CDialog
	MFC base class for a dialog, may be used for single tab panel.
*/


/**
	\class CListBox
	MFC base class for a simple scrolling list.
*/


/**
	\class CListCtrl
	MFC base class for a scrolling list with more features.
*/


/**
	\class CEdit
	MFC base class for a text entry control.
*/


/**
	\class COleDropTarget
	MFC mixin class for a target of drag and drop.
*/


/**
	\class CScrollView
	MFC base class for a scrollable window.
*/


/**
	\class CTreeCtrl
	MFC base class for a hierarchical or tree list box.
*/


/**
	\class CTreeCtrlEx
	MFC public domain extended tree we use for extra selection logic.
*/

//=============== P o w e r P l a n t   C l a s s e s ==============
/**
	\class LAttachment
	PowerPlant mixin used to intercept events on controls.
	Because we can use these, the PowerPlant forms integration doesn't
	need to subclass the way the MFC version does and is much more
	flexible.
*/


/**
	\class LDialogBox
	PowerPlant base class for a dialog box (fairly recent, was LGADialog).
*/


/**
	\class LEditField
	PowerPlant base class for small text entry.
*/


/**
	\class LListener
	PowerPlant mixin class for listeners, like oofBroadcaster.
*/


/**
	\class LOutlineItem
	PowerPlant base class for managing an item in a hierarchical tree.
*/


/**
	\class LPane
	PowerPlant base class for drawable item.
*/


/**
	\class LPeriodical
	PowerPlant base class for time controller eg: for timeouts.
*/



/**
	\class LTableStorage
	PowerPlant base class for abstract storage of data on LTableView.
*/


/**
	\class LView
	PowerPlant base class for scrollable display.
*/


/**
	\class LWindow
	PowerPlant base class for scrollable, resizable window.
*/
