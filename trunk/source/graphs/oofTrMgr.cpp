// COPYRIGHT 1996 A.D. Software, All rights reserved

// =================================================================================
//	oofTrMgr.cp
// =================================================================================

#include "oofpch_g.h"  // for precompilation of graph files

#ifndef H_OOFRECT
	#include "oofRect.h"
#endif
#ifndef H_OOFTRMGR
	#include "oofTrMgr.h"
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// ---------------------------------------------------------------------------------
//		 oofTransformationManager
// ---------------------------------------------------------------------------------

oofTransformationManager::oofTransformationManager() :
	mIdealRect(0, 0, 0, 0),
	mPaneRect(0, 0, 0, 0)
{
	mDirty = true;
	mLeftMargin=2;
	mRightMargin=2;
	mTopMargin=2;
	mBottomMargin=2;
	mXTickLabelHeight=0;
	mYTickLabelWidth=0;
	setDrawRect();
}


// ---------------------------------------------------------------------------------
//		 clear
// ---------------------------------------------------------------------------------

void
oofTransformationManager::clear()
{
//	mIdealRect.set(0, 0, 0, 0);
	mPaneRect.set(0, 0, 0, 0);
	mDirty = true;
	mLeftMargin=2;
	mRightMargin=2;
	mTopMargin=2;
	mBottomMargin=2;
	mXTickLabelHeight=0;
	mYTickLabelWidth=0;
	setDrawRect();
}

	
// ---------------------------------------------------------------------------------
//		 setIdealRect
// ---------------------------------------------------------------------------------

void
oofTransformationManager::setIdealRect( oofRect& inRect )
{
	if (!mIdealRect.isRectEqual(&inRect)) {
		mIdealRect	= inRect;
		mDirty=true;
	} 
}


// ---------------------------------------------------------------------------------
//		 ExpandToInclude
// ---------------------------------------------------------------------------------

void
oofTransformationManager::ExpandToInclude(long x,long y)
{
	// REMEMBER: The ideal rectangle is in the FIRST quadrant - see below
	if(x<mIdealRect.left){
		mIdealRect.left=x;
		mDirty = true;
	}
	if(x>mIdealRect.right){
		mIdealRect.right=x;
		mDirty = true;
	}
	// NOTE: This stuff is not what you'd expect !
	if(y>mIdealRect.top){
		mIdealRect.top=y;
		mDirty = true;
	}
	if(y<mIdealRect.bottom){
		mIdealRect.bottom=y;
		mDirty = true;
	}
}

// ---------------------------------------------------------------------------------
//		 setPaneRect
// ---------------------------------------------------------------------------------

void
oofTransformationManager::setPaneRect( oofRect& inRect )
{
	if (!mPaneRect.isRectEqual(&inRect)) {
		mPaneRect	= inRect;
		setDrawRect();
		mDirty=true;
	}
}


// ---------------------------------------------------------------------------------
//		 getIdealRect
// ---------------------------------------------------------------------------------

oofRect&
oofTransformationManager::getIdealRect()
{
	return mIdealRect;
}

// ---------------------------------------------------------------------------------
//		 getMargins
// ---------------------------------------------------------------------------------


oofRect
oofTransformationManager::getMargins()
{
	oofRect	r(mLeftMargin, mTopMargin, mRightMargin, mBottomMargin);
	return r;
}

// ---------------------------------------------------------------------------------
//		 setMargins
// ---------------------------------------------------------------------------------

void
oofTransformationManager::setMargins(oofRect& inRect)
{
	mTopMargin		= inRect.top;
	mLeftMargin		= inRect.left;
	mBottomMargin	= inRect.bottom;
	mRightMargin	= inRect.right;
	setDrawRect();
	mDirty			= true;
}

// ---------------------------------------------------------------------------------
//		 getTickMargins
// ---------------------------------------------------------------------------------


Point
oofTransformationManager::getTickMargins()
{
	Point p={ mXTickLabelHeight, mYTickLabelWidth };
	
	return p;
}

// ---------------------------------------------------------------------------------
//		 setTickMargins
// ---------------------------------------------------------------------------------

void
oofTransformationManager::setTickMargins(Point inPoint)
{
	if(inPoint.h!=mYTickLabelWidth) {
		mLeftMargin=mLeftMargin+(inPoint.h-mYTickLabelWidth);
		mYTickLabelWidth=inPoint.h;
		mDirty=true;
	}
	if(inPoint.v!=mXTickLabelHeight) {
		mBottomMargin=mBottomMargin+(inPoint.v-mXTickLabelHeight);
		mXTickLabelHeight=inPoint.v;
		mDirty=true;
	}
	//if(mDirty) 
		setDrawRect();
}

// ---------------------------------------------------------------------------------
//		 setDrawRect()
// ---------------------------------------------------------------------------------

void
oofTransformationManager::setDrawRect()
{
	mDrawRect.set(mPaneRect.left+mLeftMargin,mPaneRect.top+mTopMargin,
					 mPaneRect.right-mRightMargin,mPaneRect.bottom-mBottomMargin);
	mDrawRect.correct();
	assert((mDrawRect.rawWidth()!=0)&&(mDrawRect.rawHeight()!=0)); //stop div by zero in trandforms
	
}

// ---------------------------------------------------------------------------------
//		 isDirty
// ---------------------------------------------------------------------------------

bool
oofTransformationManager::isDirty() const
{
	return mDirty;
}

// ---------------------------------------------------------------------------------
//		 Clean
// ---------------------------------------------------------------------------------

void
oofTransformationManager::setToClean()
{
	mDirty=false;
}

// ---------------------------------------------------------------------------------
//		 TransformPoint
// ---------------------------------------------------------------------------------

void
oofTransformationManager::TransformPoint(int idealX, int idealY,
									int &drawX, int &drawY)
{
	assert((mDrawRect.rawWidth()!=0)&&(mDrawRect.rawHeight()!=0)); //stop div by zero in trandforms
	drawX = (int)((float)(idealX - mIdealRect.left) * 
				  ((float)(mDrawRect.rawWidth()) / (float)(mIdealRect.rawWidth())) + 
				  0.5) + 
			mDrawRect.left;
		
	drawY = (int)((float)(mIdealRect.top - idealY) * 
				  ((float)(mDrawRect.rawHeight()) / (float)(mIdealRect.invHeight())) +
				  0.5) + 
			mDrawRect.top;
}


// ---------------------------------------------------------------------------------
//		 TransformRect
// ---------------------------------------------------------------------------------

void
oofTransformationManager::TransformRect( const oofRect *inIdealRect,
						oofRect *outDrawRect )
{
	
	assert((mDrawRect.rawWidth()!=0)&&(mDrawRect.rawHeight()!=0)); //stop div by zero in trandforms
	
	outDrawRect->left =
		(int)((float)(inIdealRect->left - mIdealRect.left)  * ((float)(mDrawRect.rawWidth()) /
		(float)(mIdealRect.rawWidth()))+0.5) + mDrawRect.left;
		
	outDrawRect->top =
		(int)((float)(mIdealRect.top - inIdealRect->top) * ((float)(mDrawRect.rawHeight()) /
		(float)(mIdealRect.invHeight()))+0.5) + mDrawRect.top;
		
	outDrawRect->right =
		(int)((float)(inIdealRect->right - mIdealRect.left)  * ((float)(mDrawRect.rawWidth()) /
		(float)(mIdealRect.rawWidth()))+0.5) + mDrawRect.left;
		
	outDrawRect->bottom =
		(int)((float)(mIdealRect.top - inIdealRect->bottom) * ((float)(mDrawRect.rawHeight()) /
		(float)(mIdealRect.invHeight()))+0.5) + mDrawRect.top;
}


// ---------------------------------------------------------------------------------
//		 TransformVeLength
// ---------------------------------------------------------------------------------

void
oofTransformationManager::TransformVeLength( int idealLength, int &drawLength )
{
	assert(mIdealRect.invHeight()!=0);  //stop div by zero in transform
	drawLength = (int)((float)idealLength * ((float)(mDrawRect.rawHeight())/
		(float)(mIdealRect.invHeight()))+0.5);
}

void
oofTransformationManager::TransformVeLength( int idealLength, double &drawLength )
{
	assert(mIdealRect.invHeight()!=0);  //stop div by zero in transform
	
	drawLength = (double)idealLength * ((double)(mDrawRect.rawHeight())/
		(double)(mIdealRect.invHeight()));
}


// ---------------------------------------------------------------------------------
//		 TransformHoLength
// ---------------------------------------------------------------------------------

void
oofTransformationManager::TransformHoLength( int idealLength, int &drawLength )
{
	assert(mDrawRect.rawWidth()!=0);  //stop div by zero in transform
	drawLength = (int)((float)idealLength * 
				       ((float)(mDrawRect.rawWidth()) / (float)(mIdealRect.rawWidth()))+0.5);
}

void
oofTransformationManager::TransformHoLength( int idealLength, double &drawLength )
{
	assert((mDrawRect.rawWidth()!=0)&&(mDrawRect.rawHeight()!=0)); //stop div by zero in transforms
	drawLength = (double)idealLength * 
				 ((double)(mDrawRect.rawWidth()) / (double)(mIdealRect.rawWidth()));
}


// ---------------------------------------------------------------------------------
//		 TransformTextSize
// ---------------------------------------------------------------------------------

void
oofTransformationManager::TransformTextSize( int idealTextSize, int &drawTextSize )
{
	// Just do a straight mapping for now
	drawTextSize = idealTextSize;

/*	drawTextSize = (int)((float)idealTextSize * ((float)(mDrawRect.right - mDrawRect.left)/
		(float)(mIdealRect.right - mIdealRect.left))+0.5);*/

}

// ---------------------------------------------------------------------------------
//		describe
// ---------------------------------------------------------------------------------

void
oofTransformationManager::describe(ostream& os) const
{
    
     os << "-=-=-=-= Transform manager state -=-=-=-=" << endl
  
     << " mLeftMargin: " << mLeftMargin
    << " mRightMargin: " << mRightMargin
    << " mTopMargin: " << mTopMargin 
    << " mBottomMargin: " << mBottomMargin
    << " mXTickLabelHeight:" << mXTickLabelHeight
    << " mYTickLabelWidth:" << mYTickLabelWidth
   	<< " mIdealRect ";
   	mIdealRect.describe(os);
    os << " mPaneRect ";
   	mPaneRect.describe(os);
      os << " mDrawRect ";
   	mDrawRect.describe(os);
   	if(mDirty)
		os << "---dirty---" <<endl;
			else
		os << "---clean---" <<endl
		<< "-=-=-=-= Transform manager state -=-=-=-=" << endl << endl;
  

}


// FUNCTIONS BELOW YET TO BE IMPLEMENTED
void
oofTransformationManager::InverseTransformPoint(int drawX, int drawY,
								int &idealX, int &idealY)
{
	idealX = (int)((float)(drawX - mDrawRect.left)  * ((float)(mIdealRect.rawWidth()) /
		(float)(mDrawRect.rawWidth()))+0.5) + mIdealRect.left;
		
	idealY = (int)((float)(drawY - mDrawRect.top) * ((float)(mIdealRect.invHeight()) /
		(float)(mDrawRect.rawHeight()))+0.5) + mIdealRect.top;
}



void
oofTransformationManager::InverseTransformRect( const oofRect *inDrawRect,
								oofRect *outIdealRect )
{
	outIdealRect->left =
		(int)((float)(inDrawRect->left - mDrawRect.left)  * ((float)(mIdealRect.rawWidth()) /
		(float)(mDrawRect.rawWidth()))+0.5) + mIdealRect.left;
		
	outIdealRect->top =
		(int)((float)(mDrawRect.top - inDrawRect->top) * ((float)(mIdealRect.invHeight()) /
		(float)(mDrawRect.rawHeight()))+0.5) + mIdealRect.top;
		
	outIdealRect->right =
		(int)((float)(inDrawRect->right - mDrawRect.left)  * ((float)(mIdealRect.rawWidth()) /
		(float)(mDrawRect.rawWidth()))+0.5) + mIdealRect.left;
		
	outIdealRect->bottom =
		(int)((float)(mDrawRect.top - inDrawRect->bottom) * ((float)(mIdealRect.invHeight()) /
		(float)(mDrawRect.rawHeight()))+0.5) + mIdealRect.top;
}

void
oofTransformationManager::InverseTransformVeLength( int drawLength,
								int &idealLength )
{
	idealLength = (int)((float)drawLength * ((float)(mIdealRect.top - mIdealRect.bottom)/
		(float)(mDrawRect.rawHeight()))+0.5);
}

void
oofTransformationManager::InverseTransformHoLength( int drawLength,
								int &idealLength )
{
	idealLength = (int)((float)drawLength * ((float)(mIdealRect.rawWidth())/
		(float)(mDrawRect.rawWidth()))+0.5);
}
