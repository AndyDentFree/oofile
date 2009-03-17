#ifndef H_OOFMACRO
#define H_OOFMACRO

// COPYRIGHT 2000 A.D. Software, All rights reserved

/** 
	\file oofmacro.h
	macros that define user tables and relationship classes.
	included via oofile.h
*/


#ifndef H_OOFRAM
	#include "oofram.h"
#endif

/** 
   common body of relationship tables.
   Can be used by itself, if you want to add more members to the relationship field.
   Most commonly used inside other macros:
   \see DECLARE_REF
   \see DECLARE_SET
   \ingroup oofMacros
*/
#define REL_REF_BODY(tb) \
public: \
tb* operator->() { return (tb*) relatedTable();} \
operator tb*() { return (tb*) relatedTable();} \
operator tb&() { return *((tb*) relatedTable());} \
unsigned long countAllRelatedIn(tb* rhs) { return CountAllRelatedIn((dbTable*)rhs); } \
unsigned long countAllRelatedIn(tb& rhs) { return CountAllRelatedIn((dbTable*)&rhs); } \
virtual bool pointsToCorrectTableType() const { return strcmp(#tb, mRelatedTableProto->defaultName())==0; }


/** 
      define related tables at end of 1:1 or 1:N relationship.
	   \ingroup oofMacros
*/
#define DECLARE_REF(tb) \
class tb; \
class tb ## Ref : public dbRelRef {\
REL_REF_BODY(tb) };

/** 
      define related tables at end of N:1 or N:M relationship.
	   \ingroup oofMacros
*/
#define DECLARE_SET(tb) \
class tb; \
class tb ## Set : public dbRelSet { \
REL_REF_BODY(tb) };



/**
	\def OOF_DEBUG_LOG_NAME
	Define this as a filename to build a logging version of OOFILE .
	\warning can generate very large files and run quite slowly.
	\ingroup oofMacros
*/

#ifdef OOF_DEBUG_LOG_NAME
// ----- start of macros with debug logging
/** 
	 methods used by most OOFILE tables which have pure virtuals.
	 Use instead of OOFILE_METHODS for a base which must be subclassed
	 as OOFILE_METHODS calls constructors which will give compiler errors
	 \see file::ooftst53.h
	\ingroup oofMacros
*/
#define ABSTRACT_BASE_METHODS(tb) \
public: \
virtual const char* defaultName() const { return #tb ;} \
tb&	operator[](const dbQueryClause& rhs) { search(rhs); return *this; } \
tb&	operator[](unsigned long index) {	gotoRecord(index); return *this;} \
tb&	operator[](long index) { assert(index>=0); gotoRecord(index); return *this;} \
virtual dbTable* cloneTableWithoutSelection() const { StandardDebugLog("dbTable::cloneTableWithoutSelection()"); return cloneTableConst(selNotShared); } \
virtual dbTable* cloneTableSharingSelection() const { StandardDebugLog("dbTable::cloneTableSharingSelection()"); return cloneTableConst(selShared); } \
virtual dbTable* cloneTbleWithCachNoSelection() const { StandardDebugLog("dbTable::cloneTbleWithCachNoSelection()"); return cloneTableConst(selNotSharedCacheCopied); } \
tb* cloneWithoutSelection() const { return (tb*) cloneTableConst(selNotShared); } \
tb* cloneSharingSelection() const { return (tb*) cloneTableConst(selShared); } \
tb* cloneWithCachNoSelection() const { return (tb*) cloneTableConst(selNotSharedCacheCopied); } \
tb& operator-(tb& rhs) { dbTable::difference_with(rhs); return *this; } \
tb& difference_with(tb& rhs) { dbTable::difference_with(rhs); return *this; } \
tb& operator&(tb& rhs) { dbTable::intersection_with(rhs); return *this; } \
tb& intersection_with( { dbTable::intersection_with(rhs); return *this; } \
tb& operator|(tb& rhs) { dbTable::union_with(rhs); return *this; } \
tb& operator+(tb& rhs) { dbTable::union_with(rhs); return *this; } \
tb& union_with(tb& rhs) { dbTable::union_with(rhs); return *this; } \
tb& operator=(const tb& rhs) { copyRecFrom(&rhs); return *this; } \
friend ostream& operator<<(ostream& os, tb& tbl) {tbl.extract(os); return os;} \
friend ostream& operator<<(ostream& os, tb* tbl) {tbl->extract(os); return os;} \
friend istream& operator>>(istream& is, tb& tbl) {tbl.insert(is); return is;} \
friend istream& operator>>(istream& is, tb* tbl) {tbl->insert(is); return is;} 

/**
	define main methods for type-safe user tables.
	We use a macro so the return type of many calls is the actual table.
	It's possible but difficult to use a dbTable subclass lacking this macro.
	\ingroup oofMacros
*/
#define OOFILE_METHODS(tb) \
ABSTRACT_BASE_METHODS(tb) \
virtual dbTable* cloneTableConst(selSharingT selSharing) const {  \
	assert(selSharing!=selCopyOnWrite || isTableValid()); \
	OOF_dbTableStatics::sCloningSelSharing = selSharing; \
	OOF_dbTableStatics::sCloning = true; \
	tb* ret = new tb(*this); \
	ret->postCloneTableCleanup(); \
	return ret; } \
virtual dbTable* cloneTable(selSharingT selSharing=selCopyOnWrite) {  \
	if (selSharing==selCopyOnWrite) \
		ensureSelectionLoaded(); \
	return cloneTableConst(selSharing); } \
tb* clone(selSharingT selSharing=selCopyOnWrite) const { return (tb*) cloneTableConst(selSharing); } \
tb* cloneEmptyInRAM() const {  \
	StandardDebugLog("dbTable::cloneEmptyInRAM()"); \
	dbConnect_ram tempConnect; \
	OOF_dbTableStatics::sCloningInDifferentConnection = true; \
	tb* ret = new tb(*this); \
	tempConnect.newConnection(); \
	return ret; } \
tb* cloneInRAM() {  \
	StandardDebugLog("dbTable::cloneInRAM()"); \
	tb* ret = cloneEmptyInRAM(); \
	ret->copyAllFrom(this); \
	return ret; }


// ----- end of macros with debug logging
#else
// ----- start of macros with no logging

// NOTE the default copy ctor will work, see the dbTable copy ctor
// for how it works behind the scenes

#define ABSTRACT_BASE_METHODS(tb) \
public: \
virtual const char* defaultName() const { return #tb ;} \
tb&	operator[](const dbQueryClause& rhs) { search(rhs); return *this; } \
tb&	operator[](unsigned long index) {	gotoRecord(index); return *this;} \
tb&	operator[](long index) {	assert(index>=0); gotoRecord(index); return *this;} \
virtual dbTable* cloneTableWithoutSelection() 		const {  return cloneTableConst(selNotShared); } \
virtual dbTable* cloneTableSharingSelection() 		const { return cloneTableConst(selShared); } \
virtual dbTable* cloneTbleWithCachNoSelection() 	const { return cloneTableConst(selNotSharedCacheCopied); } \
tb* cloneWithoutSelection() 		const { return (tb*) cloneTableConst(selNotShared); } \
tb* cloneSharingSelection()			const { return (tb*) cloneTableConst(selShared); } \
tb* cloneWithCachNoSelection() 	const { return (tb*) cloneTableConst(selNotSharedCacheCopied); } \
tb& operator-(tb& rhs) { dbTable::difference_with(rhs); return *this; } \
tb& difference_with(tb& rhs) { dbTable::difference_with(rhs); return *this; } \
tb& operator&(tb& rhs) { dbTable::intersection_with(rhs); return *this; } \
tb& intersection_with(tb& rhs) { dbTable::intersection_with(rhs); return *this; } \
tb& operator|(tb& rhs) { dbTable::union_with(rhs); return *this; } \
tb& operator+(tb& rhs) { dbTable::union_with(rhs); return *this; } \
tb& union_with(tb& rhs) { dbTable::union_with(rhs); return *this; } \
tb& operator=(const tb& rhs) { copyRecFrom(&rhs); return *this; } \
friend ostream& operator<<(ostream& os, tb& tbl) {tbl.extract(os); return os;} \
friend ostream& operator<<(ostream& os, tb* tbl) {tbl->extract(os); return os;} \
friend istream& operator>>(istream& is, tb& tbl) {tbl.insert(is); return is;} \
friend istream& operator>>(istream& is, tb* tbl) {tbl->insert(is); return is;} 

// the above friend operator<< etc. are not defined as friends because they
// need friend access, but so that they can be defined inside the
// OOFILE_METHODS macro. Otherwise we'd need a macro to close the table 
// definition, as we'd normally declare their prototypes outside the class


#define OOFILE_METHODS(tb) \
ABSTRACT_BASE_METHODS(tb) \
virtual dbTable* cloneTableConst(selSharingT selSharing) const {  \
	assert(selSharing!=selCopyOnWrite || isTableValid()); \
	OOF_dbTableStatics::sCloningSelSharing = selSharing; \
	OOF_dbTableStatics::sCloning = true; \
	tb* ret = new tb(*this); \
	ret->postCloneTableCleanup(); \
	return ret; } \
virtual dbTable* cloneTable(selSharingT selSharing=selCopyOnWrite) {  \
	if (selSharing==selCopyOnWrite) \
		ensureSelectionLoaded(); \
	return cloneTableConst(selSharing); } \
tb* clone(selSharingT selSharing=selCopyOnWrite) const { return (tb*) cloneTableConst(selSharing); } \
tb* cloneEmptyInRAM() const {  \
	dbConnect_ram tempConnect; \
	OOF_dbTableStatics::sCloningInDifferentConnection = true; \
	tb* ret = new tb(*this); \
	tempConnect.newConnection(); \
	return ret; } \
tb* cloneInRAM() {  \
	tb* ret = cloneEmptyInRAM(); \
	ret->copyAllFrom(this); \
	return ret; }
	
// ----- end of macros with no logging
#endif


#define DECLARE_ABSTRACT_CLASS(tb) \
class tb : public dbTable { \
ABSTRACT_BASE_METHODS(tb) 


#define DECLARE_CLASS(tb) \
class tb : public dbTable { \
OOFILE_METHODS(tb) 


/** 
	Old name for DECLARE_CLASS.
	\ingroup oofMacros
*/
#define CLASS_TABLE DECLARE_CLASS

/** 
	Old name for DECLARE_REF.
	\ingroup oofMacros
*/
#define REF_TABLE DECLARE_REF

/** 
	Old name for DECLARE_SET.
	\ingroup oofMacros
*/
#define SET_TABLE DECLARE_SET

#endif


