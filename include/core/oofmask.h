// COPYRIGHT 1998 A.D. Software, All rights reserved

#ifndef H_OOFMASK
#define H_OOFMASK

#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif
#ifndef H_OOFARRAY
	#include "oofarray.h"
#endif

/**
	string masking class used primarily for web page filling.
	makes it easy to setup a template file with a user-defined
	masking scheme and fill it in.
	\ingroup oofUtils
*/
class oofMaskMultiStrings {
public:
	oofMaskMultiStrings();
	~oofMaskMultiStrings();
	void allocBuffer(unsigned long paramLen);	
	void loadFile(const oofFileRef&);
	void startChunks(unsigned short startAt=0);  // implied by ctor
	bool finishedParse();
	
// output methods
	char* orphanBuffer();
	unsigned long filledLen() const;
	unsigned long totalChunkLen();

// constructing the mask	
	void appendTarget(const oofString&);
	void appendTarget(const oofString&,const oofString&);

	void appendChunk(unsigned long offset, unsigned long len);

// filling in masked values	
	void copyParamAfterChunk(const oofString&);
	void copyParam(const oofString&);
	void copyNextChunk();
	void copyChunk(unsigned short chunkIndex);
	void copyFinalChunk();
	
protected:
	oofString	mSource;
	char* mBuffer;	// owned
	OOF_ExpandableLongArray	mChunks;
	unsigned long mBufferLen, mAppendCursor, mNextChunkStartsAt, mCachedTotalLen;
	bool mCopiedFinalChunk;
	unsigned short mNextChunkIndex;
};

#endif

