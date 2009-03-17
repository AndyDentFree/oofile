#ifndef H_OOFADORNTSV
#define H_OOFADORNTSV

// COPYRIGHT 1997 A.D. Software, All rights reserved

// TSV adorner layer of OOFILE database


#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderTSV
*/
class oofBorderAdorner_TSVRender : public oofBorderAdorner_Render {
public:
	oofBorderAdorner_TSVRender(oofBorderAdorner* b) : oofBorderAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderTSV
*/
class oofShadingAdorner_TSVRender : public oofShadingAdorner_Render {
public:
	oofShadingAdorner_TSVRender(oofShadingAdorner* b) : oofShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderTSV
*/
class oofBandShadingAdorner_TSVRender : public oofBandShadingAdorner_Render {
public:
	oofBandShadingAdorner_TSVRender(oofBandShadingAdorner* b) : oofBandShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

#endif

