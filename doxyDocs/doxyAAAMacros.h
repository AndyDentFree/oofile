/**
@file doxyAAAMacros.h

	define macros here to force doxygen to include in generated docs
	as of 1.2.9.1 Doxygen doesn't handle #ifndef when parsing 
	and gives warnings for \\def directives for #defines that
	are not defined in the source. Most of the following are defined
	conditionally based on external settings or are defined by 
	the programmer to control how OOFILE is built.

*/
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


// and document some macros!


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