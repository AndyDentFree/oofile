#ifndef H_OOFADORNCSV
#define H_OOFADORNCSV

// COPYRIGHT 1997 A.D. Software, All rights reserved

/**
	\file oofadCSV.h 
	CSV adorner layer of OOFILE database.
*/

#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

// ----------------------------------------------------------------
//   B o r d e r   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderCSV
*/
class oofBorderAdorner_CSVRender : public oofBorderAdorner_Render {
public:
	oofBorderAdorner_CSVRender(oofBorderAdorner* b) : oofBorderAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderCSV
*/
class oofShadingAdorner_CSVRender : public oofShadingAdorner_Render {
public:
	oofShadingAdorner_CSVRender(oofShadingAdorner* b) : oofShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

// ----------------------------------------------------------------
//   B a n d   S h a d i n g   A d o r n e r

/**
	No output renderer.
	\ingroup oofRenderCSV
*/
class oofBandShadingAdorner_CSVRender : public oofBandShadingAdorner_Render {
public:
	oofBandShadingAdorner_CSVRender(oofBandShadingAdorner* b) : oofBandShadingAdorner_Render(b) {};
	virtual oofAdornerRender* clone(const oofAdorner* newOwner) const;
	
	virtual void draw( /*const*/ oofRepDrawable *drawable, unsigned long height, unsigned long width );
};

#endif

