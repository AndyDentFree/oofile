#ifndef H_OOFADORNMAC
#define H_OOFADORNMAC

// COPYRIGHT 1997 A.D. Software, All rights reserved

// Mac adorner layer of OOFILE database


#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	Draw cell outline Mac output as specified by oofBorderAdorner.
	\ingroup oofRenderMac
*/
class oofBorderAdorner_MacRender : public oofBorderAdorner_Render {
public:
	oofBorderAdorner_MacRender(oofBorderAdorner* b) : oofBorderAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
	virtual void drawcell( int x, int y, /*const*/ oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode );

	// Individual drawing for each edge
	void drawTo( oofBorderAdorner::BorderCode type, long distHoriz, long distVert );
};

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	Draw cell background on Mac output as specified by oofShadingAdorner.
	\ingroup oofRenderMac
*/
class oofShadingAdorner_MacRender : public oofShadingAdorner_Render {
public:
	oofShadingAdorner_MacRender(oofShadingAdorner* b) : oofShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
	virtual void drawcell( int x, int y, /*const*/ oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode );
};

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	Draw band background on Mac output as specified by oofBandShadingAdorner.
	\ingroup oofRenderMac
*/
class oofBandShadingAdorner_MacRender : public oofBandShadingAdorner_Render {
public:
	oofBandShadingAdorner_MacRender(oofBandShadingAdorner* b) : oofBandShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
	virtual void drawcell( int x, int y, /*const*/ oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode );
};

#endif

