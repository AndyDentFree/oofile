// ===========================================================================
//	oofadorn.cpp 			©1996 A.D. Software   All rights reserved.
// ===========================================================================
//

// 990531 AD disclaimer
// the Adorner mechanism was implemented with only minimal design guidance from
// me by 3rd party under contractual circumstances which forced me to accept the code
// It may violate our programming guidelines and I am NOT happy with the design so
// expect it to change in future, although external API changes will be minimal.


#include "oofpch_r.h"  // for precompilation of report files

#ifndef H_OOFADORN
	#include "oofadorn.h"
#endif

#ifndef H_OOFREP
	#include "oofrep.h"
#endif


// -------------------------------------------------------
//             o o f A d o r n e r R e n d e r
// -------------------------------------------------------
void
oofAdornerRender::drawcell( int /*x*/, int /*y*/, oofRepViewBand* drawable, unsigned long height, unsigned long width, oofAdorner::AdornerCellCode /*overrunCode*/ )
{
	// Default behaviour is to just call the standard drawing code (for cases where no custom code is needed for tables)
	draw( drawable, height, width );
}

// -------------------------------------------------------
//             o o f A d o r n e r L i s t
// -------------------------------------------------------
oofAdornerList::oofAdornerList()
{
}


oofAdornerList::oofAdornerList(const oofAdornerList& rhs)
{
	*this = rhs;
}
	
	
const oofAdornerList& 
oofAdornerList::operator=(const oofAdornerList& rhs)
{
	const unsigned long numItems = rhs.count();
	for (unsigned long i=0; i<numItems; i++) {
		oofAdorner* theAdorn = rhs.value(i);
		append(theAdorn->clone());
	}
	return *this;
}


void
oofAdornerList::adopt(oofAdornerList& rhs)
{
	const unsigned long numItems = rhs.count();
	for (unsigned long i=0; i<numItems; i++) {
		oofAdorner* adoptedAdorn = rhs.value(i);
		rhs.mAdorners[i] = 0;
		append(adoptedAdorn);
	}
}


oofAdornerList::~oofAdornerList()
{
	for (start(); more(); next()) {
		oofAdorner* theAdorn = current();
		delete theAdorn;
	}
}


oofAdornerList* 
oofAdornerList::clone()
{
	return new oofAdornerList(*this);
}



	
void 
oofAdornerList::append(oofAdorner* adoptedAdorner)
{
	mAdorners.append((unsigned long) adoptedAdorner);
}


// -------------------------------------------------------
//            O O F _ a d o r n e r H o l d e r
// -------------------------------------------------------
OOF_adornerHolder::OOF_adornerHolder(const oofString& id, oofAdorner*  firstAdorner) :
	mID(id)
{
	if (firstAdorner)
		mAdorners << firstAdorner;
}


oofString
OOF_adornerHolder::name() const
{
	return mID;
}

// -------------------------------------------------------
//             o o f A d o r n e r 
// -------------------------------------------------------
oofAdorner::oofAdorner() :
	mRender(0)
{
}

oofAdorner::oofAdorner(const oofAdorner& rhs)
{
	if (rhs.mRender)
		mRender = rhs.mRender->clone(this);
	else
		mRender = 0;
}


oofAdorner::~oofAdorner()
{
	delete mRender;
}

void
oofAdorner::draw( oofRepDrawable *drawable, unsigned long height, unsigned long width )
{
// central default call for drawing, but still able to be overriden in extreme
// cases. Now that all is factored out to the render objects (Strategy pattern)
// we shouldn't need to do anything.
	assert(mRender);
	mRender->draw( drawable, height, width );
}

void
oofAdorner::drawcell( int x, int y, oofRepViewBand *drawable, unsigned long height, unsigned long width, AdornerCellCode overrunCode )
{
	assert(mRender);
	mRender->drawcell( x, y, drawable, height, width, overrunCode );
}

// default render factories so if user chooses not to implement
// a render for their report object, we still behave OK, just ignore them

void	
oofAdorner::makeRender(const oofRepMacEnv&)
{
	delete mRender;
	mRender = 0;
}

void	
oofAdorner::makeRender(const oofRepWinEnv&)
{
	delete mRender;
	mRender = 0;
}

void	
oofAdorner::makeRender(const oofRepRTFEnv&)
{
	delete mRender;
	mRender = 0;
}

void	
oofAdorner::makeRender(const oofRepHTMLEnv&)
{
	delete mRender;
	mRender = 0;
}

void	
oofAdorner::makeRender(const oofRepCSVEnv&)
{
	delete mRender;
	mRender = 0;
}

void	
oofAdorner::makeRender(const oofRepTSVEnv&)
{
	delete mRender;
	mRender = 0;
}

void	
oofAdorner::makeRender(const oofRepTextEnv&)
{
	delete mRender;
	mRender = 0;
}

void	//dg 22/2/99
oofAdorner::makeRender(const oofRepXMLEnv&)
{
	delete mRender;
	mRender = 0;
}

// -------------------------------------------------------
//             o o f B o r d e r A d o r n e r
// -------------------------------------------------------
oofBorderAdorner::oofBorderAdorner(BorderCode borderType, oofAdorner::AdornerContextCode whenCode ) :
	mBorderType( borderType ),
	mLeftStyle( borderType ),
	mRightStyle( borderType ),
	mTopStyle( borderType ),
	mBottomStyle( borderType )
{
	mWhenCode = whenCode;
}

oofBorderAdorner::oofBorderAdorner(const oofBorderAdorner& rhs) :
	oofAdorner( rhs )
{
	mWhenCode = rhs.mWhenCode;
	mBorderType = rhs.mBorderType;
	
	mLeftStyle = rhs.mLeftStyle;
	mRightStyle = rhs.mRightStyle;
	mTopStyle = rhs.mTopStyle;
	mBottomStyle = rhs.mBottomStyle;
}

oofAdorner*
oofBorderAdorner::clone() const
{
	return new oofBorderAdorner(*this);
}


oofString
oofBorderAdorner::signature() const
{
	return "oofBorderAdorner";
}


void 
oofBorderAdorner::registerSerializable()
{
#ifdef OOF_READ_XML	
	oofSerializableFactory::registerDefaultMake("oofBorderAdorner", oofBorderAdorner::makeAdorner);
#endif
}


const char*  kBorderAdorner_serializationMask = "%d %d";

oofSerializable* 
oofBorderAdorner::makeAdorner (const oofString& inData)
{
	int iTypeCode, iWhenCode;
	sscanf(inData.chars(), kBorderAdorner_serializationMask, &iTypeCode, &iWhenCode);
	const BorderCode typeCode = (BorderCode)iTypeCode;
	const AdornerContextCode whenCode = (AdornerContextCode)iWhenCode;
	return new oofBorderAdorner(typeCode, whenCode);
}


oofString 
oofBorderAdorner::serialize() const
{
	char buf[32];
	const int iBorderType = (int)mBorderType;
	const int iWhenCode = (int)mWhenCode;
	sprintf(buf, kBorderAdorner_serializationMask, iBorderType, iWhenCode);
	return oofString(buf);
}


// -------------------------------------------------------
//             o o f S h a d i n g A d o r n e r
// -------------------------------------------------------
oofShadingAdorner::oofShadingAdorner( unsigned short percentage, oofAdorner::AdornerContextCode whenCode ) :
	mRed( percentage ),
	mGreen( percentage ),
	mBlue( percentage )
{
	mWhenCode = whenCode;
}

oofShadingAdorner::oofShadingAdorner(const oofShadingAdorner& rhs) :
	oofAdorner( rhs )		// MUST remember to call the base class to get the mRender cloned as well
{
	mWhenCode = rhs.mWhenCode;
	mRed = rhs.mRed;
	mGreen = rhs.mGreen;
	mBlue = rhs.mBlue;
}


oofAdorner*
oofShadingAdorner::clone() const
{
	return new oofShadingAdorner(*this);
}


oofString
oofShadingAdorner::signature() const
{
	return "oofShadingAdorner";
}


void 
oofShadingAdorner::registerSerializable()
{
#ifdef OOF_READ_XML	
	oofSerializableFactory::registerDefaultMake("oofShadingAdorner", oofShadingAdorner::makeAdorner);
#endif
}


const char*  kShadingAdorner_serializationMask = "%hu %d";

oofSerializable* 
oofShadingAdorner::makeAdorner (const oofString& inData)
{
	unsigned short percentage;
	int iWhenCode;
	sscanf(inData.chars(), kShadingAdorner_serializationMask, &percentage, &iWhenCode);
	const AdornerContextCode whenCode = (AdornerContextCode)iWhenCode;
	return new oofShadingAdorner(percentage, whenCode);
}


oofString 
oofShadingAdorner::serialize() const
{

	char buf[32];
	const int iWhenCode = (int)mWhenCode;
	sprintf(buf, kShadingAdorner_serializationMask, mRed, iWhenCode);
	// just use one value as the percentage - see the ctor as they are all set the same
	return oofString(buf);
}


// -------------------------------------------------------
//             o o f B a n d S h a d i n g A d o r n e r
// -------------------------------------------------------
oofBandShadingAdorner::oofBandShadingAdorner( unsigned short percentage, oofAdorner::AdornerContextCode whenCode ) :
	mPercentage( percentage )
{
	mWhenCode = whenCode;
}

oofBandShadingAdorner::oofBandShadingAdorner(const oofBandShadingAdorner& rhs) :
	oofAdorner( rhs )		// MUST remember to call the base class to get the mRender cloned as well
{
	mWhenCode = rhs.mWhenCode;
	mPercentage = rhs.mPercentage;
}

oofAdorner*
oofBandShadingAdorner::clone() const
{
	return new oofBandShadingAdorner(*this);
}


oofString
oofBandShadingAdorner::signature() const
{
	return "oofBandShadingAdorner";
}


void 
oofBandShadingAdorner::registerSerializable()
{
#ifdef OOF_READ_XML	
	oofSerializableFactory::registerDefaultMake("oofBandShadingAdorner", oofBandShadingAdorner::makeAdorner);
#endif
}


const char*  kBandShadingAdorner_serializationMask = "%hu %d";

oofSerializable* 
oofBandShadingAdorner::makeAdorner (const oofString& inData)
{
	unsigned short percentage;
	int iWhenCode;
	sscanf(inData.chars(), kBandShadingAdorner_serializationMask, &percentage, &iWhenCode);
	const AdornerContextCode whenCode = (AdornerContextCode)iWhenCode;
	return new oofBandShadingAdorner(percentage, whenCode);
}


oofString 
oofBandShadingAdorner::serialize() const
{
	char buf[32];
	const int iWhenCode = (int)mWhenCode;
	sprintf(buf, kBandShadingAdorner_serializationMask, mPercentage, iWhenCode);
	return oofString(buf);
}
