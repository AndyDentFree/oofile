/**
@file doxyAAAGroups.h

 GROUP DEFINITIONS so Doxygen will provide separate index pages to packages
 other groups may be defined in more suitable headers for specific areas
 This file is encountered before others so some groups are here 
 to make sure top-level groups are in the right order.
*/


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

