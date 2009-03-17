#ifndef H_OOFREPREND
#define H_OOFREPREND
// COPYRIGHT 1997 A.D. Software, All rights reserved

// abstract base classes for report renderers
// there is one abstract base for each drawable oofRepDrawable
// (note that some intermediate oofRepDrawables are containers or abstract bases)

// extra pure virtual members are defined for the classes where local measurement
// will be required, eg: the width() of a text block in a Mac GUI

class oofRepSpaceBand;
class oofRepTextBand;
class oofRepGraphBand;
class oofRepLayoutBand;
class oofRepLineBand;
class oofRepViewBand;
class oofRepTextBlock;
class oofRepFieldBlock;
class oofRepPageNoBlock;

/**
	Abstract base for rendering an oofRepSpaceBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepSpaceBand_Render : public oofRepRender {
protected:
	oofRepSpaceBand_Render(oofRepSpaceBand* b) : mOwner(b) {};

// data storage
	oofRepSpaceBand* mOwner;
friend class oofRepSpaceBand;	// owner and creator class
};


/**
	Abstract base for rendering an oofRepGraphBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepGraphBand_Render : public oofRepRender {
protected:
	oofRepGraphBand_Render(oofRepGraphBand* b) : mOwner(b) {};

// data storage
	oofRepGraphBand* mOwner;
friend class oofRepGraphBand;	// owner and creator class
};


/**
	Abstract base for rendering an oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepLayoutBand_Render : public oofRepRender {
protected:
	oofRepLayoutBand_Render(oofRepLayoutBand* b) : mOwner(b) {};

void SetCurrentBlockCoords(short inX, short inY, oofRepBlock*);
	
public:  // reflective API
	oofRepLayoutBand* ownerLayout() const;
	void getCurrentBlockCoords(short& outX, short& outY, oofRepBlock*& outBlock) const;
	
protected:
// data storage
	oofRepLayoutBand* mOwner;
	short mCurrentX, mCurrentY;
	oofRepBlock* mCurrentBlock;
	
friend class oofRepLayoutBand;	// owner and creator class
};


/**
	Abstract base for rendering an oofRepLineBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepLineBand_Render : public oofRepRender {
protected:
	oofRepLineBand_Render(oofRepLineBand* b) : mOwner(b) {};

// data storage
	oofRepLineBand* mOwner;
friend class oofRepLineBand;	// owner and creator class
};


/**
	Abstract base for rendering an oofRepPictBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepPictBand_Render : public oofRepRender {
protected:
	oofRepPictBand_Render(oofRepPictBand* b) : mOwner(b) {};

// data storage
	oofRepPictBand* mOwner;
friend class oofRepPictBand;	// owner and creator class
};


/**
	Add extra virtual methods used by oofRep::DrawViewBand beyond normal band drawing.
	Declares null methods rather than virtuals as subclasses typically only override
	one or two.
	\ingroup oofRepEdit
*/
class OOFREP_EXPORT OOF_mixViewExtraRender {
public:
	virtual void startTableBody(bool /*recalcHeaders*/) {}; 
	virtual void endTableBody() {};
	virtual void startTableWrapper() {};
	virtual void endTableWrapper() {};
	
	
	virtual void drawColumnHeaders() {};
	
	virtual OOF_mixViewExtraRender* finishClone(oofRepRender* newClone) const=0;
};


/**
	Abstract base for rendering an oofRepViewBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepViewBand_Render : public oofRepRender, public OOF_mixViewExtraRender {
protected:
	oofRepViewBand_Render(oofRepViewBand* b) : 
		mOwner(b),
		mDrawnColHeadersThisPage(false)
	{};
	virtual void finishConstruction ();

	virtual OOF_mixViewExtraRender* finishClone(oofRepRender* newClone) const;
	
	virtual void CalcColWidths();

public:
	oofRepViewBand* viewBand() const;


protected:
// data storage
	oofRepViewBand* mOwner;
	bool	mDrawnColHeadersThisPage;
friend class oofRepViewBand;	// owner and creator class
};


/**
	Abstract base for rendering an oofRepTextBlock within a oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepTextBlock_Render : public oofRepRender {
protected:
	oofRepTextBlock_Render(oofRepTextBlock* b) : mOwner(b) {};

// data storage
	oofRepTextBlock* mOwner;
friend class oofRepTextBlock;	// owner and creator class
};


/**
	Abstract base for rendering an oofRepPictBlock within a oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepPictBlock_Render : public oofRepRender {
protected:
	oofRepPictBlock_Render(oofRepPictBlock* b) : mOwner(b) {};

// data storage
	oofRepPictBlock* mOwner;
friend class oofRepPictBlock;	// owner and creator class
};


/**
	Abstract base for rendering an oofRepPageNoBlock within a oofRepLayoutBand.
	\ingroup oofRW
*/
class OOFREP_EXPORT oofRepPageNoBlock_Render : public oofRepRender {
public:
	oofRepPageNoBlock_Render(oofRepPageNoBlock* b) : mOwner(b) {};

// data storage
	oofRepPageNoBlock* mOwner;
friend class oofRepPageNoBlock;	// owner and creator class
};


// -------------------------------------------------------
//   o o f R e p V i e w B a n d_ R e n d e r
// -------------------------------------------------------

inline oofRepViewBand* 
oofRepViewBand_Render::viewBand() const
{
	return mOwner;
}


// -------------------------------------------------------
//   o o f R e p L a y o u t B a n d_ R e n d e r
// -------------------------------------------------------

inline oofRepLayoutBand* 
oofRepLayoutBand_Render::ownerLayout() const 
{ 
	return mOwner; 
}


inline void 
oofRepLayoutBand_Render::getCurrentBlockCoords(short& outX, short& outY, oofRepBlock*& outBlock) const
{
	outX = mCurrentX;
	outY = mCurrentY;
	outBlock = mCurrentBlock;
}



inline void 
oofRepLayoutBand_Render::SetCurrentBlockCoords(short inX, short inY, oofRepBlock* inBlock)
{
	mCurrentX = inX;
	mCurrentY = inY;
	mCurrentBlock = inBlock;
}



#endif


