// OOFILE record-oriented abstract backend implementation
// 
// COPYRIGHT 1994 A.D. Software, All rights reserved


#include "oofpch_c.h"  // for precompilation of core files

#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFRECS
	#include "oofrecs.h"
#endif
#ifndef H_OOFWORDS
	#include "oofwords.h"
#endif
#include <ctype.h>

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

#ifdef OOF_INVARIANT_DUMP_NAME
	#ifndef _Macintosh
	// optional statics
		oofDirectory OOF_simpleRecordBackend::sInvariantDumpDir;
		ofstream OOF_simpleRecordBackend::sInvariantDump;
	#endif
#endif

// -------------------------------------------------------
//    O O F I L E _ s i m p l e R e c o r d B a c k e n d
//         see also oofrec1.cpp & oofrec2.cpp
// -------------------------------------------------------
bool 
OOF_simpleRecordBackend::search(const dbQueryClause* qClause) 
{
	assert( Invariant("simpleB::search-entry") );
	
	dbTable *fieldTableNotUs, *fieldRelatedTable;
	qClause->getSearchTables(mTable, fieldTableNotUs, fieldRelatedTable);
	if (fieldTableNotUs) {
		if (fieldRelatedTable) {
			if (fieldTableNotUs->search(qClause)) {
				const bool ret = fieldTableNotUs->relateSelectionFrom(mTable);
				assert( Invariant("simpleB::search-exit relSelFrom") );
				return ret;
			}
			else {
				selectNone();	// traversal finds nothing
				assert( Invariant("simpleB::search-exit found none") );
				return false;	// related search found nothing, no point in traversing
			}
		}
		else {
			return false;  // quick hack - safe failure of bad spec
		}
	}  // clause not directly only this table
	
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif

	bool ret = false;
	if (qClause->queryClauseType()==dbQueryClause::builtQuery)  // get the actual query
		qClause = ((dbQuery*) qClause)->query();  // safe downcast
		
	switch (qClause->queryClauseType()) {
	case dbQueryClause::trinaryFieldToLiterals :
		ret = SearchTrinaryFieldToLiterals( (dbQueryTrinary*) qClause);  // safe downcast
		break;
	
	case dbQueryClause::binaryfieldToLiteral :
		ret = SearchBinaryFieldToLiteral( (dbQueryBinary*) qClause);  // safe downcast
		break;
	
	case dbQueryClause::binaryCombination : {
		dbQueryBinaryCombo* comboClause = (dbQueryBinaryCombo*) qClause; // safe downcast
		ret = SearchCombineSubclauses(comboClause, false); 
	}
		break;
	

	case dbQueryClause::entireTable : 
		dbConnect::raise("Table keyword searches are only supported in the ctree backend", false);
		selectNone();
		break;
	
	default:
		ret = false;
	}
	gotoBOF();		// clear mCurrLoadedRecOffset which may have been set by iterators above
	unloadRecord();  // Make sure the selection context is logical before returning.
					// don't want to start() in case triggers sort, as we shouldn't 
					// presume they need the results of the search sorted (eg: may just count)

	assert( Invariant("simpleB::search-exit") );
	return ret;
}


bool 
OOF_simpleRecordBackend::checkDuplicate(const dbQueryClause*)
{
	return false;	// default for non-indexed backends
}


bool 
OOF_simpleRecordBackend::searchSelection(const dbQueryClause* qClause) 
{	
	assert( Invariant("simpleB::searchSelection-entry") );

	dbTable *fieldTableNotUs, *fieldRelatedTable;
	qClause->getSearchTables(mTable, fieldTableNotUs, fieldRelatedTable);
	if (fieldTableNotUs) {
		if (fieldRelatedTable) {
			if (fieldTableNotUs->search(qClause)) {
				dbSelection narrowingSel = &mSelection;
				if (fieldTableNotUs->relateSelectionFrom(mTable)) {
					mSelection.intersection_with(narrowingSel.internalSelection());
					assert( Invariant("simpleB::searchSelection-exit relSearch") );
					return mSelection.isEmpty();
				}
				else {
					selectNone();	// traversal finds nothing
					assert( Invariant("simpleB::searchSelection-exit noneRelated") );
					return false;	// nothing as a search selection is like AND - empty result
				}	
			}
			else {
				selectNone();	// related search found nothing, no point in traversing
				assert( Invariant("simpleB::searchSelection-exit none found in rel field") );
				return false;	// nothing as a search selection is like AND - empty result
			}
		}
		else {
			return false;  // quick hack - safe failure of bad spec
		}
	}  // clause not directly only this table
	
	
	bool ret = false;
	switch (qClause->queryClauseType()) {
	case dbQueryClause::trinaryFieldToLiterals :
		ret = SearchSelectionTrinaryFieldToLiterals( (dbQueryTrinary*) qClause);  // safe downcast
		break;
	
	case dbQueryClause::binaryfieldToLiteral :
		ret = SearchSelectionBinaryFieldToLiteral( (dbQueryBinary*) qClause);  // safe downcast
		break;
	
	case dbQueryClause::binaryCombination : {
		ret = SearchCombineSubclauses((dbQueryBinaryCombo*) qClause, true);  // safe downcast
		break;
	}
	default:
		ret = false;

	}	
	gotoBOF();		// clear mCurrLoadedRecOffset which may have been set by iterators above
	unloadRecord();  // Make sure the selection context is logical before returning.
	assert( Invariant("simpleB::searchSelection-exit") );
	return ret;
}


bool 
OOF_simpleRecordBackend::searchEqual(const dbField* schField, const void* schAddress)
{
	assert( Invariant("simpleB::searchEqual(bin)-entry") );
	
	selectAll();
	return SearchEqualNonIndexed(schField, schAddress);
}


bool 
OOF_simpleRecordBackend::searchEqual(const dbField* schField, const char* schStr, bool matchEntireKey)
{
	assert( Invariant("simpleB::searchEqual(string)-entry") );

	selectAll();
	return SearchEqualNonIndexed(schField, schStr, matchEntireKey);
}


bool 
OOF_simpleRecordBackend::SearchEqualNonIndexed(const dbField* schField, const char* schStr, bool matchEntireKey)
{
	assert( Invariant("simpleB::SearchEqualNonIndexed(string)-entry") );
	
	if (mSelection.isEmpty()) {
		assert( Invariant("simpleB::SearchEqualNonIndexed(string)-exit emptySel") );
		return false;
	}
		
	if (dbQueryClause::wildcardsInString(schStr))
		return SearchEqualNonIndexedWildcard(schField,schStr,matchEntireKey);
		
	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection(mTable->prototype());
	
	unloadRecord();
	if(matchEntireKey) { // True searchequals		
		for(start();more();next()) {  // Vertical Iteration through the selection
			if(OOF_stricmp(schField->asChars(), schStr)==0)
				newSelection.append(mCurrLoadedRecOffset);
			unloadRecord();
		}
	}
	else { // Only check if it starts with the key !
		unsigned int schLen=strlen(schStr);
		for(start();more();next()) {  // Vertical Iteration through the selection
			if(OOF_strnicmp(schField->asChars(), schStr, schLen)==0)
				newSelection.append(mCurrLoadedRecOffset);
			unloadRecord();
		}
	}
	
	mSelection = newSelection;	

	assert( Invariant("simpleB::SearchEqualNonIndexed(string)-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchWordNonIndexed(const dbField* schField, const char* schStr, bool startsWith)
{
	assert( Invariant("simpleB::SearchWordNonIndexed-entry") );

	if (mSelection.isEmpty())
		return false;
		
/*	if (dbQueryClause::wildcardsInString(schStr))
		return SearchEqualNonIndexedWildcard(schField,schStr,matchEntireKey);
NOT YET IMPLEMENTED - cope with wildcards in keywords
*/		
	oofWordParser* wp = schField->words();
	assert(wp);	// should NOT be able to specify this kind of search on a field which can't then
				// provide us with a word parser!!!
		
	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	unloadRecord();
	if (startsWith) {
		unsigned int matchLen = strlen(schStr);
		for (start();more();next()) {  // Vertical Iteration through the selection
			for (wp->start(schField->asChars()); wp->more(); wp->next()) {  // Horizontal Iteration through the field's words
				if (OOF_strnicmp(wp->word(), schStr, matchLen)==0) {
					newSelection.append(mCurrLoadedRecOffset);
					break;
				}
			} // word loop
			unloadRecord();
		} // record loop
	}  // match starts of words
	else {
		for (start();more();next()) {  // Vertical Iteration through the selection
			for (wp->start(schField->asChars()); wp->more(); wp->next()) {  // Horizontal Iteration through the field's words
				if (OOF_stricmp(wp->word(), schStr)==0) {
					newSelection.append(mCurrLoadedRecOffset);
					break;
				}
			} // word loop
			unloadRecord();
		} // record loop
	} // match full words
	
	mSelection = newSelection;	
	assert( Invariant("simpleB::SearchWordNonIndexed-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchNotEqualNonIndexed(const dbField* schField, const char* schStr, bool matchEntireKey)
{
	assert( Invariant("simpleB::SearchNotEqualNonIndexed(string)-entry") );

	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchNotEqualNonIndexed(string)-exit emptySel") );
		return false;
	}	
	if(dbQueryClause::wildcardsInString(schStr)){
		return SearchNotEqualNonIndexedWildcard(schField,schStr,matchEntireKey);
	}	
	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	unloadRecord();
	if(matchEntireKey) // True searchequals
		for(start();more();next()) {  // Vertical Iteration through the selection
			if(OOF_stricmp(schField->asChars(), schStr)!=0)
				newSelection.append(mCurrLoadedRecOffset);
			unloadRecord();
		}
	else { // Only check if it doesn't start with the key !
		unsigned int schLen=strlen(schStr);
		for(start();more();next()) {  // Vertical Iteration through the selection
			if(OOF_strnicmp(schField->asChars(), schStr, schLen)!=0)
				newSelection.append(mCurrLoadedRecOffset);
			unloadRecord();
		}
	}
	
	mSelection = newSelection;	

	assert( Invariant("simpleB::SearchNotEqualNonIndexed(string)-exit") );
	return !mSelection.isEmpty();
}

bool
OOF_simpleRecordBackend::SearchEqualNonIndexedWildcard(const dbField* schField, const char* schStr, bool matchEntireKey)
{
	assert( Invariant("simpleB::SearchEqualNonIndexedWildcard-entry") );
	
	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchEqualNonIndexedWildcard-exit emptySel") );
		return false;
	}	
	unsigned short numwcs=0;
	WC wc[100]; // Yes - redo this without the static storage !
	
	char *thestr=new char[strlen(schStr)+1];
	strcpy(thestr,schStr);
	
	// Parse the wildcard string
	wc[0].thewc=prefix;
	wc[0].thechars=thestr;
	for(char *s=thestr;*s;s++)
		if (*s == dbQueryClause::sWildcardMultiple) {
			numwcs++;
			wc[numwcs].thewc=multiple;
			wc[numwcs].thechars=s+1;
			*s='\0';
			// Special cases (*sigh*)
			//
			// Are we prefixed by another of ourself,
			// in which case we are meaningless !
			if((numwcs>1)&&(*(wc[numwcs-1].thechars)=='\0')&&(wc[numwcs-1].thewc==multiple)){
				numwcs--;
				wc[numwcs].thechars=s+1;					
			}
			// Are we the last character ?
			if(*(s+1)=='\0') {
				wc[numwcs].thewc=multiplefinal;				
			}
		  }
		  else if (*s == dbQueryClause::sWildcardSingle) {
				numwcs++;
				wc[numwcs].thewc=single;
				wc[numwcs].thechars=s+1;
				*s='\0';
				// Special cases (*sigh*)
				//
				// Are we prefixed by a multiple ('*'),
				// in which case we should go first !
				if((numwcs>1)&&(*(wc[numwcs-1].thechars)=='\0')&&(wc[numwcs-1].thewc==multiple)){
					wc[numwcs].thewc=multiple;					
					wc[numwcs-1].thewc=single;					
				}
				// Are we the last character ?
				if(*(s+1)=='\0') {
					if(wc[numwcs].thewc==multiple)
						wc[numwcs].thewc=multiplefinal;
					else		
						wc[numwcs].thewc=singlefinal;
				}		
			  }
				
	if(!matchEntireKey) // We got called as a startsWith !
		if(!(wc[numwcs].thewc==multiplefinal)) { // if we already have a '*' on the end of the string - do nothing
			if(wc[numwcs].thewc==singlefinal)	 // If we had a trailing '*', it's no longer trailing
				wc[numwcs].thewc=single;
			numwcs++;
			wc[numwcs].thewc=multiplefinal;  						// Make a new final string
			wc[numwcs].thechars=&thestr[strlen(thestr)]; // point the trailer at a null !
	}
	
	int startingWC;
	
	// OK - Lets do the thing - I have a couple of performance heuristics that go here...
	if(*(wc[0].thechars)=='\0')
		startingWC=1;
	else {
		startingWC=0;
		if (CanUseIndex(schField))
			searchEqual(schField, wc[0].thechars, false);  // 1) Do an indexed startsWith search - this will cut down our non-indexed iteration
	}
	
	if((numwcs==1)&&(wc[1].thewc==multiplefinal)){
		// We have a case where there is simply a prefix string (or maybe even not) and just a '*'
		if(startingWC==0) // We have a non-empty prefix string
			if (!CanUseIndex(schField)){
				// We have already doe a searchequal for the indexed case - now we just do the non-indexed verison
				SearchEqualNonIndexed(schField, wc[0].thechars, false);
			}
		// OK - we're done !
		delete[] thestr;
		assert( Invariant("simpleB::SearchEqualNonIndexedWildcard-exit trailingWC") );
		return !mSelection.isEmpty();
	}  // optimal case - trailing wildcard
	
	OOF_recordSelection newSelection( mTable->prototype() );

	unloadRecord();
	for(start();more();next()){  // Vertical Iteration through the selection
	
		int curwc=startingWC;
		int curchar=0;
		int curskipped=0;
				
		const oofString fldContents=schField->copyString();  //Retrieve the current field contents
		const char *s=fldContents;
		
		while(*s){
			if(wc[curwc].thewc==single) {
				if(!curskipped) {
					s++;
					curskipped++;
				} else if(*(wc[curwc].thechars)=='\0'){
					// singular folowed by a null literal is a match always
					// BUT we don't advance the pointer in this case !
					curwc++;
					curskipped=0;
				} else if (toupper(*s)==toupper(wc[curwc].thechars[curchar])) {
					// singular followed by a real literal is a match !
					s++;
					curchar++;
					if(!wc[curwc].thechars[curchar]) { // End of matched literal
						curwc++;
						if(curwc>numwcs) // We have run out of literals
							break;
						curchar=0;
						curskipped=0;
					}
				} else { // filed to match
					curskipped++;
					if(curskipped>1) // failed to match more than once !
						break;
				}
			} else if(wc[curwc].thewc==singlefinal) { 
				// We have a trailing '?'
				s++;
				curwc++;
				break; // We skipped the '?', so we can leave the rest to the final test
			} else if(wc[curwc].thewc==multiplefinal) { 
				// We have a trailing '*' so we match from here automatically
				curwc++;
				s = "";  // point to local null so loop termination works below, which is all that the above achieved
				break;
			} else if(toupper(*s)==toupper(wc[curwc].thechars[curchar])) { // Matched character
				s++;
				curchar++;
				curskipped=0;
				if(!wc[curwc].thechars[curchar]) { // End of matched literal
					curwc++;
					if(curwc>numwcs) // We have run out of literals
						break;
					curchar=0;
				} 
			} else { // Character didn't match
				if(curchar>0) { // are we currently matching a literal ?
					s-=(curchar-1);
					curchar=0;
				} else {
					s++;
					curskipped++;
				}
			}
		}
		
		if((curwc>numwcs)&&!(*s))
			newSelection.append(mCurrLoadedRecOffset);
			
		unloadRecord();
	}
	
	delete[] thestr;

	mSelection = newSelection;	
	assert( Invariant("simpleB::SearchEqualNonIndexedWildcard-exit") );
	return !mSelection.isEmpty();
}

bool
OOF_simpleRecordBackend::SearchNotEqualNonIndexedWildcard(const dbField* schField, const char* schStr, bool matchEntireKey)
{
	assert( Invariant("simpleB::SearchNotEqualNonIndexedWildcard-entry") );

	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchNotEqualNonIndexedWildcard-exit emptySel") );
		return false;	
	}	
	unsigned short numwcs=0;
	WC wc[100]; // Yes - redo this without the static storage !
	
	char *thestr=new char[strlen(schStr)+1];
	strcpy(thestr,schStr);
	
	// Parse the wildcard string
	wc[0].thewc=prefix;
	wc[0].thechars=thestr;
	for(char *s=thestr;*s;s++)
		if (*s == dbQueryClause::sWildcardMultiple) {
			numwcs++;
			wc[numwcs].thewc=multiple;
			wc[numwcs].thechars=s+1;
			*s='\0';
			// Special cases (*sigh*)
			//
			// Are we prefixed by another of ourself,
			// in which case we are meaningless !
			if((numwcs>1)&&(*(wc[numwcs-1].thechars)=='\0')&&(wc[numwcs-1].thewc==multiple)){
				numwcs--;
				wc[numwcs].thechars=s+1;					
			}
			// Are we the last character ?
			if(*(s+1)=='\0') {
				wc[numwcs].thewc=multiplefinal;				
			}
		  }
		  else if (*s == dbQueryClause::sWildcardSingle) {
				numwcs++;
				wc[numwcs].thewc=single;
				wc[numwcs].thechars=s+1;
				*s='\0';
				// Special cases (*sigh*)
				//
				// Are we prefixed by a multiple ('*'),
				// in which case we should go first !
				if((numwcs>1)&&(*(wc[numwcs-1].thechars)=='\0')&&(wc[numwcs-1].thewc==multiple)){
					wc[numwcs].thewc=multiple;					
					wc[numwcs-1].thewc=single;					
				}
				// Are we the last character ?
				if(*(s+1)=='\0') {
					if(wc[numwcs].thewc==multiple)
						wc[numwcs].thewc=multiplefinal;
					else		
						wc[numwcs].thewc=singlefinal;
				}		
			  }
			  		
	if(!matchEntireKey) // We got called as a startsWith !
		if(!(wc[numwcs].thewc==multiplefinal)) { // if we already have a '*' on the end of the string - do nothing
			if(wc[numwcs].thewc==singlefinal)	 // If we had a trailing '*', it's no longer trailing
				wc[numwcs].thewc=single;
			numwcs++;
			wc[numwcs].thewc=multiplefinal;  						// Make a new final string
			wc[numwcs].thechars=&thestr[strlen(thestr)]; // point the trailer at a null !
	}
	
	int startingWC;
	if(*(wc[0].thechars)=='\0')
		startingWC=1;
	else
		startingWC=0;
	
	if((numwcs==1)&&(wc[1].thewc==multiplefinal)){
		// We have a case where there is simply a prefix string (or maybe even not) and justa '*'
		if(startingWC==0) // We have a non-empty prefix string
			SearchNotEqualNonIndexed(schField, wc[0].thechars, 0);
		// OK - we're done !
		delete[] thestr;
		assert( Invariant("simpleB::SearchNotEqualNonIndexedWildcard-exit trailingWC") );
		return !mSelection.isEmpty();
	}

	OOF_recordSelection newSelection( mTable->prototype() );

	unloadRecord();
	for(start();more();next()){  // Vertical Iteration through the selection
	
		int curwc=startingWC;
		int curchar=0;
		int curskipped=0;
		
		const oofString fldContents=schField->copyString();  //Retrieve the current field contents
		const char *s=fldContents;
		
		while(*s){
			if(wc[curwc].thewc==single) {
				if(!curskipped) {
					s++;
					curskipped++;
				} else if(*(wc[curwc].thechars)=='\0'){
					// singular folowed by a null literal is a match always
					// BUT we don't advance the pointer in this case !
					curwc++;
					curskipped=0;
				} else if (toupper(*s)==toupper(wc[curwc].thechars[curchar])) {
					// singular followed by a real literal is a match !
					s++;
					curchar++;
					if(!wc[curwc].thechars[curchar]) { // End of matched literal
						curwc++;
						if(curwc>numwcs) // We have run out of literals
							break;
						curchar=0;
						curskipped=0;
					}
				} else { // filed to match
					curskipped++;
					if(curskipped>1) // failed to match more than once !
						break;
				}
			} else if(wc[curwc].thewc==singlefinal) { 
				// We have a trailing '?'
				s++;
				curwc++;
				break; // We skipped the '?', so we can leave the rest to the final test
			} else if(wc[curwc].thewc==multiplefinal) { 
				// We have a trailing '*' so we match from here automatically
				curwc++;
				s = "";   // point at static null string to terminate loop
				break;
			} else if(toupper(*s)==toupper(wc[curwc].thechars[curchar])) { // Matched character
				s++;
				curchar++;
				curskipped=0;
				if(!wc[curwc].thechars[curchar]) { // End of matched literal
					curwc++;
					if(curwc>numwcs) // We have run out of literals
						break;
					curchar=0;
				} 
			} else { // Character didn't match
				if(curchar>0) { // are we currently matching a literal ?
					s-=(curchar-1);
					curchar=0;
				} else {
					s++;
					curskipped++;
				}
			}
		}
		
		if(!((curwc>numwcs)&&!(*s)))
			newSelection.append(mCurrLoadedRecOffset);
			
		unloadRecord();
	}
	
	delete[] thestr;

	mSelection = newSelection;	

	assert( Invariant("simpleB::SearchNotEqualNonIndexedWildcard-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchEqualNonIndexed(const dbField* schField, const void* schAddress)
{
	assert( Invariant("simpleB::SearchEqualNonIndexed(bin)-entry") );
	
	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchEqualNonIndexed(bin)-exit emptySel") );
		return false;
	}	
	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	switch(schField->fieldStorageLen()){
		case 2: {
			unsigned short theValue;
			memcpy(&theValue,schAddress,2);
			for(start();more();next()) {  // Vertical Iteration through the selection
				if((unsigned short)(*(dbUshort*)schField)==theValue)
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case 4: {
			unsigned long theValue;
			memcpy(&theValue,schAddress,4);
			for(start();more();next()) { // Vertical Iteration through the selection
				if((unsigned long)(*(dbUlong*)schField)==theValue)
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case 8: {
			double theValue;
			memcpy(&theValue,schAddress,8);
			for(start();more();next()) { // Vertical Iteration through the selection
				if((double)(*(dbReal*)schField)==theValue)
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		default: {
#ifdef OOF_DEBUG
			dbConnect::raise("Search not yet implemented for non-indexed multi-byte binary field");
#endif
			; // NOT YET IMPLEMENTED - scan a non-indexed multi-byte binary field
		}	
	}
	
	mSelection = newSelection;	

	assert( Invariant("simpleB::SearchEqualNonIndexed(bin)-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchNotEqualNonIndexed(const dbField* schField, const void* schAddress)
{
	assert( Invariant("simpleB::SearchNotEqualNonIndexed(bin)-entry") );

	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchNotEqualNonIndexed(bin)-exit") );
		return false;
	}	
	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	switch(schField->fieldStorageLen()){
		case 2: {
			unsigned short theValue;
			memcpy(&theValue,schAddress,2);
			for(start();more();next())  // Vertical Iteration through the selection
				if((unsigned short)(*(dbUshort*)schField)!=theValue)
						newSelection.append(mCurrLoadedRecOffset);
			break;
		}
		
		case 4: {
			unsigned long theValue;
			memcpy(&theValue,schAddress,4);
			for(start();more();next())  // Vertical Iteration through the selection
				if((unsigned long)(*(dbUlong*)schField)!=theValue)
						newSelection.append(mCurrLoadedRecOffset);
			break;
		}
		
		case 8: {
			double theValue;
			memcpy(&theValue,schAddress,8);
			for(start();more();next())  // Vertical Iteration through the selection
				if((double)(*(dbReal*)schField)!=theValue)
						newSelection.append(mCurrLoadedRecOffset);
			break;
		}
		
		default: {
#ifdef OOF_DEBUG
			dbConnect::raise("Search not yet implemented for non-indexed multi-byte binary field");
#endif
			; // NOT YET IMPLEMENTED - scan a non-indexed multi-byte binary field
		}	
	}
	
	mSelection = newSelection;	

	assert( Invariant("simpleB::SearchNotEqualNonIndexed(bin)-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchBetweenNonIndexed(const dbField* schField, const char* strFrom, const char* strTo)
{
	assert( Invariant("simpleB::SearchBetweenNonIndexed(string)-entry") );
	
	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchBetweenNonIndexed(string)-exit emptySel") );
		return false;
	}	
	// If the database was ordered by this field, we would only need to perform one test
	// on each record instead of two - perhpas we could optimise somehow ?

	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	unloadRecord();
	for(start();more();next()){
		const char *theFld = schField->asChars();
		if((OOF_stricmp(theFld, strFrom)>=0)&&(OOF_stricmp(theFld, strTo)<=0)) 
			newSelection.append(mCurrLoadedRecOffset);
		unloadRecord();	
	}
	
	mSelection = newSelection;	
	assert( Invariant("simpleB::SearchBetweenNonIndexed(string)-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchOutsideNonIndexed(const dbField* schField, const char* strFrom, const char* strTo)
{
	assert( Invariant("simpleB::SearchOutsideNonIndexed(string)-entry") );

	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchOutsideNonIndexed(string)-exit emptySel") );
		return false;
	}	
	// If the database was ordered by this field, we would only need to perform one test
	// on each record instead of two - perhpas we could optimise somehow ?

	// NOTE: This method is used on indexed fields as well - we could heavily optimise
	//       by using a combinatorial indexed search !

	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	unloadRecord();	
	for(start();more();next()){
		const char *theFld = schField->asChars();
		if((OOF_stricmp(theFld, strFrom)<0)||(OOF_stricmp(theFld, strTo)>0)) 
			newSelection.append(mCurrLoadedRecOffset);
		unloadRecord();	
	}
	
	mSelection = newSelection;	

	assert( Invariant("simpleB::SearchOutsideNonIndexed(string)-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchBetweenNonIndexed(const dbField* schField, const void* addressFrom, const void* addressTo)
{
	assert( Invariant("simpleB::SearchBetweenNonIndexed(bin)-entry") );
	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchBetweenNonIndexed(bin)-exit emptySel") );
		return false;
	}	
	// If the database was ordered by this field, we would only need to perform one test
	// on each record instead of two - perhpas we could optimise somehow ?

	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	switch(schField->nativeType()){
		case uShortField: {
			unsigned short From, To;
			memcpy(&From,addressFrom,sizeof(unsigned short));
			memcpy(&To,addressTo,sizeof(unsigned short));
			for(start();more();next()){
				const unsigned short theFld = (unsigned short)(*(dbUshort*)schField);
				if((theFld>=From)&&(theFld<=To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case shortField: {
			short From, To;
			memcpy(&From,addressFrom,sizeof(short));
			memcpy(&To,addressTo,sizeof(short));
			for(start();more();next()){
				const short theFld = (short)(*(dbShort*)schField);
				if((theFld>=From)&&(theFld<=To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case uLongField: {
			unsigned long From, To;
			memcpy(&From,addressFrom,sizeof(unsigned long));
			memcpy(&To,addressTo,sizeof(unsigned long));
			for(start();more();next()){
				const unsigned long theFld = (unsigned long)(*(dbUlong*)schField);
				if((theFld>=From)&&(theFld<=To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case longField: {
			long From, To;
			memcpy(&From,addressFrom,sizeof(long));
			memcpy(&To,addressTo,sizeof(long));
			for(start();more();next()){
				const long theFld = (long)(*(dbLong*)schField);
				if((theFld>=From)&&(theFld<=To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case realField: {
			double From, To;
			memcpy(&From,addressFrom,sizeof(double));
			memcpy(&To,addressTo,sizeof(double));
			for(start();more();next()){
				const double theFld = (double)(*(dbReal*)schField);
				if((theFld>=From)&&(theFld<=To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		default: {
#ifdef OOF_DEBUG
			dbConnect::raise("SearchBetween not yet implemented for non-indexed multi-byte binary field");
#endif
			; // NOT YET IMPLEMENTED - scan a non-indexed multi-byte binary field
		}	
	}
	
	mSelection = newSelection;
	assert( Invariant("simpleB::SearchBetweenNonIndexed(bin)-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchOutsideNonIndexed(const dbField* schField, const void* addressFrom, const void* addressTo)
{
	assert( Invariant("simpleB::SearchOutsideNonIndexed(bin)-entry") );
	
	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchOutsideNonIndexed(bin)-exit emptySel") );
		return false;
	}		
	// If the database was ordered by this field, we would only need to perform one test
	// on each record instead of two - perhpas we could optimise somehow ?
	
	// NOTE: This method is used on indexed fields as well - we could heavily optimise
	//       by using a combinatorial indexed search !

	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	switch(schField->nativeType()){
		case uShortField: {
			unsigned short From, To;
			memcpy(&From,addressFrom,sizeof(unsigned short));
			memcpy(&To,addressTo,sizeof(unsigned short));
			for(start();more();next()){
				const unsigned short theFld = (unsigned short)(*(dbUshort*)schField);
				if((theFld<From)||(theFld>To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case shortField: {
			short From, To;
			memcpy(&From,addressFrom,sizeof(short));
			memcpy(&To,addressTo,sizeof(short));
			for(start();more();next()){
				const short theFld = (short)(*(dbShort*)schField);
				if((theFld<From)||(theFld>To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case uLongField:{
			unsigned long From, To;
			memcpy(&From,addressFrom,sizeof(unsigned long));
			memcpy(&To,addressTo,sizeof(unsigned long));
			for(start();more();next()){
				const unsigned long theFld = (unsigned long)(*(dbUlong*)schField);
				if((theFld<From)||(theFld>To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case longField: {
			long From, To;
			memcpy(&From,addressFrom,sizeof(long));
			memcpy(&To,addressTo,sizeof(long));
			for(start();more();next()){
				const long theFld = (long)(*(dbLong*)schField);
				if((theFld<From)||(theFld>To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		case realField: {
			double From, To;
			memcpy(&From,addressFrom,sizeof(double));
			memcpy(&To,addressTo,sizeof(double));
			for(start();more();next()){
				const double theFld = (double)(*(dbReal*)schField);
				if((theFld<From)||(theFld>To))
						newSelection.append(mCurrLoadedRecOffset);
			}
			break;
		}
		
		default: {
#ifdef OOF_DEBUG
			dbConnect::raise("SearchBetween not yet implemented for non-indexed multi-byte binary field");
#endif
			; // NOT YET IMPLEMENTED - scan a non-indexed multi-byte binary field
		}	
	}
	
	mSelection = newSelection;	

	assert( Invariant("simpleB::SearchOutsideNonIndexed(bin)-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchComparativeNonIndexed(const dbField* schField, const char* schStr, dbQueryClause::BinaryQueryOps theOp)

{
	assert( Invariant("simpleB::SearchComparativeNonIndexed(string)-entry") );

	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchComparativeNonIndexed(string)-exit emptySel") );
		return false;
		
	}	
	OOF_recordSelection newSelection( mTable->prototype() );
	unloadRecord();	
	
	switch(theOp) { // We only have to test comparative searches here, the others have already
									// been weeded out
		case (dbQueryClause::lessThan) :
				for(start();more();next()){
					if(OOF_stricmp(schField->asChars(), schStr)<0) 
						newSelection.append(mCurrLoadedRecOffset);
					unloadRecord();	
				}
				break;
				
		case (dbQueryClause::lessThanOrEqual) :
				for(start();more();next()){
					if(OOF_stricmp(schField->asChars(), schStr)<=0) 
						newSelection.append(mCurrLoadedRecOffset);
					unloadRecord();	
				}
				break;
							
		case (dbQueryClause::greaterThan) :
				for(start();more();next()){
					if(OOF_stricmp(schField->asChars(), schStr)>0) 
						newSelection.append(mCurrLoadedRecOffset);
					unloadRecord();	
				}
				break;
				
		case (dbQueryClause::greaterThanOrEqual) :
				for(start();more();next()){
					if(OOF_stricmp(schField->asChars(), schStr)>=0) 
						newSelection.append(mCurrLoadedRecOffset);
					unloadRecord();	
				}
				break;
				
		default:
			break;
	}

	mSelection = newSelection;	
	assert( Invariant("simpleB::SearchComparativeNonIndexed(string)-exit") );
	return !mSelection.isEmpty();
}


bool 
OOF_simpleRecordBackend::SearchComparativeNonIndexed(const dbField* schField, const void* schAddress,  dbQueryClause::BinaryQueryOps theOp)
{
	assert( Invariant("simpleB::SearchComparativeNonIndexed(bin)-entry") );
	
	if (mSelection.isEmpty()){
		assert( Invariant("simpleB::SearchComparativeNonIndexed(bin)-exit emptySel") );
		return false;
	}	
	OOF_recordSelection newSelection( mTable->prototype() );
	
	switch(schField->nativeType()){
		case uShortField: {
			unsigned short Value;
			memcpy(&Value,schAddress,sizeof(unsigned short));
			switch(theOp) { // We only have to test comparative searches here, the others have already
											// been weeded out
				case (dbQueryClause::lessThan) :
						for(start();more();next()){
							if((unsigned short)(*(dbUshort*)schField)<Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::lessThanOrEqual) :
						for(start();more();next()){
							if((unsigned short)(*(dbUshort*)schField)<=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
									
				case (dbQueryClause::greaterThan) :
						for(start();more();next()){
							if((unsigned short)(*(dbUshort*)schField)>Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::greaterThanOrEqual) :
						for(start();more();next()){
							if((unsigned short)(*(dbUshort*)schField)>=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
								
				default:
						break;

			}
			break;
		}
		
		case shortField: {
			short Value;
			memcpy(&Value,schAddress,sizeof(short));
			switch(theOp) { // We only have to test comparative searches here, the others have already
											// been weeded out
				case (dbQueryClause::lessThan) :
						for(start();more();next()){
							if((short)(*(dbShort*)schField)<Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::lessThanOrEqual) :
						for(start();more();next()){
							if((short)(*(dbShort*)schField)<=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
									
				case (dbQueryClause::greaterThan) :
						for(start();more();next()){
							if((short)(*(dbShort*)schField)>Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::greaterThanOrEqual) :
						for(start();more();next()){
							if((short)(*(dbShort*)schField)>=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;

				default:
						break;

			}
			break;
		}
		
		case uLongField:{
			unsigned long Value;
			memcpy(&Value,schAddress,sizeof(unsigned long));
			switch(theOp) { // We only have to test comparative searches here, the others have already
											// been weeded out
				case (dbQueryClause::lessThan) :
						for(start();more();next()){
							if((unsigned long)(*(dbUlong*)schField)<Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::lessThanOrEqual) :
						for(start();more();next()){
							if((unsigned long)(*(dbUlong*)schField)<=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
									
				case (dbQueryClause::greaterThan) :
						for(start();more();next()){
							if((unsigned long)(*(dbUlong*)schField)>Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::greaterThanOrEqual) :
						for(start();more();next()){
							if((unsigned long)(*(dbUlong*)schField)>=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;

				default:
						break;

			}
			break;
		}
		
		case longField: {
			long Value;
			memcpy(&Value,schAddress,sizeof(long));
			switch(theOp) { // We only have to test comparative searches here, the others have already
											// been weeded out
				case (dbQueryClause::lessThan) :
						for(start();more();next()){
							if((long)(*(dbLong*)schField)<Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::lessThanOrEqual) :
						for(start();more();next()){
							if((long)(*(dbLong*)schField)<=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
									
				case (dbQueryClause::greaterThan) :
						for(start();more();next()){
							if((long)(*(dbLong*)schField)>Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::greaterThanOrEqual) :
						for(start();more();next()){
							if((long)(*(dbLong*)schField)>=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;

				default:
						break;

			}
			break;
		}
		
		case realField: {
			double Value;
			memcpy(&Value,schAddress,sizeof(double));
			switch(theOp) { // We only have to test comparative searches here, the others have already
											// been weeded out
				case (dbQueryClause::lessThan) :
						for(start();more();next()){
							if((double)(*(dbReal*)schField)<Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::lessThanOrEqual) :
						for(start();more();next()){
							if((double)(*(dbReal*)schField)<=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
									
				case (dbQueryClause::greaterThan) :
						for(start();more();next()){
							if((double)(*(dbReal*)schField)>Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;
						
				case (dbQueryClause::greaterThanOrEqual) :
						for(start();more();next()){
							if((double)(*(dbReal*)schField)>=Value)
									newSelection.append(mCurrLoadedRecOffset);
						}
						break;

				default:
						break;

			}
			break;
		}
		
		default: {
#ifdef OOF_DEBUG
			dbConnect::raise("SearchBetween not yet implemented for non-indexed multi-byte binary field");
#endif
			; // NOT YET IMPLEMENTED - scan a non-indexed multi-byte binary field
		}	
	}
	
	mSelection = newSelection;	

	assert( Invariant("simpleB::SearchComparativeNonIndexed-schAddress-exit") );
	return !mSelection.isEmpty();
}


bool
OOF_simpleRecordBackend::searchSelContainsAnyOf(const dbField* schField, const char **schStrs, unsigned long count)
{
	assert( Invariant("simpleB::searchSelContainsAnyOf-entry") );

	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	unloadRecord();	
	for(start();more();next()){  // Vertical Iteration through the selection
		const oofString fldContents=schField->copyString();
		bool matched=false;
		for(unsigned long i=0;(i<count)&&!matched;i++) {
			unsigned long keyLen=strlen(schStrs[i]);
			for(const char *s=fldContents;*s;s++) 			
				if(OOF_strnicmp(s, schStrs[i], keyLen)==0) {
					matched=true;
					break;
				}
		} // string loop
		if(matched)
			newSelection.append(mCurrLoadedRecOffset);
		unloadRecord();	
	}	// record loop
	
	mSelection = newSelection;	

	assert( Invariant("simpleB::searchSelContainsAnyOf-exit") );
	return !mSelection.isEmpty();
}


bool
OOF_simpleRecordBackend::searchSelContainsAnyDelimited(const dbField* schField, const char *schStr,  char delim)
{
	assert( Invariant("simpleB::searchSelContainsAnyDelimited-entry") );
	
	char *myStr=new char[strlen(schStr)+1];
	strcpy(myStr,schStr);
	
	unsigned long numStrs=1;
	if(myStr[0]==delim)
		myStr++;
	for(unsigned long i=0;myStr[i];i++){
		if(myStr[i]==delim){
			if(myStr[i+1])
				numStrs++;
			else
				myStr[i]='\0';
		}
	}
	
	char **myStrArray=new char*[numStrs];	// can't declare as const char** as vc1.52 complains on delete[]
//	char **myStrArray=new char*[numStrs]; causes g++ to complain when passed into searchContainsAnyOf
	myStrArray[0]=myStr;
	numStrs=1;
	for(unsigned long j=0;myStr[j];j++){
		if(myStr[j]==delim){
			myStrArray[numStrs++]=&myStr[j+1];
			myStr[j]='\0';
		}
	}
	
	bool retval=searchSelContainsAnyOf(schField, (const char**)myStrArray, numStrs);
	
	delete [] myStr;
	delete [] myStrArray;
	assert( Invariant("simpleB::searchSelContainsAnyDelimited-exit") );
	return retval;
}


bool
OOF_simpleRecordBackend::searchSelContainsAllOf(const dbField* schField, const char **schStrs, unsigned long count)
{
	assert( Invariant("simpleB::searchSelContainsAllOf-entry") );
	// Make a new selection into which we can put the result - NOTE: This could be made a lot
	// more efficient if we made a reasonable stab at the initial size and the chunk size
	OOF_recordSelection newSelection( mTable->prototype() );
	
	unloadRecord();
	for(start();more();next()){  // Vertical Iteration through the selection
		const oofString fldContents=schField->copyString();
		bool matched=true;
		for(unsigned long i=0;(i<count)&&matched;i++) {
			unsigned long keyLen=strlen(schStrs[i]);
			matched=false;
			for(const char* s=fldContents;*s;s++)	
				if(OOF_strnicmp(s, schStrs[i], keyLen)==0) {
					matched=true;
					break;
			}
		}
		if(matched)
			newSelection.append(mCurrLoadedRecOffset);
		unloadRecord();
	}  // record loop
	
	mSelection = newSelection;	
	assert( Invariant("simpleB::searchSelContainsAllOf-exit") );
	return !mSelection.isEmpty();
}


bool
OOF_simpleRecordBackend::searchSelContainsAllDelimited(const dbField* schField, const char *schStr, char delim)
{
	assert( Invariant("simpleB::searchSelContainsAllDelimited-entry") );

	char *myStr=new char[strlen(schStr)+1];
	strcpy(myStr,schStr);
	
	unsigned long numStrs=1;
	if(myStr[0]==delim)
		myStr++;
	for(unsigned long i=0;myStr[i];i++){
		if(myStr[i]==delim){
			if(myStr[i+1])
				numStrs++;
			else
				myStr[i]='\0';
		}
	}
	
	char **myStrArray=new char*[numStrs];	// can't declare as const char** as vc1.52 complains on delete[]
	myStrArray[0]=myStr;
	numStrs=1;
	for(unsigned long j=0;myStr[j];j++){
		if(myStr[j]==delim){
			myStrArray[numStrs++]=&myStr[j+1];
			myStr[j]='\0';
		}
	}
	
	bool retval=searchSelContainsAllOf(schField, (const char**)myStrArray, numStrs);	// cast for g++
	
	delete [] myStr;
	delete [] myStrArray;
	
	assert( Invariant("simpleB::searchSelContainsAllDelimited-exit") );
	return retval;
}


/**
	Implements relationships via searching.
	Changes the current selection
	\param fldFrom supplies the value to be searched for.
	\param fldTo is a field probably on the current table, which is the target of the search
	\return true if finds any records
	Always changes the current selection, like search().
*/
bool 
OOF_simpleRecordBackend::loadRelatedContextJoiningFromTo(const dbField* fldFrom, const dbField* fldTo)
{
	assert( Invariant("simpleB::loadRelatedContextJoiningFromTo-entry") );
	
	OOF_recordCache* savedCache=0;

	if (mDirtyRecordCache) {
		const unsigned long numOcc = mDirtyRecordCache->countOccupied() ;  // minor optim. - this is a fast call!
		if (numOcc>0 && (numOcc==mDirtyRecordCache->count()) ) { // all new records!
			savedCache = mDirtyRecordCache;  // KIDNAP THE CACHE!
			mDirtyRecordCache=0;
			assert( Invariant("simpleB::loadRelatedContextJoiningFromTo-savedCache") );
			// *** MAY NEED TO EXPLICITLY REMOVE THE NEW ONES FROM THE SELECTION?????
			
		} // has cache of all new recs
	}  // has cache
	
	if (isNew() && !mDirtyRecordCache)  {  // kidnapped cache or never was one
	// current record is also (only?) new so save
		OOF_recordBufferedContext* ctx = new OOF_recordBufferedContext(*this);
		assert(ctx);
		DonateBlobBodiesTo(ctx);
		if (!savedCache)
			savedCache = new OOF_recordCache(ctx);
		else
			savedCache->append(ctx);
	}  // save current new record to special cache
	
	bool ret = false;
	switch (fldTo->fieldType()) {
	case charField : {
		ret = searchEqual(fldTo, ((dbChar*)fldFrom)->operator const char*());  // safe downcast
		break;
	}
		
	case textField : {
		ret = searchEqual(fldTo, ((dbText*)fldFrom)->operator const char*());  // safe downcast
		break;
	}
		
	default :  // binary single fields
		ret = searchEqual(fldTo, fldFrom->currentbinaryContents());
	}

// maybe restore cache
	if (savedCache) {
		assert(!mDirtyRecordCache);  // because only kidnapped it if all new recs in there
		mDirtyRecordCache=savedCache;
		unsigned long numCached = savedCache->count();
		for (unsigned long i=0; i<numCached; i++) {
			const OOF_recordBufferedContext* ctx = (const OOF_recordBufferedContext*) (savedCache->item(i));	// safe downcast
			assert(ctx);
			if (ctx->isNew()) {  // only copy the new ones, modified records in the cache will have their oids appended already
				const unsigned long theOID = ctx->currentOffset();
				mSelection.append(theOID);
			}  // NOTE you may have items in the cache which are not in the current
			// selection - that is perfectly legal for a bufferForever mode
			// you should regard the cache as an alternate data source
		}
	}
			
	start();
	assert( Invariant("simpleB::loadRelatedContextJoiningFromTo-exit") );
	return ret;
}


bool 
OOF_simpleRecordBackend::SearchSelectionBinaryFieldToLiteral(const dbQueryBinary* qClause)
{
	assert( Invariant("simpleB::SearchSelectionBinaryFieldToLiteral-entry") );
	
	const dbQueryLiteralStr* strRHS = qClause->literalStrClause();
	dbQueryClause::BinaryQueryOps theOp = qClause->binaryOperator();
	bool ret = false;
	if (strRHS) { 
		switch(theOp) {
		case (dbQueryClause::startsWith) :
			ret = SearchEqualNonIndexed(qClause->lhsField(), strRHS->str(), false);
			break;
		
		case (dbQueryClause::hasWord) :
			ret = SearchWordNonIndexed(qClause->lhsField(), strRHS->str());
			break;
		
		case (dbQueryClause::hasWordStartsWith) :
			ret = SearchWordNonIndexed(qClause->lhsField(), strRHS->str(), true);
			break;
		
		case (dbQueryClause::equals) :
			ret = SearchEqualNonIndexed(qClause->lhsField(), strRHS->str());
			break;
		
		case (dbQueryClause::notEquals) :
			ret = SearchNotEqualNonIndexed(qClause->lhsField(), strRHS->str());
			break;
			
		default:
			ret = SearchComparativeNonIndexed(qClause->lhsField(), strRHS->str(), theOp);
		}
	} 
	else {
		if (qClause->literalMultiStrClause())
			dbConnect::raise("multiple string searches only allowed on keyword indexed fields", false);
		else {
			const dbQueryLiteral* rhs = qClause->literalClause();
			assert(rhs);
			switch(theOp) {
			case (dbQueryClause::equals) :
				ret = SearchEqualNonIndexed(qClause->lhsField(), rhs->binaryContents());
				break;
			
			case (dbQueryClause::notEquals) :
				ret = SearchNotEqualNonIndexed(qClause->lhsField(), rhs->binaryContents());			
				break;
	
			default:
				ret = SearchComparativeNonIndexed(qClause->lhsField(), rhs->binaryContents(), theOp);
			}
		}  // binary literal
	}  // not string literal

	assert( Invariant("simpleB::SearchSelectionBinaryFieldToLiteral-exit") );
	return ret; 
}
 

bool 
OOF_simpleRecordBackend::SearchSelectionTrinaryFieldToLiterals(const dbQueryTrinary* qClause)
{
	assert( Invariant("simpleB::SearchSelectionTrinaryFieldToLiterals-entry") );
	
	// Currently we just call the non-indexed searches, which will do their own exit Invariants
	//
	// The non-indexed versions always work on a current selection, so this will have
	// the effect of refining the selection
	//
	// This could be optimised for indexed fields - We need to develop some heuristics for this
	//
	const dbQueryLiteralStr* strFrom = qClause->literalStrFromClause();
	dbQueryClause::TrinaryQueryOps theOp = qClause->trinaryOperator();
	if (strFrom) { 
		const dbQueryLiteralStr* strTo = qClause->literalStrToClause();
		assert(strTo);  // must be both strings or both binary literals
		
		switch(theOp) {
		case (dbQueryClause::between) :
			return SearchBetweenNonIndexed(qClause->lhsField(), strFrom->str(), strTo->str());
		
		case (dbQueryClause::outside) :
			return SearchOutsideNonIndexed(qClause->lhsField(), strFrom->str(), strTo->str());
		}
	}
	
	else	  {
		const dbQueryLiteral* numFrom = qClause->literalFromClause();
		const dbQueryLiteral* numTo = qClause->literalToClause();
		switch(theOp) {
		case (dbQueryClause::between) :
			return SearchBetweenNonIndexed(qClause->lhsField(), numFrom->binaryContents(), numTo->binaryContents());
		
		case (dbQueryClause::outside) :
			return SearchOutsideNonIndexed(qClause->lhsField(), numFrom->binaryContents(), numTo->binaryContents());
		}
	}	
	assert( Invariant("simpleB::SearchSelectionTrinaryFieldToLiterals-exit unhandled") );
	return false;
}

 
bool 
OOF_simpleRecordBackend::SearchBinaryFieldToLiteral(const dbQueryBinary* qClause)
{
	assert( Invariant("simpleB::SearchBinaryFieldToLiteral-entry") );

	dbQueryLiteralStr* strRHS = qClause->literalStrClause();
	dbQueryClause::BinaryQueryOps theOp = qClause->binaryOperator();
	bool ret = false;
	selectAll();
	#ifdef OOF_MEM_DEBUG_FORCE_POOL_CHECK
		OOF_MEM_DEBUG_FORCE_POOL_CHECK
	#endif

	if (strRHS) { 
		switch(theOp) {
		case (dbQueryClause::startsWith) :
			ret = SearchEqualNonIndexed(qClause->lhsField(), strRHS->str(), false);
			break;
		
		case (dbQueryClause::hasWord) :
			ret = SearchWordNonIndexed(qClause->lhsField(), strRHS->str());
			break;
		
		case (dbQueryClause::hasWordStartsWith) :
			ret = SearchWordNonIndexed(qClause->lhsField(), strRHS->str(), true);
			break;
		
		case (dbQueryClause::equals) :
			ret = SearchEqualNonIndexed(qClause->lhsField(), strRHS->str());
			break;
		
		case (dbQueryClause::notEquals) :
			ret = SearchNotEqualNonIndexed(qClause->lhsField(), strRHS->str());
			break;
			
		default:
			ret = SearchComparativeNonIndexed(qClause->lhsField(), strRHS->str(), theOp);
		}
	} 
	else {
		if (qClause->literalMultiStrClause())
			dbConnect::raise("multiple string searches only allowed on keyword indexed fields", false);
		else {
			const dbQueryLiteral* rhs = qClause->literalClause();
			assert(rhs);
			switch(theOp) {
			case (dbQueryClause::equals) :
				ret = SearchEqualNonIndexed(qClause->lhsField(), rhs->binaryContents());
				break;
			
			case (dbQueryClause::notEquals) :
				ret = SearchNotEqualNonIndexed(qClause->lhsField(), rhs->binaryContents());	
				break;
	
			default:
				return SearchComparativeNonIndexed(qClause->lhsField(), rhs->binaryContents(), theOp);
			}
		} // binary rhs
	} // not string rhs
	assert( Invariant("simpleB::SearchBinaryFieldToLiteral-exit") );
	return ret; 
}
 
 
bool 
OOF_simpleRecordBackend::SearchCombineSubclauses(const dbQueryBinaryCombo* qClause, bool searchSel)
{
	assert( Invariant("simpleB::SearchCombineSubclauses-entry") );

	bool lhsResults = false;
	OOF_recordSelection saveOriginalSelection( mTable->prototype() );
	
	if (searchSel) {
		saveOriginalSelection = mSelection;  // not adopted
		lhsResults = searchSelection(qClause->item(0));
	}
	else
		lhsResults = search(qClause->item(0));
		
	if (qClause->binaryOperator()==dbQueryClause::oofAND) {
		if (!lhsResults){  // try LHS, it may find nothing!
			assert( Invariant("simpleB::SearchCombineSubclauses-exit AND rhs fail") );
			return false;
		}		
		const bool ret = searchSelection(qClause->item(1));
		assert( Invariant("simpleB::SearchCombineSubclauses-exit AND") );
		return ret;
	} // AND search
	
	else  {   // must be an OR
// NOTE unlike above, even in searchSelection we combine results
		OOF_recordSelection saveLhsSelection( mTable->prototype());
		saveLhsSelection.adopt(mSelection);
		if (searchSel) {
			mSelection.adopt(saveOriginalSelection);
			searchSelection(qClause->item(1));  // don't care if it works
		}
		else
			search(qClause->item(1));  // don't care if it works
		if (lhsResults) // have something to combine with the search we just did
			mSelection.union_with(saveLhsSelection);

		assert( Invariant("simpleB::SearchCombineSubclauses-exit OR") );
		return !mSelection.isEmpty();
	}  // OR search
}


bool 
OOF_simpleRecordBackend::SearchTrinaryFieldToLiterals(const dbQueryTrinary* qClause)
{
	assert( Invariant("simpleB::SearchTrinaryFieldToLiterals-entry") );
	
	const dbQueryLiteralStr* strFrom = qClause->literalStrFromClause();
	dbQueryClause::TrinaryQueryOps theOp = qClause->trinaryOperator();

	if (strFrom) { 
		const dbQueryLiteralStr* strTo = qClause->literalStrToClause();
		assert(strTo);  // must be both strings or both binary literals
		
		switch(theOp) {
		case (dbQueryClause::between) :
			selectAll();
			return SearchBetweenNonIndexed(qClause->lhsField(), strFrom->str(), strTo->str());
		
		case (dbQueryClause::outside) :
			selectAll();
			return SearchOutsideNonIndexed(qClause->lhsField(), strFrom->str(), strTo->str());
		}
	}
	
	else	  {
		const dbQueryLiteral* numFrom = qClause->literalFromClause();
		const dbQueryLiteral* numTo = qClause->literalToClause();
		switch(theOp) {
		case (dbQueryClause::between) :
			selectAll();
			return SearchBetweenNonIndexed(qClause->lhsField(), numFrom->binaryContents(), numTo->binaryContents());
		
		case (dbQueryClause::outside) :
			selectAll();
			return SearchOutsideNonIndexed(qClause->lhsField(), numFrom->binaryContents(), numTo->binaryContents());
		}
	}	

	assert( Invariant("simpleB::SearchTrinaryFieldToLiterals-exit unhandled") );
	return false;
}


void 
OOF_simpleRecordBackend::lockRecord()
{
	// null method for backends that lack locking
}


void 
OOF_simpleRecordBackend::unlockRecord()
{
	// null method for backends that lack locking
}

void 
OOF_simpleRecordBackend::beginTransaction(short /*tMode*/)
{
	// null method for backends that do not have transactions
}


void 
OOF_simpleRecordBackend::commitTransaction(short /*rMode*/)
{
	// null method for backends that do not have transactions 
}


void 
OOF_simpleRecordBackend::abortTransaction(short /*rMode*/)
{
	// null method for backends that do not have transactions
}


/**
	Check combination of member variables defining Invariant state.
	Used to guard against changes to code allowing backend to enter illegal states.
	\note If you define a filename OOF_INVARIANT_DUMP_NAME this will dump 
	a very comprehensive log of all OOFILE execution, as Invariant is called on
	entry and exit from most backend methods that affect the database.
	\todo change code checking for some states to work on Intel byte order
*/
bool 
OOF_simpleRecordBackend::Invariant(const char* optionalComment) const
{
#ifndef _Macintosh
	return true;  // Andy 991224 hack for other platforms
#endif

// Note if an this code returns false
// please add a temporay if(state == XXXX)
//							ret = true
// and contact AD software with  a description of the state and the calling circumstances????? 
// mailto:dent@oofile.com.au

#ifdef OOF_INVARIANT_DUMP_NAME
	InvariantDump(optionalComment);  
#else
	#ifdef __MWERKS__
		#pragma unused (optionalComment)
	#endif
#endif

	bool ret = false;  
	unsigned long curState = state();
	unsigned long noSelection = curState & 0xffffff00;
	
	
	dbSorter* theSorter = mSelection.sorter();
	// handle closed table
		if(((curState & '\0\xff\0\0')=='\0C\0\0')&& ((curState ^ 'UCNA') )) // Mask out all but table valid field and look for 'C' 
		return false;

	//check sorter state
	if(theSorter){
		unsigned long sortState = theSorter->state();
	if(((sortState & '\0\xff\xff\0')=='\0NS\0')||((sortState & '\0\0\xff\xff')=='\0\0SX')
		||((sortState & '\0\0\xff\0')=='\0\0S\0') && ((curState & '\0\xff\0\0') == '\0I\0\0') // sorting on invalid table
		|| (((sortState & '\0\0\xff\0')=='\0\0S\0') &&((noSelection) & '\0\xff\0\0'== '\0I\0\0'))) // sorting on an invalid table
			return false; // its bad so we do not care about other states
	}
	
	// Check validating state
	// 991221 we can end up in nested invalidates (I think) because we reset the current "object" (ie: record)
	// when traversing a relationship on non-indexed fields doing the search
	/*
	if(mTable->validatingDepth() > 1)
		return false;
	*/
	
// loaded valid table with empty selection
// the folowing has temporally removed as it occurs as a transient in  ,this will be reinstated when further
//investigations devleop a clean save method of handling transient state
// typically the transient transition is LVNX (where X is O, S or all) -> LVNE -> UVNE
	//if((curState & '\xff\xff\0\xff')=='LV\0E') // check for LVNE r LVDE
	
	if(curState =='LVDE') // check for LVNE r LVDE
	 	 return false;
	
	// legal states
	ret = (!(noSelection ^ 'LID\0')) || (!(noSelection ^ 'LIN\0' )) || (!(noSelection ^ 'UVN\0' )) 
	|| (!(noSelection ^ 'UIN\0' )) || (!(curState ^ 'NINE')) || (!(noSelection ^ 'LVN\0') 
	|| !(noSelection ^ 'LVD\0')||(!(curState ^ 'UCNA')) ||!(noSelection ^ 'NVN\0' )||!(noSelection ^ 'NVD\0' ));


	return ret; 
}


unsigned long 
OOF_simpleRecordBackend::state() const
{


	unsigned long ret = 0;
	switch(mRecordState) {
		case (eNew) :
			ret |= 'N\0\0\0' ;
			break;

		case (eLoaded) :
			ret |= 'L\0\0\0';
			break;

		case (eUnloaded) :
			ret |= 'U\0\0\0';
			break;
			
		default:
			assert(!"unknown state");
	}
	//unsigned long aState = mTable->mTableValid;
	if(mTable){
		switch(mTable->validStatus()) { 
		
		case (dbTable::eInvalid) :
			ret|= '\0\x49\0\0';
			break;

		case (dbTable::eValid) :
			ret|= '\0V\0\0';
			break;

		case (dbTable::eClosed) :
			ret|= '\0C\0\0';
			break;
			
		default:
			assert(!"unknown state");
		}
	}
	
	if (mDirty)
		ret|= '\0\0D\0';
	else
		ret|= '\0\0N\0';
	
	switch(mSelection.state()) {
		case (OOF_SelectionStateDefiner::empty) :
			ret |= '\0\0\0\x45';
			break;
	
		case (OOF_SelectionStateDefiner::oneRec) :
			ret |= '\0\0\0O';
			break;
	
		case (OOF_SelectionStateDefiner::someRecs) :
			ret |= '\0\0\0S';
			break;
			
		case (OOF_SelectionStateDefiner::allRecs) :
			ret |= '\0\0\0\x41';
			break;
		
		default:
			assert(!"unknown state");
	}
	
	return ret;
}

#ifdef OOF_INVARIANT_DUMP_NAME

void 
OOF_simpleRecordBackend::InvariantDump(const char* optionalComment) const
{
	oofString stateString(optionalComment,"\t", InvariantDescription() );
		
	#ifdef _Macintosh
	// needs programmer to enter MacsBug and start logging to file, eg:
	// log ooftest
		//::DebugStr("\p;LOG OOFLOG ; SC;; printf \"David was here\";log G; \n ");
	
		oofString makeDump = ";log stLOg;sc;printf \" stlog: ";
		// time stamp in mac ticks 16.66ms aslo a uniqui ID i hope
		unsigned long aTime = ::TickCount();
		makeDump.convertNumber(aTime, "%.8X\t", true  /* append */);
		//::TickCount()
		makeDump += stateString;
		
		makeDump += "\" ;g; \n";		
		assert(makeDump.length() <= 255);	
		Str255 pString;
		makeDump.getStr255(pString);
		::DebugStr(pString);
		
		//::DebugStr("\pSC ;g");
	
	#else
	// always reopen the file
		stSaveDirectory preserveUserDir;
		
		sInvariantDumpDir.gotoDirectory();	// wherever we were when app started
		sInvariantDump.open(OOF_INVARIANT_DUMP_NAME, ios::app | ios::out);	// now will always open in same place!
		assert(sInvariantDump.good());
	
		sInvariantDump << stateString;
		if (optionalComment) 
			sInvariantDump << '\t' << optionalComment;
		sInvariantDump <<  endl;
		
	// close with flushing to force it to be written even if we crash
		sInvariantDump.flush();
		sInvariantDump.close();
	#endif  // _Macintosh
}

#endif // OOF_INVARIANT_DUMP_NAME


void 
OOF_simpleRecordBackend::describeInvariant(ostream& os) const 
{	
	const oofString stateString = InvariantDescription();
		os << " Current database state: " << endl
		<<  stateString  << endl;
}
	
	
oofString 
OOF_simpleRecordBackend::InvariantDescription() const 
{	
	oofString stateString;

	stateString.convertNumber(mTable->validatingDepth(),"%u",true);
	stateString += "\t";

	
	switch(mRecordState) {
		case (eNew) :
			stateString += "N\t";
			break;
	
		case (eLoaded) :
			stateString += "L\t";
			break;
	
		case (eUnloaded) :
			stateString += "U\t";
			break;
			
		default:
			assert(!"unknown state");
		}
	
		switch(mTable->validStatus()) {
		case (dbTable::eInvalid) :
			stateString += "I\t";
			break;
	
		case (dbTable::eValid) :
			stateString += "V\t";
			break;
	
		case (dbTable::eClosed) :
			stateString += "C\t";
			break;
			
		default:
			assert(!"unknown state");
		}
		
		if(mDirty)
			stateString += "D\t";
		else
			stateString += "N\t";	
		//add the address of this 
		stateString.convertNumber((unsigned long) this, "%.8X\t", true  /* append */);
		stateString.convertNumber(mCurrLoadedRecOffset, "%.8X\t", true  /* append */);
		//selection state
		stateString.convertNumber(mSelection.oneRecOffset(), "%.8X\t", true  /* append */);
		stateString.convertNumber(mSelection.iter(), "%.8X\t", true  /* append */);
		
		//stateString.convertNumber(mSelection.mRep->mCurrSingleRecOffset, "%X\t", true  /* append */);
		//stateString.convertNumber(mSelection.mRep->iter(), "%X\t", true  /* append */);
	
		switch(mSelection.state()) {
		case (OOF_SelectionStateDefiner::empty) :
			stateString += "E\t";
			break;
	
		case (OOF_SelectionStateDefiner::oneRec) :
			stateString += "O\t";
			break;
	
		case (OOF_SelectionStateDefiner::someRecs) :
			stateString += "S\t";
			break;
			
		case (OOF_SelectionStateDefiner::allRecs) :
			stateString += "A\t";
			break;
		
		default:
			assert(!"unknown state");
		}
		
		
		// sorter state
		dbSorter* theSorter = mSelection.sorter();
		if(theSorter){
			union {
				char theString[4];
				unsigned long currentState;
			} stateValue;
			stateValue.currentState = theSorter->state();
			
			 stateString.append(stateValue.theString ,4); // may have endian problems
			
			//stateString+= "\t";	
		}
		else{
			stateString += "NONE";
		
		}
		return stateString;
}
		
