// COPYRIGHT 1996 A.D. Software, All rights reserved

// report-writer layer of OOFILE database

#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOF3
	#include "oof3.h"
#endif
#ifndef H_OOFREP
	#include "oofrep.h"
#endif
#ifndef _STRING
	#include <string.h>
#endif
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef H_OOFIOS
	#include "oofios.h"
#endif
#ifndef H_OOFREPREND
	#include "oofreprn.h"	// abstract renderers
#endif
#ifndef H_OOFREPEDITOR
	#include "oofrepeditor.h"
#endif
#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

#ifdef OOF_READ_XML	
// 990529 HACK - this must precede the oofRepDefaultSettings ctor
OOF_Dictionary oofSerializableFactory::sFactories(true);  // owns objects
#endif

oofRepSettings* 	oofRepDefaultSettings::sDefaultSettings;
oofRepDefaultSettings		oofRepDefaultSettings::sDefaultSettingsFactory;	// the Singleton


// -------------------------------------------------------
//      o o f R e p D e f a u l t S e t t i n g s 
// -------------------------------------------------------
oofRepDefaultSettings::oofRepDefaultSettings()
{
	if (sDefaultSettings==0) {
		sDefaultSettings = new oofRepSettings;
		/* Note that at this point we must provide default values
		for any states which the oofRepSettings object tracks, or else
		we get serious problems. Beware.
		*/
// all figures in points unless otherwise noted	
		sDefaultSettings->lineThickness(2);
		sDefaultSettings->pageHeight(792); 	// GUI calculates from PREC, default to 11" for RTF
		sDefaultSettings->pageWidth(576); 	// GUI calculates from PREC, default to 8" for RTF
		sDefaultSettings->borderType(0);
		sDefaultSettings->bandIndenting(0);
		sDefaultSettings->leftMargin(72);
		sDefaultSettings->rightMargin(72);
		sDefaultSettings->topMargin(36);
		sDefaultSettings->bottomMargin(36);
		sDefaultSettings->colSepWidth(20);
		sDefaultSettings->blockVertSep(20);
		sDefaultSettings->defaultTextStyle(new oofRepTextStyle("Helvetica", 10));
		sDefaultSettings->defaultPageHeaders(new oofRepBandList);
		sDefaultSettings->defaultPageFooters(new oofRepBandList);
		sDefaultSettings->drawHeadingsDownToLevel(1);
		sDefaultSettings->printGraphsInMono(true);
		sDefaultSettings->modalPreviewWindow(false);
		sDefaultSettings->graphBandMinSizePercent(50);  // percentage
		sDefaultSettings->XMLsettings(new oofRepXMLsettings);
		// sDefaultSettings->reportEditor(new oofRepEditor);  // left at null - reports not editable by default
		
// other init stuff - register default serializable classes
		oofShadingAdorner::registerSerializable();
		oofBandShadingAdorner::registerSerializable();
		oofBorderAdorner::registerSerializable();		
	}
}


oofRepDefaultSettings::~oofRepDefaultSettings()
{
	if (--sDefaultSettings->refcount()==0)
		delete sDefaultSettings;
}


// -------------------------------------------------------
//      o o f R e p S e t t i n g s 
// -------------------------------------------------------
oofRepSettings::oofRepSettings() : 
	mRefCount(1),
	mLineThickness(0),
	mPageHeight(0),
	mPageWidth(0),
	mBorderType(0),
	mBandIndenting(0),
	mLeftMargin(0),
	mRightMargin(0),
	mTopMargin(0),
	mBottomMargin(0),
	mColSepWidth(0),
	mBlockVertSep(0),
	mDefaultTextStyle(0),
	mDefaultPageHeaders(0),
	mDefaultPageFooters(0),
	mDrawHeadingsDownToLevel(0),
	mPrintGraphsInMono(0),
	mModalPreviewWindow(0),
	mGraphBandMinSizePercent(0),
	mXMLsettings(0),
	mRepEditor(0)
{
// zero all the pointers to settings above
}


oofRepSettings::oofRepSettings(oofRepSettings* rhs) :
	mRefCount(1),
	mLineThickness(0),
	mPageHeight(0),
	mPageWidth(0),
	mBorderType(0),
	mBandIndenting(0),
	mLeftMargin(0),
	mRightMargin(0),
	mTopMargin(0),
	mBottomMargin(0),
	mColSepWidth(0),
	mBlockVertSep(0),
	mDefaultTextStyle(0),
	mDefaultPageHeaders(0),
	mDefaultPageFooters(0),
	mDrawHeadingsDownToLevel(0),
	mPrintGraphsInMono(0),
	mModalPreviewWindow(0),
	mGraphBandMinSizePercent(0),
	mXMLsettings(0),
	mRepEditor(0)
{
	if (rhs->mLineThickness)
		lineThickness(*rhs->mLineThickness);
	
	if (rhs->mPageHeight)
		pageHeight(*rhs->mPageHeight);
	
	if (rhs->mPageWidth)
		pageWidth(*rhs->mPageWidth);
	
	if (rhs->mBorderType)
		borderType(*rhs->mBorderType);
	
	if (rhs->mBandIndenting)
		bandIndenting(*rhs->mBandIndenting);
	
	if (rhs->mLeftMargin)
		leftMargin(*rhs->mLeftMargin);
	
	if (rhs->mRightMargin)
		rightMargin(*rhs->mRightMargin);
	
	if (rhs->mTopMargin)
		topMargin(*rhs->mTopMargin);
	
	if (rhs->mBottomMargin)
		bottomMargin(*rhs->mBottomMargin);
	
	if (rhs->mColSepWidth)
		colSepWidth(*rhs->mColSepWidth);
	
	if (rhs->mBlockVertSep)
		blockVertSep(*rhs->mBlockVertSep);
	
	if (rhs->mDefaultTextStyle)
		defaultTextStyle(rhs->mDefaultTextStyle->clone());
	
	if (rhs->mDefaultPageHeaders)
		defaultPageHeaders(rhs->mDefaultPageHeaders->clone());
	
	if (rhs->mDefaultPageFooters)
		defaultPageFooters(rhs->mDefaultPageFooters->clone());
	
	if (rhs->mDrawHeadingsDownToLevel)
		drawHeadingsDownToLevel(*rhs->mDrawHeadingsDownToLevel);
		
	if (rhs->mPrintGraphsInMono)
		printGraphsInMono(*rhs->mPrintGraphsInMono);
		
	if (rhs->mModalPreviewWindow)
		modalPreviewWindow(*rhs->mModalPreviewWindow);
		
	if (rhs->mGraphBandMinSizePercent)
		graphBandMinSizePercent(*rhs->mGraphBandMinSizePercent);	
		
	if (rhs->mXMLsettings)
		XMLsettings(rhs->mXMLsettings->clone());	
#ifndef OOF_REP_NO_GUI
		
	if (rhs->mRepEditor)
		reportEditor(rhs->mRepEditor->clone());	
#endif
}


oofRepSettings::~oofRepSettings()
{
	if (mRefCount <= 1) {
		delete mLineThickness;
		delete mPageHeight;
		delete mPageWidth;
		delete mBorderType;
		delete mBandIndenting;
		delete mLeftMargin;
		delete mRightMargin;
		delete mTopMargin;
		delete mBottomMargin;
		delete mColSepWidth;
		delete mBlockVertSep;
		delete mDefaultTextStyle;
		delete mDefaultPageHeaders;
		delete mDefaultPageFooters;
		delete mDrawHeadingsDownToLevel;
		delete mPrintGraphsInMono;
		delete mModalPreviewWindow;
		delete mGraphBandMinSizePercent;
		delete mXMLsettings;
		delete mRepEditor;
	}
	else
		--mRefCount;
}


unsigned short&
oofRepSettings::refcount()
{
	return mRefCount;
}


unsigned short	
oofRepSettings::lineThickness() const
{
	if (mLineThickness)
		return *mLineThickness;
	else
		return oofRepDefaultSettings::settings()->lineThickness();
}


void			
oofRepSettings::lineThickness(unsigned short n)
{
	if (mLineThickness)
		*mLineThickness = n;
	else
		mLineThickness = new unsigned short(n);
}

unsigned short	
oofRepSettings::pageHeight() const
{
	if (mPageHeight)
		return *mPageHeight;
	else
		return oofRepDefaultSettings::settings()->pageHeight();
}


void			
oofRepSettings::pageHeight(unsigned short n)
{
	if (mPageHeight)
		*mPageHeight = n;
	else
		mPageHeight = new unsigned short(n);
}

unsigned short	
oofRepSettings::pageWidth() const
{
	if (mPageWidth)
		return *mPageWidth;
	else
		return oofRepDefaultSettings::settings()->pageWidth();
}


void			
oofRepSettings::pageWidth(unsigned short n)
{
	if (mPageWidth)
		*mPageWidth = n;
	else
		mPageWidth = new unsigned short(n);
}

unsigned short
oofRepSettings::borderType() const
{
	if (mBorderType)
		return *mBorderType;
	else
		return oofRepDefaultSettings::settings()->borderType();
}

void
oofRepSettings::borderType(unsigned short n)
{
	if (mBorderType)
		*mBorderType = n;
	else
		mBorderType = new unsigned short(n);
}

unsigned short
oofRepSettings::bandIndenting() const
{
	if (mBandIndenting)
		return *mBandIndenting;
	else
		return oofRepDefaultSettings::settings()->bandIndenting();
}

void
oofRepSettings::bandIndenting(unsigned short n)
{
	if (mBandIndenting)
		*mBandIndenting = n;
	else
		mBandIndenting = new unsigned short(n);
}

unsigned short	
oofRepSettings::leftMargin() const
{
	if (mLeftMargin)
		return *mLeftMargin;
	else
		return oofRepDefaultSettings::settings()->leftMargin();
}


void			
oofRepSettings::leftMargin(unsigned short n)
{
	if (mLeftMargin)
		*mLeftMargin = n;
	else
		mLeftMargin = new unsigned short(n);
}

unsigned short	
oofRepSettings::rightMargin() const
{
	if (mRightMargin)
		return *mRightMargin;
	else
		return oofRepDefaultSettings::settings()->rightMargin();
}


void			
oofRepSettings::rightMargin(unsigned short n)
{
	if (mRightMargin)
		*mRightMargin = n;
	else
		mRightMargin = new unsigned short(n);
}

unsigned short	
oofRepSettings::topMargin() const
{
	if (mTopMargin)
		return *mTopMargin;
	else
		return oofRepDefaultSettings::settings()->topMargin();
}


void			
oofRepSettings::topMargin(unsigned short n)
{
	if (mTopMargin)
		*mTopMargin = n;
	else
		mTopMargin = new unsigned short(n);
}

unsigned short	
oofRepSettings::bottomMargin() const
{
	if (mBottomMargin)
		return *mBottomMargin;
	else
		return oofRepDefaultSettings::settings()->bottomMargin();
}


void			
oofRepSettings::bottomMargin(unsigned short n)
{
	if (mBottomMargin)
		*mBottomMargin = n;
	else
		mBottomMargin = new unsigned short(n);
}


unsigned short	
oofRepSettings::colSepWidth() const
{
	if (mColSepWidth)
		return *mColSepWidth;
	else
		return oofRepDefaultSettings::settings()->colSepWidth();
}


void			
oofRepSettings::colSepWidth(unsigned short n)
{
	if (mColSepWidth)
		*mColSepWidth = n;
	else
		mColSepWidth = new unsigned short(n);
}

oofRepBandList&	
oofRepSettings::defaultPageHeaders() const
{
	if (mDefaultPageHeaders)
		return *mDefaultPageHeaders;
	else
		return oofRepDefaultSettings::settings()->defaultPageHeaders();
}


void			
oofRepSettings::defaultPageHeaders(oofRepBandList*adoptedList)
{
	delete mDefaultPageHeaders;
	mDefaultPageHeaders = adoptedList;
}


oofRepBandList&	
oofRepSettings::defaultPageFooters() const
{
	if (mDefaultPageFooters)
		return *mDefaultPageFooters;
	else
		return oofRepDefaultSettings::settings()->defaultPageFooters();
}


void			
oofRepSettings::defaultPageFooters(oofRepBandList* adoptedList)
{
	delete mDefaultPageFooters;
	mDefaultPageFooters = adoptedList;
}


unsigned short	
oofRepSettings::blockVertSep() const
{
	if (mBlockVertSep)
		return *mBlockVertSep;
	else
		return oofRepDefaultSettings::settings()->blockVertSep();
}


void			
oofRepSettings::blockVertSep(unsigned short n)
{
	if (mBlockVertSep)
		*mBlockVertSep = n;
	else
		mBlockVertSep = new unsigned short(n);
}



oofRepTextStyle*	
oofRepSettings::defaultTextStyle() const
{
	if (mDefaultTextStyle)
		return mDefaultTextStyle;
	else
		return oofRepDefaultSettings::settings()->defaultTextStyle();
}


void			
oofRepSettings::defaultTextStyle(oofRepTextStyle* adoptedStyle)
{
	delete mDefaultTextStyle;
	mDefaultTextStyle = adoptedStyle;
}


unsigned short	
oofRepSettings::drawHeadingsDownToLevel() const
{
	if (mDrawHeadingsDownToLevel)
		return *mDrawHeadingsDownToLevel;
	else
		return oofRepDefaultSettings::settings()->drawHeadingsDownToLevel();
}


void			
oofRepSettings::drawHeadingsDownToLevel(unsigned short n)
{
	if (mDrawHeadingsDownToLevel)
		*mDrawHeadingsDownToLevel = n;
	else
		mDrawHeadingsDownToLevel = new unsigned short(n);
}


bool			
oofRepSettings::printGraphsInMono() const
{
	if (mPrintGraphsInMono)
		return *mPrintGraphsInMono;
	else
		return oofRepDefaultSettings::settings()->printGraphsInMono();
}


void			
oofRepSettings::printGraphsInMono(bool b)
{
	if (mPrintGraphsInMono)
		*mPrintGraphsInMono = b;
	else
		mPrintGraphsInMono = new bool(b);
}


bool			
oofRepSettings::modalPreviewWindow() const
{
	if (mModalPreviewWindow)
		return *mModalPreviewWindow;
	else
		return oofRepDefaultSettings::settings()->modalPreviewWindow();
}


void			
oofRepSettings::modalPreviewWindow(bool b)
{
	if (mModalPreviewWindow)
		*mModalPreviewWindow = b;
	else
		mModalPreviewWindow = new bool(b);
}


unsigned short	
oofRepSettings::graphBandMinSizePercent() const
{
	if (mGraphBandMinSizePercent)
		return *mGraphBandMinSizePercent;
	else
		return oofRepDefaultSettings::settings()->graphBandMinSizePercent();
}


void			
oofRepSettings::graphBandMinSizePercent(unsigned short n)
{
	if (mGraphBandMinSizePercent)
		*mGraphBandMinSizePercent = n;
	else
		mGraphBandMinSizePercent = new unsigned short(n);
}


oofRepXMLsettings*	
oofRepSettings::XMLsettings() const
{
	if (mXMLsettings)
		return mXMLsettings;
	else
		return oofRepDefaultSettings::settings()->XMLsettings();
}


void			
oofRepSettings::XMLsettings(oofRepXMLsettings* adoptedSettings)
{
	delete mXMLsettings;
	mXMLsettings = adoptedSettings;
}


void
oofRepSettings::describe(ostream& os) {
os << "----------ofrepsettings state-----------" << endl;

	os << "lineThickness: " <<	lineThickness() << endl
	<< "pageHeight: " <<	pageHeight() << endl
	<< "pageWidth: " <<	pageWidth() << endl
	<< "borderType: " <<	borderType() << endl
	<< "bandIndenting: " <<	bandIndenting() << endl
	<< "leftMargin: " <<	leftMargin() << endl
	<< "rightMargin: " <<	rightMargin() << endl
	<< "topMargin: " <<	topMargin() << endl
	<< "bottomMargin: " <<	bottomMargin() << endl
	<< "colSepWidth: " <<	colSepWidth() << endl
	<< "blockVertSep: " <<	blockVertSep() << endl;
	os << "defaultTextStyle: "; defaultTextStyle()->extract(os);
	//os << "defaultPageHeaders: "; defaultPageHeaders()->describe(os);
	//os << "defaultPageFooters: ";defaultPageFooters()->describe(os);
	os << "drawHeadingsDownToLevel" << drawHeadingsDownToLevel() << endl
	<< "printGraphsInMono";( printGraphsInMono()? (os << "true"):(os << "false"));
	os << endl
	<< "modalPreviewWindow: ";
	(modalPreviewWindow() ? os << "true":os << "false");
	os << endl 
	<< "graphBandMinSizePercent: " <<	graphBandMinSizePercent() << endl; 
	
	
	
	os << "----------End ofrepsettings state-----------" << endl;

}	


oofRepEditor*	
oofRepSettings::reportEditor() const
{
	if (mRepEditor)
		return mRepEditor;
	else {
		oofRepSettings* defSettings = oofRepDefaultSettings::settings();
		if (this==defSettings)
			return NULL;  // cope with default being left at NULL!
		else
			return defSettings->reportEditor();  // may still be null
	}
}


void			
oofRepSettings::reportEditor(oofRepEditor* adoptedEditor)
{
	delete mRepEditor;
	mRepEditor = adoptedEditor;
}
