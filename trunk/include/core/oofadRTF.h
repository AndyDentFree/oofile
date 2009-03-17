#ifndef H_OOFADORNRTF
#define H_OOFADORNRTF

// COPYRIGHT 1997 A.D. Software, All rights reserved

// RTF adorner layer of OOFILE database


#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderRTF
*/
class oofBorderAdorner_RTFRender : public oofBorderAdorner_Render {
public:
	oofBorderAdorner_RTFRender(oofBorderAdorner* b) : oofBorderAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderHTML
*/
class oofShadingAdorner_RTFRender : public oofShadingAdorner_Render {
public:
	oofShadingAdorner_RTFRender(oofShadingAdorner* b) : oofShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderRTF
*/
class oofBandShadingAdorner_RTFRender : public oofBandShadingAdorner_Render {
public:
	oofBandShadingAdorner_RTFRender(oofBandShadingAdorner* b) : oofBandShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

#endif

