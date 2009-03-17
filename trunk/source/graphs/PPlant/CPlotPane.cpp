// =================================================================================
//	CPlotPane.cp
// =================================================================================

#include <UDrawingState.h>
#include "CPlotPane.h"
#ifndef H_OOFGRPHS
	#include "oofGrphs.h"
#endif
#ifndef _H_LStream
	#include <LStream.h>
#endif
#ifndef _H_UTextTraits
	#include <UTextTraits.h>
#endif
#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

// ---------------------------------------------------------------------------------
//		 CreatePlotPaneStream [static]
// ---------------------------------------------------------------------------------

CPlotPane *
CPlotPane::CreatePlotPaneStream(
	LStream	*inStream )
{
	return new CPlotPane( inStream );
}


// ---------------------------------------------------------------------------------
//		 CPlotPane
// ---------------------------------------------------------------------------------

CPlotPane::CPlotPane()
	:mGraphPtr(0),
	mTxtrID(0)
{
}


// ---------------------------------------------------------------------------------
//		 CPlotPane(SPaneInfo&)
// ---------------------------------------------------------------------------------

CPlotPane::CPlotPane(
	const SPaneInfo	&inPaneInfo,
	ResIDT			inTextTraitsID )
		: LPane( inPaneInfo ),
			mTxtrID(inTextTraitsID)
{
}


// ---------------------------------------------------------------------------------
//		 CPlotPane(const CPlotPane&)
// ---------------------------------------------------------------------------------

CPlotPane::CPlotPane(
	const CPlotPane	&inOriginal )
		: LPane( inOriginal ),
		mTxtrID(inOriginal.mTxtrID)
		
{
}


// ---------------------------------------------------------------------------------
//		 CPlotPane(LStream*)
// ---------------------------------------------------------------------------------

CPlotPane::CPlotPane(
	LStream	*inStream )
		: LPane( inStream )
{
	inStream->ReadData(&mTxtrID, sizeof(ResIDT));
}


// ---------------------------------------------------------------------------------
//		 ~CPlotPane
// ---------------------------------------------------------------------------------

CPlotPane::~CPlotPane()
{
}


// ---------------------------------------------------------------------------------
//		 SetGraph
// ---------------------------------------------------------------------------------

void
CPlotPane::SetGraph(oofGraph* theGraph)
{
	mGraphPtr=theGraph;
}


// ---------------------------------------------------------------------------------
//		 DrawSelf
// ---------------------------------------------------------------------------------

void
CPlotPane::DrawSelf()
{
	SetPortFont();

	if(mGraphPtr){
		Rect theFrame;
		CalcLocalFrameRect( theFrame );
		
		mGraphPtr->setPaneRect(theFrame);
		mGraphPtr->draw();
	}
}


// ---------------------------------------------------------------------------------
//		 SetPortFont
// ---------------------------------------------------------------------------------

void
CPlotPane::SetPortFont()
{
	if (mTxtrID)
		SInt16	just = UTextTraits::SetPortTextTraits(mTxtrID);
	else
		::TextFont(1);
}

