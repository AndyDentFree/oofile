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
#ifndef H_OOFLIST
	#include "ooflist.h"
#endif
#ifndef H_OOFWORDS
	#include "oofwords.h"
#endif
#ifndef OOF_EXCEP
	#include "oofexcep.h"
#endif
#include <ctype.h>

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

void OOF_copyFirstFour(unsigned long *dest,const char *input);
// Prototype for function used in these sorts only


// --------------------------------------------------------------------------------------------------
//                        U T I L I T I E S
// --------------------------------------------------------------------------------------------------
void
OOF_copyFirstFour(unsigned long *dest,const char *input)
// Make this more efficient sometime..
{
	char *destch=(char *)dest;
	
#if OOF_INTEL_BYTE_ORDER
// we are treating strings as an unsigned long
// so must swap the order of the bytes to keep numeric order
// ascending same as string sort order
	for(unsigned i=0;i<4;i++){
		destch[3-i]=toupper(input[i]);
	}
#else
	for(unsigned i=0;i<4;i++){
		destch[i]=toupper(input[i]);
	}
#endif	
}


// -------------------------------------------------------
//    O O F I L E _ s i m p l e R e c o r d B a c k e n d
//         see also oofrec1.cpp & oofrec3.cpp
// -------------------------------------------------------
void 
OOF_simpleRecordBackend::sortSelectionNow(const dbSorter* theSorter)
{
	// OK - so we start by sorting and partitioning the base field
	//
	// This could use som heuristics if the field in indexed, since ctree
	// does some of this good stuff for us - for now, we just use a non-indexed
	// case..
	//
	// At least we could get ctree to put the fields in the correct order for us !
	// But later - let's get it working for now !
#ifdef OOF_DEBUG
	if(!theSorter->countSegments())
			dbConnect::raise("dbSorter has no segments - What am I meant to be sorting on ?");
#endif

//	mSelection.aboutToDirtySelection(); NOT NEEDED UNLESS SORTS CHANGE TO BE IN-PLACE

	const fieldNumT sortFieldNum = theSorter->fieldNumberOfSegment(0);
	const dbField *theField=field(sortFieldNum);
	OOF_recordSelection newSelection( mTable->prototype() );
										
	switch(theField->fieldType()) {
	case uLongField:
		{
			OOF_ULPSorter lhs;
			
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					lhs.sortedInsert(*(dbUlong*)theField, mCurrLoadedRecOffset);
				}
			}
			else {
				for(start();more();next()) {
					lhs.sortedInsertReverse(*(dbUlong*)theField, mCurrLoadedRecOffset);
				}
			}
			
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) { // If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}	
			}											
		}
		break;
	
	case dateField:	
		{
			OOF_ULPSorter lhs;
			
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					lhs.sortedInsert(*(dbDate*)theField, mCurrLoadedRecOffset);
				}
			}
			else {
				for(start();more();next()) {
					lhs.sortedInsertReverse(*(dbDate*)theField, mCurrLoadedRecOffset);
				}
			}
			
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) {// If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}	
			}											
		}
		break;
		
	case dateTimeField:	
		{
			OOF_ULPSorter lhs;
			
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					lhs.sortedInsert(*(dbDateTime*)theField, mCurrLoadedRecOffset);
				}
			}
			else {
				for(start();more();next()) {
					lhs.sortedInsertReverse(*(dbDateTime*)theField, mCurrLoadedRecOffset);
				}
			}
			
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) { // If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}		
			}										
		}
		break;

	case timeField:	
		{
			OOF_ULPSorter lhs;
			
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					lhs.sortedInsert(*(dbTime*)theField, mCurrLoadedRecOffset);
				}
			}
			else {
				for(start();more();next()) {
					lhs.sortedInsertReverse(*(dbTime*)theField, mCurrLoadedRecOffset);
				}
			}
			
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) { // If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}
			}												
		}
		break;
			
	case uShortField:	
		{
			OOF_ULPSorter lhs;
			
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					lhs.sortedInsert(*(dbUshort*)theField, mCurrLoadedRecOffset);
				}
			}
			else {
				for(start();more();next()) {
					lhs.sortedInsertReverse(*(dbUshort*)theField, mCurrLoadedRecOffset);
				}
			}
			
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) {// If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}	
			}											
		}
		break;
		
	case longField:	
		{
			OOF_LPSorter lhs;
			
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					lhs.sortedInsert(*(dbLong*)theField, mCurrLoadedRecOffset);
				}
			}
			else {
				for(start();more();next()) {
					lhs.sortedInsertReverse(*(dbLong*)theField, mCurrLoadedRecOffset);
				}
			}
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) { // If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}		
			}										
		}
		break;
		
	case shortField:	
		{
			OOF_LPSorter lhs;
			
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					lhs.sortedInsert(*(dbShort*)theField, mCurrLoadedRecOffset);
				}
			}
			else {
				for(start();more();next()) {
					lhs.sortedInsertReverse(*(dbShort*)theField, mCurrLoadedRecOffset);
				}
			}
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) {// If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}	
			}											
		}
		break;
		
	case realField:	
		{
			OOF_DPSorter lhs;
			
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					lhs.sortedInsert(*(dbReal*)theField, mCurrLoadedRecOffset);
				}
			}
			else {
				for(start();more();next()) {
					lhs.sortedInsertReverse(*(dbReal*)theField, mCurrLoadedRecOffset);
				}
			}
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) {// If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}
			}												
		}
		break;
		
	case charField:	
		{
			OOF_ULPSorter lhs;
			dbTable* compareTable = theField->fieldTable()->cloneTableWithoutSelection();
			dbChar* compareField = (dbChar *)compareTable->field(theField->fieldNumber());
	
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next()) {
					unsigned long FirstFour = 0;
					OOF_copyFirstFour(&FirstFour,*(dbChar *)theField);
					lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
					while(lhs.lastEqual()) {
						// Load the matching record into compareField
						compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
						int diff=OOF_stricmp(*(dbChar *)theField,*compareField);
						if(!diff){
							lhs.theEqual()->mPart=lhs.theEqual()->mNext->mPart+1;
							break;
						} 
						else if(diff>0) {
							lhs.theEqual()->mPart=0; // This is not part of a partition
							lhs.exchange(lhs.theEqual());
						} 
						else {
							lhs.theEqual()->mPart=0; // This is not part of a partition
							break;
						}
					}
				}
			}
			else {
				for(start();more();next()) {
					unsigned long FirstFour = 0;
					OOF_copyFirstFour(&FirstFour,*(dbChar *)theField);
					lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
					while(lhs.lastEqual()) {
						// Load the matching record into compareField
						compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
						int diff=OOF_stricmp(*(dbChar *)theField,*compareField);
						if(!diff){
							lhs.theEqual()->mPart=lhs.theEqual()->mNext->mPart+1;
							break;
						} 
						else if(diff<0) {
							lhs.theEqual()->mPart=0; // This is not part of a partition
							lhs.exchange(lhs.theEqual());
						} 
						else {
							lhs.theEqual()->mPart=0; // This is not part of a partition
							break;
						}
					}
				}
			}
			
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) { // If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}	
			}											
			delete compareTable;
		}
		break;
			
	case textField:	
		{
			OOF_ULPSorter lhs;
			dbTable* compareTable = theField->fieldTable()->cloneTableWithoutSelection();
			dbText* compareField = (dbText *)compareTable->field(theField->fieldNumber());
	
			if(!theSorter->segment(0)->isReversed()) {
				for(start();more();next())
				{
					unsigned long FirstFour = 0;
					OOF_copyFirstFour(&FirstFour,*(dbText *)theField);
					lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
					while(lhs.lastEqual()) {
						// Load the matching record into compareField
						compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
						int diff=OOF_stricmp(*(dbText *)theField,*compareField);
						if(!diff){
							lhs.theEqual()->mPart=lhs.theEqual()->mNext->mPart+1;
							break;
						} else if(diff>0) {
							lhs.theEqual()->mPart=0; // This is not part of a partition
							lhs.exchange(lhs.theEqual());
						} else {
							lhs.theEqual()->mPart=0; // This is not part of a partition
							break;
						}
					}
				}
			}
			else
				for(start();more();next())
				{
					unsigned long FirstFour = 0;
					OOF_copyFirstFour(&FirstFour,*(dbText *)theField);
					lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
					while(lhs.lastEqual()) {
						// Load the matching record into compareField
						compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
						int diff=OOF_stricmp(*(dbText *)theField,*compareField);
						if(!diff){
							lhs.theEqual()->mPart=lhs.theEqual()->mNext->mPart+1;
							break;
						} 
						else if(diff<0) {
							lhs.theEqual()->mPart=0; // This is not part of a partition
							lhs.exchange(lhs.theEqual());
						} 
						else {
							lhs.theEqual()->mPart=0; // This is not part of a partition
							break;
						}
					}
				}
			
			// OK - we have a sorted and partitioned lhs - now copy it back
			//         and subsort the partitions
			if(theSorter->countSegments()==1) {// If we have no subsorts - don't bother
				while(!lhs.isEmpty())
					newSelection.append(lhs.remove());
			}
			else {
				while(!lhs.isEmpty()) {
					if(lhs.theFirst()->mPart) { // we found a partition
						unsigned long count=lhs.theFirst()->mPart+1;
						while(lhs.theFirst()->mPart) {
			 				newSelection.append(lhs.remove());
						}
			 			newSelection.append(lhs.remove());  // Append the zeroth item in the partition
						SubSort(theSorter, 1, &newSelection[(newSelection.count()-count)], count);
					} 
					else
			 			newSelection.append(lhs.remove());
				}	
			}											
			delete compareTable;
		}
		break;
		
	case compoundField:	
		{
			dbCompoundField* sortingBy = (dbCompoundField*)theField;	// safe downcast
			dbSorter temp(*sortingBy);	// both descend from OOF_FieldSegOwner so can copy construct
			sortSelectionNow(&temp);
			return;	// EARLY EXIT - recursive call has already done the following
		}
		break;
	
	default:
		{
			oofE_General excp(stringstream() << flush 
				<< "SortSelectionNow is not defined on field type"
				<< theField->fieldType() << " for other than indexed selections of all records");
			RAISE_EXCEPTION(excp);
	// NOT YET IMPLEMENTED
		}
	}
	
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.			
}

void 
OOF_simpleRecordBackend::SubSort(const dbSorter *theSorter, const unsigned long level, unsigned long *theData, const unsigned long count)
{
	assert( Invariant("simpleB::SubSort-entry") );
	// OK - Now we're sorting at the recursive level of partitioning in a nested sort !
	//
	// We have a pseudo-selection in the form of a fixed array of unsigned longs (which are record pointers)
	// and we will simply rearrange them within the storage passed to us
	//
	// NOTE: The array passed to us with have the records in current record order
	//            ie: We will only be jumping forward
	//
	//  I hope this doesn't screw up Andy's Selection Stuff !   : )
	//
	const fieldNumT sortFieldNum = theSorter->fieldNumberOfSegment(level);
	const dbField *theField=field(sortFieldNum);
	switch(theField->fieldType()) {
		case uLongField:	{
								OOF_ULPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsert(*(dbUlong*)theField, theData[i]);
									}
								 } else {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsertReverse(*(dbUlong*)theField, theData[i]);
									}
								 }
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case uShortField:	{
								OOF_ULPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsert(*(dbUshort*)theField, theData[i]);
									}
								 } else {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsertReverse(*(dbUshort*)theField, theData[i]);
									}
								 }
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case dateField:	{
								OOF_ULPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsert(*(dbDate*)theField, theData[i]);
									}
								 } else {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsertReverse(*(dbDate*)theField, theData[i]);
									}
								 }
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case dateTimeField:	{
								OOF_ULPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsert(*(dbDateTime*)theField, theData[i]);
									}
								 } else {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsertReverse(*(dbDateTime*)theField, theData[i]);
									}
								 }
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case timeField:	{
								OOF_ULPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsert(*(dbTime*)theField, theData[i]);
									}
								 } else {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsertReverse(*(dbTime*)theField, theData[i]);
									}
								 }
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case longField:	{
								OOF_LPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsert(*(dbLong*)theField, theData[i]);
									}
								 } else {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsertReverse(*(dbLong*)theField, theData[i]);
									}
								 }
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case shortField:	{
								OOF_LPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsert(*(dbShort*)theField, theData[i]);
									}
								 } else {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsertReverse(*(dbShort*)theField, theData[i]);
									}
								 }
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case realField:		{
								OOF_DPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsert(*(dbReal*)theField, theData[i]);
									}
								 } else {
									for(unsigned int i=0;i<count;i++){
										LoadRecordAtOffset(theData[i]);
										lhs.sortedInsertReverse(*(dbReal*)theField, theData[i]);
									}
								 }
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case charField:		{
								OOF_ULPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++)
									{
										LoadRecordAtOffset(theData[i]);
										unsigned long FirstFour = 0;
										OOF_copyFirstFour(&FirstFour,*(dbChar *)theField);
										lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
										if(lhs.lastEqual()) {
											const oofString compareField=theField->copyString();
											while(lhs.lastEqual()) {
												// Load the matching record into a string
												LoadRecordAtOffset(lhs.theEqual()->mNext->mData);
												const int diff=OOF_stricmp(compareField,*(dbChar *)theField);	// safe downcast
												if(!diff){
													lhs.theEqual()->mPart=lhs.theEqual()->mNext->mPart+1;
													break;
												} else if(diff>0) {
													lhs.theEqual()->mPart=0; // This is not part of a partition
													lhs.exchange(lhs.theEqual());
												} else {
													lhs.theEqual()->mPart=0; // This is not part of a partition
													break;
												}
											}
										}
									}
								}
								else {
									for(unsigned int i=0;i<count;i++)
									{
										LoadRecordAtOffset(theData[i]);
										unsigned long FirstFour = 0;
										OOF_copyFirstFour(&FirstFour,*(dbChar *)theField);
										lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
										if(lhs.lastEqual()) {
											const oofString compareField=theField->copyString();
											while(lhs.lastEqual()) {
												// Load the matching record into a string
												LoadRecordAtOffset(lhs.theEqual()->mNext->mData);
												const int diff=OOF_stricmp(compareField,*(dbChar *)theField);
												if(!diff){
													lhs.theEqual()->mPart=lhs.theEqual()->mNext->mPart+1;
													break;
												} else if(diff<0) {
													lhs.theEqual()->mPart=0; // This is not part of a partition
													lhs.exchange(lhs.theEqual());
												} else {
													lhs.theEqual()->mPart=0; // This is not part of a partition
													break;
												}
											}
										}
									}
								}
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		case textField:		{
								OOF_ULPSorter lhs;
						
								if(!theSorter->segment(level)->isReversed()) {
									for(unsigned int i=0;i<count;i++)
									{
										((dbText *)theField)->reset();
										LoadRecordAtOffset(theData[i]);
										unsigned long FirstFour = 0;
										OOF_copyFirstFour(&FirstFour,*(dbText *)theField);
										lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
										if(lhs.lastEqual()) {
											const oofString compareField=theField->copyString();
											while(lhs.lastEqual()) {
												// Load the matching record into a string
												LoadRecordAtOffset(lhs.theEqual()->mNext->mData);
												const int diff=OOF_stricmp(compareField,*(dbText *)theField);
												if(!diff){
													lhs.theEqual()->mPart=lhs.theEqual()->mNext->mPart+1;
													break;
												} else if(diff>0) {
													lhs.theEqual()->mPart=0; // This is not part of a partition
													lhs.exchange(lhs.theEqual());
												} else {
													lhs.theEqual()->mPart=0; // This is not part of a partition
													break;
												}
											}
										}
									}
								}
								else {
									for(unsigned int i=0;i<count;i++)
									{
										LoadRecordAtOffset(theData[i]);
										unsigned long FirstFour = 0;
										OOF_copyFirstFour(&FirstFour,*(dbChar *)theField);
										lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
										if(lhs.lastEqual()) {
											const oofString compareField=theField->copyString();
											while(lhs.lastEqual()) {
												// Load the matching record into a string
												LoadRecordAtOffset(lhs.theEqual()->mNext->mData);
												const int diff=OOF_stricmp(compareField,*(dbText *)theField);
												if(!diff){
													lhs.theEqual()->mPart=lhs.theEqual()->mNext->mPart+1;
													break;
												} else if(diff<0) {
													lhs.theEqual()->mPart=0; // This is not part of a partition
													lhs.exchange(lhs.theEqual());
												} else {
													lhs.theEqual()->mPart=0; // This is not part of a partition
													break;
												}
											}
										}
									}
								}
								
								// OK - we have a sorted and partitioned lhs - now copy it back
								//         and subsort the partitions
								if(theSorter->countSegments()==(level+1)) { // If we have no further subsorts - don't bother
									for(unsigned int i=0;i<count;i++)
										theData[i]=lhs.remove();											
								} 
								else {
									unsigned int i=0;
									while(i<count) {
										if(lhs.theFirst()->mPart) { // we found a partition
											unsigned long thisCount=lhs.theFirst()->mPart+1;
											while(lhs.theFirst()->mPart) {
								 				theData[i++]=lhs.remove();
											}
								 			theData[i++]=lhs.remove(); // Append the zeroth item in the partition
											SubSort(theSorter, level+1, &theData[(i-thisCount)], thisCount);
										} else
								 			theData[i++]=lhs.remove();
									}
								}			
							}
							break;
		default:
#ifdef OOF_DEBUG
			dbConnect::raise(stringstream() << flush << "SubSort is not defined on field type"
			<< theField->fieldType() << " for other than indexed selections of all records");
#endif
		// NOT YET IMPLEMENTED - any field types left unimplemented, or is this a catchall for future?
		;
	}
	assert( Invariant("simpleB::SubSort-exit") );
}


void 
OOF_simpleRecordBackend::sortSelectionNow(fieldNumT sortFieldNo)
{
	if ( (mSelection.state() == OOF_recordSelection::empty) ||
		 (mSelection.state() == OOF_recordSelection::oneRec) )
		return;

	dbField* theFld = (dbField*) mFields.value(sortFieldNo);  // safe downcast
	
	if ( CanUseIndex(theFld) && (mSelection.state() == OOF_recordSelection::allRecs) ) {
		return;		
	}
//	mSelection.aboutToDirtySelection(); NOT NEEDED UNLESS SORTS CHANGE TO BE IN-PLACE
	
	switch(theFld->fieldType()){
		case uShortField: SortSelectionNow((dbUshort*)theFld); break;
		case shortField: SortSelectionNow((dbShort*)theFld); break;
		case uLongField: SortSelectionNow((dbUlong*)theFld); break;
		case longField: SortSelectionNow((dbLong*)theFld); break;
		case dateField: SortSelectionNow((dbUlong*)theFld); break;
		case dateTimeField: SortSelectionNow((dbUlong*)theFld); break;
		case timeField: SortSelectionNow((dbLong*)theFld); break;
		case realField: SortSelectionNow((dbReal*)theFld); break;
		case charField: SortSelectionNow((dbChar*)theFld); break;
		case textField: SortSelectionNow((dbText*)theFld); break;
		case compoundField:	
			{
				dbCompoundField* sortingBy = (dbCompoundField*)theFld;	// safe downcast
				dbSorter temp(*sortingBy);	// both descend from OOF_FieldSegOwner so can copy construct
				sortSelectionNow(&temp);
				return;	// EARLY EXIT - recursive call has already done the following
			}
			break;

		case fixedBinaryField: {
			unsigned long fixedLength = theFld->fieldStorageLen();
			// NOT YET IMPLEMENTED - cope with 4-byte fields as special case
			if (fixedLength>3)
				SortSelectionNow(theFld, fixedLength); 
			else {	// NOT YET IMPLEMENTED - cope with 2-byte fields with uShort sorter
				dbConnect::raise(stringstream() << flush 
					<< "Unable to Sort by field" << theFld->fieldName() 
					<< " SortSelectionNow is not defined on fields of type"
					<< theFld->fieldType() << " < 4 bytes long",
					false /* don't terminate */
				);
			}
		}
		break;
		default:
			dbConnect::raise(stringstream() << flush 
				<< "Unable to Sort by field" << theFld->fieldName() 
				<< " SortSelectionNow is not defined on fields of type"
				<< theFld->fieldType() << " for other than indexed selections of all records",
				false /* don't terminate */
			);
		// NOT YET IMPLEMENTED
		;
	}
}


void
OOF_simpleRecordBackend::sortSelectionInverseNow(fieldNumT sortFieldNo)
{
	assert( Invariant("simpleB::sortSelectionInverseNow-entry") );

// non-indexed sort, likely called on a subset of records

	if ( (mSelection.state() == OOF_recordSelection::empty) ||
		 (mSelection.state() == OOF_recordSelection::oneRec) 
	) {
		assert( Invariant("simpleB::sortSelectionInverseNow-exit emptyOrOneRec") );
		return;
	}

//	mSelection.aboutToDirtySelection(); NOT NEEDED UNLESS SORTS CHANGE TO BE IN-PLACE
	
	dbField* theFld = (dbField*) mFields.value(sortFieldNo);  // safe downcast
	switch(theFld->fieldType()){
		case uShortField: SortSelectionInverseNow((dbUshort*)theFld); break;
		case shortField: SortSelectionInverseNow((dbShort*)theFld); break;
		case uLongField: SortSelectionInverseNow((dbUlong*)theFld); break;
		case longField: SortSelectionInverseNow((dbLong*)theFld); break;
		case dateTimeField: SortSelectionInverseNow((dbUlong*)theFld); break;
		case dateField: SortSelectionInverseNow((dbUlong*)theFld); break;
		case timeField: SortSelectionInverseNow((dbLong*)theFld); break;
		case realField: SortSelectionInverseNow((dbReal*)theFld); break;
		case charField: SortSelectionInverseNow((dbChar*)theFld); break;
		case textField: SortSelectionInverseNow((dbText*)theFld); break;

		case compoundField:
			{
				dbCompoundField* sortingBy = (dbCompoundField*)theFld;	// safe downcast
				dbSorter temp(*sortingBy);	// both descend from OOF_FieldSegOwner so can copy construct
				temp.toggleSegmentReversedFlags();
				sortSelectionNow(&temp);
				return;	// EARLY EXIT - recursive call has already done the following
			}
			break;

		case fixedBinaryField: {
			unsigned long fixedLength = theFld->fieldStorageLen();
			// NOT YET IMPLEMENTED - cope with 4-byte fields as special case
			if (fixedLength>3)
				SortSelectionInverseNow(theFld, fixedLength); 
			else {	// NOT YET IMPLEMENTED - cope with 2-byte fields with uShort sorter
				dbConnect::raise(stringstream() << flush 
					<< "Unable to Sort by field" << theFld->fieldName() 
					<< " SortSelectionInverseNow is not defined on fields of type"
					<< theFld->fieldType() << " < 4 bytes long",
					false /* don't terminate */
				);
			}
		}
		break;
		
		default:
			dbConnect::raise(stringstream() << flush 
				<< "Unable to Sort by field" << theFld->fieldName() 
				<< " SortSelectionInverseNow is not defined on fields of type"
				<< theFld->fieldType(),
				false /* don't terminate */
			);
		// NOT YET IMPLEMENTED
		;
	}

	assert( Invariant("simpleB::sortSelectionInverseNow-exit") );
}

void 
OOF_simpleRecordBackend::SortSelectionNow(const dbUlong* theFld)
{
	OOF_ULSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsert(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionNow(const dbLong* theFld)
{
	OOF_LSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsert(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void OOF_simpleRecordBackend::SortSelectionNow(const dbUshort* theFld)
{
	
	OOF_ULSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsert(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionNow(const dbShort* theFld)
{
	OOF_LSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsert(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionNow(const dbReal* theFld)
{
	OOF_DSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsert(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionNow(const dbChar* theFld)
{
	OOF_ULSorter lhs;
	unsigned long FirstFour;
			
   if (mDirtyRecordCache && mDirtyRecordCache->count()>0) {
		if (mSelection.state() == OOF_recordSelection::allRecs) {
		 // case one: all records selected with bufferforever and dirty record cache
		 
			unsigned long currentIterator = 0;
			oofString compareField;
			
			for(start();more();next()){
		     	OOF_copyFirstFour(&FirstFour,*theFld);
				lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
				bool doneFirstSwap = false;
		   	 	while(lhs.lastEqual()) {
					if(!doneFirstSwap){
		   	 			compareField = theFld->asChars(); // save before we lose selection!
		         		doneFirstSwap = true;
		       			currentIterator = mSelection.iter();// thats all the context we need as we know we are in select all
					}
					selectJustOID(lhs.theEqual()->mNext->mData); // have corrupteed selection to selectone
					if(OOF_stricmp(compareField,*theFld )>0)
						lhs.exchange(lhs.theEqual());
					else
						break;
		        } // loop looking for differences beyond first four bytes
	    		if(doneFirstSwap){
	    			selectAll();	
					gotoRecord(currentIterator);	//	but id does indicate we have been throught the loop at least once
	    		}
	    	}// sort loop	
	     }
	     else{  // NOT allRecs but still dirty cache
     	 	 
     	 	// Case two: some records selected with bufferforever and dirty record cache
		 
			OOF_recordSelection snapshot(mSelection);  // declare this temp outside the loop!
			const long numRecords = snapshot.count();
		   	oofString compareField;
		     
		    for(long i=0; i<numRecords; i++){
		        selectJustOID(snapshot.value(i));   
		        OOF_copyFirstFour(&FirstFour,*theFld);
		        lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
		        const oofString compareField = theFld->asChars(); // save before we lose selection!//david debug 000309
		   	 	while(lhs.lastEqual()) {
		          	selectJustOID(lhs.theEqual()->mNext->mData);
		            if(OOF_stricmp(compareField,*theFld )>0) 
		            	lhs.exchange(lhs.theEqual());
		            else
		            	break;
		        } // loop looking for differences beyond first four bytes
		      }  // sort loop
			}
   	}  // all vs some records
   	else {   // original - use if no dirty records
	// NOT YET IMPLEMENTED - probably faster to use case 2 above even if no 
	// dirty records - preserving original here to isolate impact of change
	// but later review performance
		dbTable* compareTable = theFld->fieldTable()->cloneTableWithoutSelection();
      	dbChar* compareField = (dbChar*)compareTable->field(theFld->fieldNumber());

     	for(start();more();next()) {
			FirstFour = 0;
			OOF_copyFirstFour(&FirstFour,*theFld);
			lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
	       	while(lhs.lastEqual()) {
	        	// Load the matching record into compareField
				compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
				if(OOF_stricmp(*theFld,*compareField)>0)
					lhs.exchange(lhs.theEqual());
				else
					break;
			}
	  	}  // sort loop
		delete compareTable;
  	}  // has dirty cache

   OOF_recordSelection newSelection( mTable->prototype() );
   while(!lhs.isEmpty())
      newSelection.append(lhs.remove());

	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionNow(const dbText* theFld)
{
	OOF_ULSorter lhs;
	dbTable* compareTable = theFld->fieldTable()->cloneTableWithoutSelection();
	dbText* compareField = (dbText *)compareTable->field(theFld->fieldNumber());

	for(start();more();next())
	{
		theFld->reset();
		unsigned long FirstFour = 0;
		OOF_copyFirstFour(&FirstFour,*theFld);
		lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
		while(lhs.lastEqual()) {
			// Load the matching record into compareField
			compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
			if(OOF_stricmp(*theFld,*compareField)>0)
				lhs.exchange(lhs.theEqual());
			else
				break;
		}
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	delete compareTable;
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void
OOF_simpleRecordBackend::SortSelectionNow(const dbField* theFld, unsigned long fixedBinaryLength)
{
// sort any arbitrary binary data of given length
	assert(fixedBinaryLength>3);
	
	OOF_ULSorter lhs;
	dbTable* compareTable = theFld->fieldTable()->cloneTableWithoutSelection();
	dbField* compareField = compareTable->field(theFld->fieldNumber());

	for(start();more();next())
	{
		unsigned long FirstFour = 0;
		const char* s1 = (const char*) theFld->binaryContents();
		OOF_copyFirstFour(&FirstFour,s1);
		lhs.sortedInsert(FirstFour, mCurrLoadedRecOffset);
		while(lhs.lastEqual()) {
			// Load the matching record into compareField
			compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
	
			// direct binary compare
			const char* s2 = (const char*) compareField->binaryContents();
			unsigned long i;
			for (i=0; (i<fixedBinaryLength) && (s1[i]==s2[i]); ++i)
				; 
			if(s1[i]>s2[i])
				lhs.exchange(lhs.theEqual());
			else
				break;
		}
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	delete compareTable;
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionInverseNow(const dbUlong* theFld)
{
	OOF_ULSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsertReverse(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionInverseNow(const dbLong* theFld)
{
	OOF_LSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsertReverse(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void OOF_simpleRecordBackend::SortSelectionInverseNow(const dbUshort* theFld)
{
	OOF_ULSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsertReverse(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionInverseNow(const dbShort* theFld)
{
	OOF_LSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsertReverse(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionInverseNow(const dbReal* theFld)
{
	OOF_DSorter lhs;

	for(start();more();next())
	{
		lhs.sortedInsertReverse(*theFld, mCurrLoadedRecOffset);
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}

void
OOF_simpleRecordBackend::SortSelectionInverseNow(const dbChar* theFld)
{
	OOF_ULSorter lhs;
	unsigned long FirstFour;
	
	if (mDirtyRecordCache && mDirtyRecordCache->count()>0) {
	   if(mSelection.state() == OOF_recordSelection::allRecs ){
		 
		    unsigned long currentIterator = 0;
			oofString compareField;
			
			for(start();more();next()){
		      	OOF_copyFirstFour(&FirstFour,*theFld);
				lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
				bool doneFirstSwap = false;
		   	 	while(lhs.lastEqual()) {
					if(!doneFirstSwap){
		   	 			compareField = theFld->asChars(); // save before we lose selection!
		         		doneFirstSwap = true;
		       			currentIterator = mSelection.iter();// that's all the context we need as we know we are in select all
					}
					selectJustOID(lhs.theEqual()->mNext->mData); // have corrupted selection to selectone
					if(OOF_stricmp(compareField,*theFld )<0)
						lhs.exchange(lhs.theEqual());
					else
						break;
		        } // loop looking for differences beyond first four bytes
	    		if(doneFirstSwap) {
	    			selectAll();	
					gotoRecord(currentIterator);	//	but it does indicate we have been through the loop at least once
	    		}
    		}// sort loop
	    }
		else{
			OOF_recordSelection snapshot(mSelection);  // declare this temp outside the loop!
		    const long numRecords = snapshot.count();
		    oofString compareField;
		    
		    for(long i=0; i<numRecords; i++){
		        selectJustOID(snapshot.value(i));   // replaces start/more/next
		        OOF_copyFirstFour(&FirstFour,*theFld);
		        lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
				bool doneFirstSwap = false;
		   	 	while(lhs.lastEqual()) {
		         	if(!doneFirstSwap){
		         		compareField = theFld->asChars(); // save before we lose selection!
		         		doneFirstSwap = true;
		         	}
		       		selectJustOID(lhs.theEqual()->mNext->mData);
		            if(OOF_stricmp(compareField,*theFld )<0)  // compareField not deref'd
		            	lhs.exchange(lhs.theEqual());
		            else
		            	break;
		        } // loop looking for differences beyond first four bytes
		    }  // sort loop
	   	}
	}
	else{  // original code if no dirty records
		dbTable* compareTable = theFld->fieldTable()->cloneTableWithoutSelection();
		dbChar* compareField = (dbChar *)compareTable->field(theFld->fieldNumber());
	
		for(start();more();next())
		{
			FirstFour = 0;
			OOF_copyFirstFour(&FirstFour,*theFld);
			lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
			while(lhs.lastEqual()) {
				// Load the matching record into compareField
				compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
				if(OOF_stricmp(*theFld,*compareField)<0)
					lhs.exchange(lhs.theEqual());
				else
					break;
			}
		}
		delete compareTable;
	}
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::SortSelectionInverseNow(const dbText* theFld)
{
	OOF_ULSorter lhs;
	dbTable* compareTable = theFld->fieldTable()->cloneTableWithoutSelection();
	dbText* compareField = (dbText *)compareTable->field(theFld->fieldNumber());

	for(start();more();next())
	{
		theFld->reset();
		unsigned long FirstFour = 0;
		OOF_copyFirstFour(&FirstFour,*theFld);
		lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
		while(lhs.lastEqual()) {
			// Load the matching record into compareField
			compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
			if(OOF_stricmp(*theFld,*compareField)<0)
				lhs.exchange(lhs.theEqual());
			else
				break;
		}
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	delete compareTable;
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void
OOF_simpleRecordBackend::SortSelectionInverseNow(const dbField* theFld, unsigned long fixedBinaryLength)
{
// sort any arbitrary binary data of given length
	assert(fixedBinaryLength>3);
	
	OOF_ULSorter lhs;
	dbTable* compareTable = theFld->fieldTable()->cloneTableWithoutSelection();
	dbField* compareField = compareTable->field(theFld->fieldNumber());

	for(start();more();next())
	{
		unsigned long FirstFour = 0;
		const char* s1 = (const char*) theFld->binaryContents();
		OOF_copyFirstFour(&FirstFour,s1);
		lhs.sortedInsertReverse(FirstFour, mCurrLoadedRecOffset);
		while(lhs.lastEqual()) {
			// Load the matching record into compareField
			compareTable->selectJustOID(lhs.theEqual()->mNext->mData);
	
			// direct binary compare
			const char* s2 = (const char*) compareField->binaryContents();
			unsigned long i;
			for (i=0; (i<fixedBinaryLength) && (s1[i]==s2[i]); ++i)
				; 
			if(s1[i]<s2[i])
				lhs.exchange(lhs.theEqual());
			else
				break;
		}
	}
	
	OOF_recordSelection newSelection( mTable->prototype() );
	while(!lhs.isEmpty())
		newSelection.append(lhs.remove());
			
	delete compareTable;
	mSelection=newSelection;
	start();  // Make sure the selection context is logical before returning.
}


void 
OOF_simpleRecordBackend::difference_with(const OOF_tableBackend* rhs)
{
	assert( Invariant("simpleB::difference_with-backend-entry") );

	OOF_simpleRecordBackend* theRHS = (OOF_simpleRecordBackend*) rhs;  // DANGEROUS downcast - may break if every have other than OOF_simpleRecordBackend as direct subclass of OOF_tableBackend
	mSelection.difference_with(theRHS->mSelection);  // calls back our invert() in some cases
	unloadRecord();

	assert( Invariant("simpleB::difference_with-backend-exit") );
}


void 
OOF_simpleRecordBackend::difference_with(const dbSelection& rhs)  
{
	assert( Invariant("simpleB::difference_with-entry") );

	mSelection.difference_with( rhs.internalSelection() );
	unloadRecord();

	assert( Invariant("simpleB::difference_with-exit") );
}


void 
OOF_simpleRecordBackend::intersection_with(const OOF_tableBackend* rhs)
{
	assert( Invariant("simpleB::intersection_with-tableBackend-entry") );

	OOF_simpleRecordBackend* theRHS = (OOF_simpleRecordBackend*) rhs;  // DANGEROUS downcast - may break if every have other than OOF_simpleRecordBackend as direct subclass of OOF_tableBackend
	mSelection.intersection_with(theRHS->mSelection);
	unloadRecord();

	assert( Invariant("simpleB::intersection_with-tableBackend-exit") );
}



void 
OOF_simpleRecordBackend::intersection_with(const dbSelection& rhs)  
{
	assert( Invariant("simpleB::intersection_with-dbSelection-entry") );

	mSelection.intersection_with( rhs.internalSelection() );
	unloadRecord();
	
	assert( Invariant("simpleB::intersection_with-dbSelection-exit") );
}


void 
OOF_simpleRecordBackend::union_with(const OOF_tableBackend* rhs)  
{
	assert( Invariant("simpleB::union_with-tableBackend-entry") );
	
	OOF_simpleRecordBackend* theRHS = (OOF_simpleRecordBackend*) rhs;  // DANGEROUS downcast - may break if every have other than OOF_simpleRecordBackend as direct subclass of OOF_tableBackend
	mSelection.union_with(theRHS->mSelection);
	unloadRecord();

	assert( Invariant("simpleB::union_with-tableBackend-exit") );
}


void 
OOF_simpleRecordBackend::union_with(const dbSelection& rhs)  
{
	assert( Invariant("simpleB::union_with-dbSelection-entry") );

	mSelection.union_with( rhs.internalSelection() );
	unloadRecord();

	assert( Invariant("simpleB::union_with-dbSelection-exit") );
}


void 
OOF_simpleRecordBackend::union_with_no_overlap(const OOF_tableBackend* rhs)  
{
	assert( Invariant("simpleB::union_with_no_overlap-tableBackend-entry") );
	
	OOF_simpleRecordBackend* theRHS = (OOF_simpleRecordBackend*) rhs;  // DANGEROUS downcast - may break if every have other than OOF_simpleRecordBackend as direct subclass of OOF_tableBackend
	mSelection.union_with_no_overlap(theRHS->mSelection);
	unloadRecord();

	assert( Invariant("simpleB::union_with_no_overlap-tableBackend-exit") );
}


void 
OOF_simpleRecordBackend::union_with_no_overlap(const dbSelection& rhs)  
{
	assert( Invariant("simpleB::union_with_no_overlap-dbSelection-entry") );

	mSelection.union_with_no_overlap( rhs.internalSelection() );
	unloadRecord();

	assert( Invariant("simpleB::union_with_no_overlap-dbSelection-exit") );
}


void
OOF_simpleRecordBackend::invert()
{
	assert( Invariant("simpleB::invert-entry") );

	switch(mSelection.state())
	{
	case OOF_recordSelection::empty:
		mSelection.selectAll();
		break;

	case OOF_recordSelection::allRecs:
		mSelection.selectNone();
		break;

	case OOF_recordSelection::oneRec:
	case OOF_recordSelection::someRecs:
// This is necessarily iterative and yucky...
// Loading the selection into a tree and doing member checks on that would
// be much faster..

// This also leaves the record pointer in an undefined state WRT the current selection

		OOF_List* theInversion = new OOF_List();
		// Save our current selection
		OOF_recordSelection savedSelection = mSelection;
		// Turn off sorting
		dbSorter* currentSorter = sorter();
		if (currentSorter)
			currentSorter->suspendSorting();
		selectAll();

		for(start();more();next())
			if(!savedSelection.contains(mCurrLoadedRecOffset))
				theInversion->append(mCurrLoadedRecOffset);

		switch(theInversion->count())		// Copy the tree back into a Selection
		{
		case 0:
			mSelection.selectNone();
			break;
		case 1:
			mSelection.selectOneRec(theInversion->remove());
			break;
		default: {
			OOF_recordSelection newSelection(OOF_recordSelection::someRecs, theInversion->count(), mTable->prototype());
			while (!theInversion->isEmpty())
				newSelection.append(theInversion->remove());
			mSelection = newSelection;
			break;
			}
		}
		delete theInversion;

		if (currentSorter)
			currentSorter->resumeSorting();
		break;
	}

	assert( Invariant("simpleB::invert - end, prior unloadRecord") );
	unloadRecord();

	assert( Invariant("simpleB::invert-exit") );
}


// -------------------------------------------------------
//               O O F _ S e l e c t i o n
//                 see also oofctre5.cpp
// -------------------------------------------------------
void 
OOF_recordSelection::difference_with(const OOF_recordSelection& rhs)
{
	// This one would especially benefit from a tree because of many calls to member

	switch(rhs.state())  // Branch on RHS State
	{
	case empty:
		break;
	
	case oneRec:		
		switch(state())		// What we do depends on LHS as well
		{
		case empty:
			break;
			
		case oneRec:
			if(contains(rhs.oneRecOffset()))
				selectNone();
			break;
			
		case someRecs: {
			unsigned long numLHS = count();
			OOF_List* theDifference = new OOF_List();
			// Copy the ones in the lhs that don't match the rhs item
			for (unsigned long i=0; i<numLHS; i++) {
				unsigned long current = value(i);
				if(!rhs.contains(current))
					theDifference->append(current);
			}
			switch(theDifference->count())		// Copy the tree back into a Selection
			{
			case 0:
				selectNone();
				break;
			case 1:
				selectOneRec(theDifference->remove());
				break;
			default: {
				OOF_recordSelection newSelection(someRecs, theDifference->count(), mPrototypicalTable);
				while (!theDifference->isEmpty())
					newSelection.append(theDifference->remove());			
				*this = newSelection;
				}
			}
			delete theDifference;
			break;
			}
			
		case allRecs:
			selectOneRec(rhs.oneRecOffset());
			invert(); // Copy the RHS to the LHS and invert
			break;
		}
		break;
	
	case someRecs:
		switch(state())		// What we do depends on A as well
		{
		case empty:
			break;

		case oneRec:
			if(rhs.contains(oneRecOffset()))
				selectNone();
			break;

		case someRecs: {
			OOF_List* theDifference = new OOF_List();
			unsigned long numLHS = count();
		
// This commented chunk will be better when the list class is not just a LL...
			
/*			OOF_List* theRHSlist = new OOF_List();
			rhs.start();							// Put the rhs into a list
			while (mSelection.more()) {
				theRHSlist->append(rhs());		// Don't sort - just bung 'em in
				next();
			}
*/		
			// Copy the ones in the lhs that aren't in the rhs
			for (unsigned long i=0; i<numLHS; i++) {
				unsigned long current = value(i);
				if(!rhs.contains(current))
					theDifference->append(current);
			}
		
			switch(theDifference->count())		// Copy the tree back into a Selection
			{
			case 0:
				selectNone();
				break;
			case 1:
				selectOneRec(theDifference->remove());
				break;
			default: {
				OOF_recordSelection newSelection(someRecs, theDifference->count(), mPrototypicalTable);
				while (!theDifference->isEmpty())
					newSelection.append(theDifference->remove());			
				*this = newSelection;
				}
			}
			delete theDifference;
/*			delete therhsList; */  // See Above
			break;
			}
			
		case allRecs: { // Copy the RHS to the LHS and invert
			unsigned long numRHS = rhs.count();
			OOF_recordSelection newSelection(someRecs, numRHS, mPrototypicalTable);
			for (unsigned long i=0; i<numRHS; i++)
				newSelection.append(rhs.value(i));
			*this = newSelection;
			invert();
			break;
			}
		}
		break;

	case allRecs:
		selectNone();
		break;
	}
}



void 
OOF_recordSelection::intersection_with(const OOF_Selection* rhs)
{
	intersection_with(*(OOF_recordSelection*)rhs);	// DANGEROUS downcast - may break if every have other than OOF_RecordSelection
}


void 
OOF_recordSelection::difference_with(const OOF_Selection* rhs)
{
	difference_with(*(OOF_recordSelection*)rhs);	// DANGEROUS downcast - may break if every have other than OOF_simpleRecordBackend as direct subclass of OOF_tableBackend
}


void 
OOF_recordSelection::intersection_with(oidT rhsOffset)
{
	switch(state())		// What we do depends on LHS as well
	{
	case empty:
		break;
		
	case oneRec:
		if(!contains(rhsOffset))
			selectNone();
		break;
		
	case someRecs:
		if(!contains(rhsOffset)) {
			selectNone();
		} 
		else {
			selectOneRec(rhsOffset);
		}
		break;
		
	case allRecs:
		selectOneRec(rhsOffset);
		break;
	}
}


void 
OOF_recordSelection::intersection_with(const OOF_recordSelection& rhs)
{
	// This one would especially benefit from a tree because of many calls to member
	switch(rhs.state())  // Branch on RHS State
	{
	case empty:
		selectNone();
		break;
	
	case oneRec:		
		switch(state())		// What we do depends on LHS as well
		{
		case empty:
			break;
			
		case oneRec:
			if(!contains(rhs.oneRecOffset()))
				selectNone();
			break;
			
		case someRecs:
			if(!contains(rhs.oneRecOffset())) {
				selectNone();
			} 
			else {
				selectOneRec(rhs.oneRecOffset());
			}
			break;
			
		case allRecs:
			copyContents(rhs);
			break;
		}
		break;
	
	case someRecs:  // rhs
		switch(state())		// What we do depends on A as well
		{
		case empty:
			break;

		case oneRec:
			if(!rhs.contains(oneRecOffset()))
				selectNone();
			break;

		case someRecs: {
			OOF_List* theIntersection = new OOF_List();
			unsigned long numRHS = rhs.count();
			
// This commented chunk will be better when the list class is not just a LL...
			
/* 			OOF_List* thelhs = new OOF_List();
			mSelection.start();						// Put the lhs into a list
			while (mSelection.more()) {
				thelhs->append(mSelection());		// Don't sort - just bung 'em in
				mSelection.next();
			}
*/
					// Copy the ones that are in both to a new list
			for (unsigned long i=0; i<numRHS; i++) {
/*				if(thelhs->member(theRhs->mSelection()))*/    // See above
				unsigned long rhsCurrent = rhs.value(i);
				if (contains(rhsCurrent))
					theIntersection->append(rhsCurrent);
			}
			switch(theIntersection->count())		// Copy the tree back into a Selection
			{
			case 0:
				selectNone();
				break;
			case 1:
				selectOneRec(theIntersection->remove());
				break;
			default: {
				OOF_recordSelection newSelection(someRecs, theIntersection->count(), mPrototypicalTable);
				while (!theIntersection->isEmpty())
					newSelection.append(theIntersection->remove());			
				*this = newSelection;
				}
			}
			delete theIntersection;
/*			delete thelhs;*/  // See above
			break;
			}

		case allRecs: {
			copyContents(rhs);
			break;
			}
		}
		break;

	case allRecs:  // rhs
		break;
	}
}


void 
OOF_recordSelection::union_with(const OOF_Selection* rhs)
{
	union_with(*(OOF_recordSelection*)rhs);	// DANGEROUS downcast - may break if every have other than OOF_RecordSelection
}


/**
	Implement optimal union_with when combining just one OID on rhs.
*/
void 
OOF_recordSelection::union_with(oidT rhsOffset)
{
		if(!contains(rhsOffset))
		{
			switch(state())		// What we do depends on LHS as well
			{
			case empty:
				selectOneRec(rhsOffset);
				break;
				
			case oneRec: {
				OOF_recordSelection newSelection(someRecs, 2, mPrototypicalTable);
				newSelection.append(oneRecOffset());
				newSelection.append(rhsOffset);
				
				*this = newSelection;
				break;
				}
				
			case someRecs:
				append(rhsOffset);
				break;
				
			case allRecs:
				break;
			}
		}
}


/**
	Implement union_with set operation on current selection with state changes.
	\todo transitions to selectAll state if combined someRecs with oneRec or someRecs
*/
void 
OOF_recordSelection::union_with(const OOF_recordSelection& rhs)  
{
	switch(rhs.state())  // Branch on RHS State
	{
	case empty:
	
		break;   // If B is empty then A = A + B returns A !
	
	case oneRec:		
		if(!contains(rhs.oneRecOffset()))
		{
			switch(state())		// What we do depends on LHS as well
			{
			case empty:
				selectOneRec(rhs.oneRecOffset());
				break;
				
			case oneRec: {
				OOF_recordSelection newSelection(someRecs, 2, mPrototypicalTable);
				newSelection.append(rhs.oneRecOffset());
				newSelection.append(oneRecOffset());
				
				*this = newSelection;  // getting the someRecs state created above
				break;
				}
				
			case someRecs:
				append(rhs.oneRecOffset());
				break;
				
			case allRecs:
				break;
			}
		}
		break;
	
	case someRecs: {
		unsigned long numRHS = rhs.count();
		switch(state())			// What we do depends on A as well
		{
		case empty: {
			OOF_recordSelection newSelection(someRecs, numRHS, mPrototypicalTable);
			for (unsigned long i=0; i<numRHS; i++)
				newSelection.append(rhs.value(i));
			*this = newSelection;
			break;
		}

		case oneRec: {
			bool Copied = false;
			OOF_recordSelection newSelection(someRecs, numRHS, mPrototypicalTable);
			for (unsigned long i=0; i<numRHS; i++) {
				newSelection.append(rhs.value(i));
				Copied = (Copied || (rhs.value(i) == oneRecOffset()) );
			}				
			if(!Copied)
				newSelection.append(oneRecOffset());
/* NOT YET IMPLEMENTED
			if (newSelection.count()==NbrOfRecords(mISAMdatno)) {
				selectAll();
			} else {
				mSelection = newSelection;
			}
*/
			*this = newSelection;
			break;
		}

		case someRecs: {
			unsigned long numLHS = count();
			OOF_List* theUnion = new OOF_List();
			// Sort the LHS
			{
				for (unsigned long i=0; i<numLHS; i++) 
					theUnion->sortedInsert(value(i));
			}
			{
				for (unsigned long j=0; j<numRHS; j++)// Insert the RHS ignoring duplicates
					theUnion->sortedInsertNoDups(rhs.value(j));
			}	
			OOF_recordSelection newSelection(someRecs, numRHS, mPrototypicalTable);
			while (!theUnion->isEmpty())
				newSelection.append(theUnion->remove());		// Copy the tree back into a Selection
			delete theUnion;
// NOT YET IMPLEMENTED - work out if we have now produced a selection of all recs
// and change state accordingly (was in original OOF_ctreeBackend version as below)
/*
			if (newSelection.count()==NbrOfRecords(mISAMdatno)) {
				selectAll();
			} else {
				mSelection = newSelection;
			}
*/
			*this = newSelection;
			break;
		}
			
		case allRecs:
			break;
		}
		break;
	}
	
	case allRecs:  // in rhs
		selectAll();
		break;
	}
}


void 
OOF_recordSelection::union_with_no_overlap(const OOF_Selection* rhs)
{
	union_with_no_overlap(*(OOF_recordSelection*)rhs);	// DANGEROUS downcast - may break if every have other than OOF_RecordSelection
}


/**
	Dangerous implementation with no checking.
	May allow you to create selections with duplicate record pointers.
	Use very much at own risk!
	\todo investigate if can be faster in case where empty combined with someRecs
	\todo transitions to selectAll state if combined someRecs with oneRec or someRecs
*/
void 
OOF_recordSelection::union_with_no_overlap(const OOF_recordSelection& rhs)  
{
	switch(rhs.state())  // Branch on RHS State
	{
	case empty:
	
		break;   // If B is empty then A = A + B returns A !
	
	case oneRec:		
		{
			switch(state())		// What we do depends on LHS as well
			{
			case empty:
				selectOneRec(rhs.oneRecOffset());
				break;
				
			case oneRec: {
				OOF_recordSelection newSelection(someRecs, 2, mPrototypicalTable);
				newSelection.append(rhs.oneRecOffset());
				newSelection.append(oneRecOffset());
				
				*this = newSelection;  // getting the someRecs state created above
				break;
				}
				
			case someRecs:
				append(rhs.oneRecOffset());
				break;
				
			case allRecs:
				break;
			}
		}
		break;
	
	case someRecs: {
		unsigned long numRHS = rhs.count();
		switch(state())			// What we do depends on A as well
		{
		case empty: {
			OOF_recordSelection newSelection(someRecs, numRHS, mPrototypicalTable);
			for (unsigned long i=0; i<numRHS; i++)
				newSelection.append(rhs.value(i));
			*this = newSelection;
			break;
		}

		case oneRec: {
			bool Copied = false;
			OOF_recordSelection newSelection(someRecs, numRHS+1, mPrototypicalTable);
			for (unsigned long i=0; i<numRHS; i++) {
				newSelection.append(rhs.value(i));
			}				
			newSelection.append(oneRecOffset());  // append our single after blind copy loop
			*this = newSelection;
			break;
		}

		case someRecs: {
			unsigned long numLHS = count();
			expandToInclude(numRHS+numLHS);
			for (unsigned long i=0; i<numRHS; i++) {
				append(rhs.value(i));
			}				
			break;
		}
			
		case allRecs:
			break;
		}
		break;
	}
	
	case allRecs:  // in rhs
		selectAll();
		break;
	}
}


void 
OOF_recordSelection::invert()
{
	assert(mPrototypicalTable);  // no business calling this on uninitialised selection!
	dbTable* tempTable = mPrototypicalTable->cloneTableWithoutSelection();
	tempTable->setSelection(this);
	tempTable->invert();
	OOF_recordSelection* newSel = (OOF_recordSelection*) tempTable->currentSelection().internalSelection();  // DANGEROUS downcast if ever other sel types
	// but still reasonably safe - we've just created the bloody thing anyway and it's our prototypical table, so should
	// be compatible with me!
	
	*this = *newSel;
	delete tempTable;
}

