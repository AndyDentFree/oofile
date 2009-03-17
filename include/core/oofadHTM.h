#ifndef H_OOFADORNHTM
#define H_OOFADORNHTM

// COPYRIGHT 1997 A.D. Software, All rights reserved

// HTML adorner layer of OOFILE database


#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderHTML
*/
class oofBorderAdorner_HTMLRender : public oofBorderAdorner_Render {
public:
	oofBorderAdorner_HTMLRender(oofBorderAdorner* b) : oofBorderAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderHTML
*/
class oofShadingAdorner_HTMLRender : public oofShadingAdorner_Render {
public:
	oofShadingAdorner_HTMLRender(oofShadingAdorner* b) : oofShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderHTML
*/
class oofBandShadingAdorner_HTMLRender : public oofBandShadingAdorner_Render {
public:
	oofBandShadingAdorner_HTMLRender(oofBandShadingAdorner* b) : oofBandShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

#endif

