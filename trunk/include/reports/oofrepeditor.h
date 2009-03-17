// COPYRIGHT 1999 A.D. Software, All rights reserved
// oofrepeditor.h
// report-writer layer of OOFILE database - editing classes

#ifndef H_OOFREPEDITOR
#define H_OOFREPEDITOR

// see also oofrepeditx.h & oofrepeditx.h

#ifndef H_OOFREP
	#include "oofrep.h"
#endif

// forward
class oofRepEditLink;
class OOF_InteractionPages;
class OOF_InteractionArea;
class oofRepViewRedirectingRender;
class oofRepEditingEnv;
class oofRepLayoutBand_Render;
class oofRepViewBand_Render;
class oofRepInteractorStrategy;
class oofRepLayoutEditLink;

/**
oofRepEditor encapsulates the factories for all the roles in editing reports and 
provides a single point of overriding from which user extensions may be invoked.

The make...RenderWrapper factories are invoked from overrides of
oofRepDrawable::makeRender(const oofRepEditingEnv&)

The make...EditLink factories are invoked from oofRepDrawable::makeEditLink 
and overrides in oofRepViewBand and oofRepLayoutBand. 

Thus user-defined drawables may provide their own render wrappers or 
edit link classes or rely on a suitable default.
*/
class OOFREP_EXPORT oofRepEditor {
public:
	oofRepEditor();
	virtual ~oofRepEditor();	
	virtual oofRepEditor* clone() const;
	
// overrideable factories
	virtual oofRepEditingEnv* makeEditingEnvironment();
	
	virtual oofRepRenderDecorator* makeDefaultRenderWrapper(oofRepRender* adoptedRender);	
	virtual oofRepRenderDecorator* makeDefaultBlockRenderWrapper(oofRepRender* adoptedRender);	
	virtual oofRepRenderDecorator* makeLayoutRenderWrapper(oofRepLayoutBand_Render* adoptedRender);
	virtual oofRepViewRedirectingRender* makeViewRenderWrapper(oofRepViewBand_Render* adoptedRender);

	virtual oofRepLayoutEditLink* makeLayoutEditLink(oofRepLayoutBand*);	
	virtual oofRepEditLink* makeViewEditLink(oofRepViewBand*, bool editingSubSelection);	
	virtual oofRepEditLink* makeColHeaderEditLink(oofRepViewBand*);	
	virtual oofRepVisitor* makeRepEditingSetupVisitor();	

	virtual void startEditing(OOF_InteractionArea*, oofRepEditingEnv*);	  // typically bring up link



// quick control over interaction response
	bool respondsToClicks() const;
	void enableInteraction();
	void disableInteraction();
	oofRepInteractorStrategy* defaultInteractor() const;

		
protected:
	bool mInteractionMappingActive;
// variables to describe interaction so can quickly query rather than using virtual
	bool mInteractsViaClicks, mInteractsViaKeys;	
	oofRepInteractorStrategy* mDefaultInteractor;  // owned, default strategy for Flyweight pattern
};


// -------------------------------------------------------
//     o o f R e p E d i t o r
// -------------------------------------------------------
inline bool 
oofRepEditor::respondsToClicks() const
{
	return mInteractionMappingActive && mInteractsViaClicks;
}


inline void 
oofRepEditor::enableInteraction()
{
	mInteractionMappingActive = true;
}


inline void 
oofRepEditor::disableInteraction()
{
	mInteractionMappingActive = false;
}


inline oofRepInteractorStrategy* 
oofRepEditor::defaultInteractor() const
{
	return mDefaultInteractor;
}

#endif  // H_OOFREPEDITOR


