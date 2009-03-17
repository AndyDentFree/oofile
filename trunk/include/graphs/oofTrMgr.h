// COPYRIGHT 1996 A.D. Software, All rights reserved

#ifndef H_OOFTRMGR
#define H_OOFTRMGR
// =================================================================================
//	oofTrMgr.h
// =================================================================================

#ifndef H_OOF0
	#include "oof0.h"
#endif

#ifdef _Macintosh
	#include <Quickdraw.h>
#endif

#ifdef _Windows
	#include "WinTypes.h"
#endif

/**
	Map ideal values to actual drawing values.
	Provides consistent rescaling of rectangles and lines.
	\note Ken's original idea was to provide more abstract drawing in 3D by 
	wrapping all transforms and thus performing distorts but not sure if
	the graphing engine has assumptions that bypass this ability.
	\see oofPlotBits which owns instance of us.
	\ingroup oofGraph
*/
class OOFGRAPH_EXPORT oofTransformationManager {
public:
	oofTransformationManager();
	virtual void	setIdealRect(oofRect& inRect);
	virtual void	ExpandToInclude(long x,long y);
	virtual void	setPaneRect(oofRect& inRect);
	virtual oofRect getPaneRect() const // EV 8/13/98
					{ return mPaneRect; }
	virtual	oofRect&	getIdealRect();
	virtual void	clear();
	
	virtual	oofRect	getMargins();
	virtual	void	setMargins(oofRect& inRect);
	
	// HACK !!!!
	virtual	Point	getTickMargins();
	virtual	void	setTickMargins(Point inPoint);
	
	// transform from ideal oofRect to draw oofRect
	virtual void	TransformPoint(int idealX, int idealY,
						int &drawX, int &drawY);
	virtual void	TransformRect( const oofRect *inIdealRect,
						oofRect *outDrawRect );
	virtual void	TransformVeLength( int idealLength,
						int &drawLength );
	virtual void	TransformHoLength( int idealLength,
						int &drawLength );
	virtual void	TransformVeLength( int idealLength,
						double &drawLength );
	virtual void	TransformHoLength( int idealLength,
						double &drawLength );
	virtual void	TransformTextSize( int idealTextSize, int &drawTextSize );
	// dump state
	virtual void 	describe(ostream& os) const;
	
	
	// transform from draw oofRect to ideal oofRect
	virtual	void	InverseTransformPoint(int drawX, int drawY,
								int &idealX, int &idealY);
	virtual	void	InverseTransformRect( const oofRect *inDrawRect,
								oofRect *outIdealRect );
	virtual	void	InverseTransformVeLength( int drawLength,
								int &idealLength );
	virtual	void	InverseTransformHoLength( int drawLength,
								int &idealLength );
	
	bool			isDirty() const;
	void			setToClean();
	
protected:
	void			setDrawRect();

private:
	oofRect	mIdealRect;
	oofRect	mPaneRect;
	oofRect	mDrawRect;
	int		mLeftMargin;
	int		mRightMargin;
	int		mTopMargin;
	int		mBottomMargin;
	
	// HACK !!
	int		mXTickLabelHeight;
	int		mYTickLabelWidth;
	
	bool	mDirty;
};
#endif
