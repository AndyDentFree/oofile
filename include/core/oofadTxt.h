#ifndef H_OOFADORNTXT
#define H_OOFADORNTXT

// COPYRIGHT 1997 A.D. Software, All rights reserved

// Text adorner layer of OOFILE database


#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderText
*/
class oofBorderAdorner_TextRender : public oofBorderAdorner_Render {
public:
	oofBorderAdorner_TextRender(oofBorderAdorner* b) : oofBorderAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderText
*/
class oofShadingAdorner_TextRender : public oofShadingAdorner_Render {
public:
	oofShadingAdorner_TextRender(oofShadingAdorner* b) : oofShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderText
*/
class oofBandShadingAdorner_TextRender : public oofBandShadingAdorner_Render {
public:
	oofBandShadingAdorner_TextRender(oofBandShadingAdorner* b) : oofBandShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

#endif
