// =================================================================================
//	PlotPane.cpp
// =================================================================================

#include "PlotPane.h"
#include "oofGrphs.h"

#ifdef OOF_DEBUG_WITH_SMARTHEAP
	#include "smrtheap.hpp"
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
		Int16	just = UTextTraits::SetPortTextTraits(mTxtrID);
	else
		::TextFont(1);
}
