// COPYRIGHT 1998 A.D. Software, All rights reserved

// string masking classes used primarily for web page filling
#ifndef H_OOFMASK
	#include "oofmask.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFSTR
	#include "oofstr.h"
#endif
#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif
#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif



// -------------------------------------------------------
//        o o f M a s k M u l t i S t r i n g s
// -------------------------------------------------------
oofMaskMultiStrings::oofMaskMultiStrings() :
	mBuffer(0),
	mBufferLen(0),
	mAppendCursor(0),
	mCachedTotalLen(0),
	mNextChunkStartsAt(0),
	mCopiedFinalChunk(false),
	mNextChunkIndex(0)
{}


oofMaskMultiStrings::~oofMaskMultiStrings()
{
	delete[] mBuffer;
	const unsigned long numChunks = mChunks.count();
	for (unsigned long i=0; i<numChunks; i++) {
		oofString* theChunk = (oofString*)mChunks.value(i);
		delete theChunk;
	}
}


void 
oofMaskMultiStrings::allocBuffer(unsigned long paramLen)
{
	if (!mCopiedFinalChunk && mNextChunkStartsAt>0) {  // we've been appending targets, need to generate final chunk
		long remainingChars = mSource.length()-mNextChunkStartsAt;
		if (remainingChars>0) {
			appendChunk(mNextChunkStartsAt, remainingChars);
		}
		mCopiedFinalChunk = true;
	}

	const unsigned long newLen = paramLen + totalChunkLen();
	if (!mBuffer || newLen!=mBufferLen) {  // avoid realloc if same size
		delete[] mBuffer;
		mBufferLen = newLen;
		mBuffer = new char[newLen+1];
	}
	// cope with re-runs
	mAppendCursor = 0;	
	mNextChunkIndex = 0;
}


unsigned long
oofMaskMultiStrings::totalChunkLen()
{
	if (mCachedTotalLen==0) {
		const unsigned long numChunks = mChunks.count();
		unsigned long accumLen=0;
	for (unsigned long i=0; i<numChunks; i++) {
		const oofString* theChunk = (const oofString*)mChunks.value(i);
			accumLen += theChunk->length();
		}
		mCachedTotalLen = accumLen;
	}
	return mCachedTotalLen;
}

	
void 
oofMaskMultiStrings::loadFile(const oofFileRef& theFile)
{
	// call this BEFORE specifying targets as they need to use it to search 
#ifdef OOF_USING_EXCEPTIONS
	try {
		mSource = theFile.readTextFile();
	} 
	#ifdef _Macintosh
		catch(const OSErr& errCode) {
			cout << "Text file not found, err " << errCode << " trying to load file " << theFile << endl;
		}
	#else
		catch(...) {
			cout << "Text file not found, or other err trying to load file " << theFile << endl;
		}
	#endif
#else
// NOT YET IMPLEMENTED - any error trapping
	mSource = theFile.readTextFile();
#endif
}

	
char* 
oofMaskMultiStrings::orphanBuffer()
{
	char* ret = mBuffer;
	mBuffer = 0;
	// don't clear mBufferLen as will commonly call filledLen at same time as this method
	// so need it to remain saying how big this is
	return ret;
}


unsigned long 
oofMaskMultiStrings::filledLen() const
{
	return mBufferLen;
}


void 
oofMaskMultiStrings::appendChunk(unsigned long offset, unsigned long len)
{
	const char* chars = mSource.chars() + offset;
	oofString* adoptedChunk = new oofString(chars, len);
	mChunks.append((long)adoptedChunk);
}


void 
oofMaskMultiStrings::appendTarget(const oofString& inTarget)
{
// the chunks we append are the bits BETWEEN the targets - the assumption
// for now being that we are given the targets in ascending order, and so
// mNextChunkStartsAt advances through the source
	assert(mSource.length()>0);		// no point before read in file!
	const unsigned long targetAt = mSource.find(inTarget, mNextChunkStartsAt); // assumption of ascending order
	if (targetAt!=oofString::kNotFound) {
		assert(targetAt > mNextChunkStartsAt);  
		const unsigned long chunkLen = targetAt - mNextChunkStartsAt;
		appendChunk(mNextChunkStartsAt, chunkLen);
		mNextChunkStartsAt = targetAt+inTarget.length();
	}
	else
		RAISE_EXCEPTION(oofE_General(oofString("oofMaskMultiStrings::appendTarget Target: '"+inTarget+"' not found in:\n"+mSource)) );
}


void 
oofMaskMultiStrings::appendTarget(const oofString& inTarget,const oofString& terminator)
{
// the chunks we append are the bits BETWEEN the targets - the assumption
// for now being that we are given the targets in ascending order, and so
// mNextChunkStartsAt advances through the source
	assert(mSource.length()>0);		// no point before read in file!
	const unsigned long targetAt = mSource.find(inTarget, mNextChunkStartsAt); // assumption of ascending order
	const unsigned long targetEndAt = mSource.find(terminator, targetAt );
	
	if (targetAt!=oofString::kNotFound) {
		assert(targetAt > mNextChunkStartsAt);  
		const unsigned long chunkLen = targetAt - mNextChunkStartsAt;
		appendChunk(mNextChunkStartsAt, chunkLen);
		mNextChunkStartsAt = targetAt+inTarget.length();
	
		if (targetEndAt!=oofString::kNotFound) {
		assert(targetEndAt > mNextChunkStartsAt);  
		const unsigned long chunkLen = targetEndAt - mNextChunkStartsAt;
		appendChunk(mNextChunkStartsAt, chunkLen);
		mNextChunkStartsAt = targetEndAt+inTarget.length();
		}
	else
		RAISE_EXCEPTION(oofE_General(oofString("oofMaskMultiStrings::appendTarget cdml terminator ']' not found in:\n"+mSource)) );
		
	
	}
	else // no or no more cdml tags so just apphend the whole string
		
	//	RAISE_EXCEPTION(oofE_General(oofString("oofMaskMultiStrings::appendTarget Target: '"+inTarget+"' not found in:\n"+mSource)) );
	appendChunk(mNextChunkStartsAt, mSource.length()-mNextChunkStartsAt);
	

}


void
oofMaskMultiStrings::startChunks(unsigned short startAt)
{
	mNextChunkIndex = startAt;
}


void 
oofMaskMultiStrings::copyParamAfterChunk(const oofString& param)
{
	copyNextChunk();
	copyParam(param);
}


void 
oofMaskMultiStrings::copyParam(const oofString& param)
{
	const unsigned long paramLen = param.length();
	if (paramLen>0) {
		assert((mAppendCursor+paramLen)<=mBufferLen);
		memcpy(&mBuffer[mAppendCursor], param.chars(), paramLen);
		mAppendCursor+=paramLen;
	}
}


void 
oofMaskMultiStrings::copyNextChunk()
{
	copyChunk(mNextChunkIndex);
	mNextChunkIndex++;
}


void 
oofMaskMultiStrings::copyChunk(unsigned short chunkIndex)
{
	assert(chunkIndex < mChunks.count());
	const oofString* theChunk = (const oofString*)mChunks.value(chunkIndex);
	assert(theChunk);
	copyParam(*theChunk);
	mBuffer[mBufferLen] = '\0';  // do this every time to guarantee string always terminated
	// ASSERT our allocation adds 1 to the total length expected
}


void 
oofMaskMultiStrings::copyFinalChunk()
{
	copyChunk(mChunks.count()-1);
}
	
bool 
oofMaskMultiStrings::finishedParse()
{
	return !(mNextChunkStartsAt < mSource.length());



}
