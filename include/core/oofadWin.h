#ifndef H_OOFADORNWIN
#define H_OOFADORNWIN

// COPYRIGHT 1997 A.D. Software, All rights reserved

// Win adorner layer of OOFILE database


#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

#ifndef _INC_WINDOWSX
	#include "windowsx.h"		// for DeleteBrush macro
#endif

// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	Draw cell outline MS Windows output as specified by oofBorderAdorner.
	\ingroup oofRenderWin
*/
class oofBorderAdorner_WinRender : public oofBorderAdorner_Render {
public:
	oofBorderAdorner_WinRender(oofBorderAdorner* b) : oofBorderAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
	virtual void drawcell( int x, int y, /*const*/ oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode );
};

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	Draw cell background on MS Windows output as specified by oofShadingAdorner.
	\ingroup oofRenderWin
*/
class oofShadingAdorner_WinRender : public oofShadingAdorner_Render {
public:
	oofShadingAdorner_WinRender(oofShadingAdorner* b) : oofShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
	virtual void drawcell( int x, int y, /*const*/ oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode );
};

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	Draw band background on MS Windows output as specified by oofBandShadingAdorner.
	\ingroup oofRenderWin
*/
class oofBandShadingAdorner_WinRender : public oofBandShadingAdorner_Render {
public:
	oofBandShadingAdorner_WinRender(oofBandShadingAdorner* b) : oofBandShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
	virtual void drawcell( int x, int y, /*const*/ oofRepViewBand *drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode overrunCode );
};

#endif
