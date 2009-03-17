#ifndef H_OOFADORNXML
#define H_OOFADORNXML

// COPYRIGHT 1997 A.D. Software, All rights reserved

// XML adorner layer of OOFILE database


#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	No output renderer.
	\ingroup oofRender
*/
class oofBorderAdorner_XMLRender : public oofBorderAdorner_Render {
public:
	oofBorderAdorner_XMLRender(oofBorderAdorner* b) : oofBorderAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderXML
*/
class oofShadingAdorner_XMLRender : public oofShadingAdorner_Render {
public:
	oofShadingAdorner_XMLRender(oofShadingAdorner* b) : oofShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderXML
*/
class oofBandShadingAdorner_XMLRender : public oofBandShadingAdorner_Render {
public:
	oofBandShadingAdorner_XMLRender(oofBandShadingAdorner* b) : oofBandShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

#endif

