#ifndef H_OOFPLOTPANE
#define H_OOFPLOTPANE
// =================================================================================
//	CPlotPane.h
// =================================================================================

#include <LPane.h>

class oofGraph;

/**
	Pane for embedding in a Mac window that draws an oofGraph.
*/
class CPlotPane : public LPane {
public:
	enum { class_ID = 'PltP' };

	static CPlotPane *	CreatePlotPaneStream( LStream *inStream );
						CPlotPane();
						CPlotPane( const SPaneInfo &inPaneInfo,
							ResIDT			inTextTraitsID );
						CPlotPane( const CPlotPane &inOriginal );
						CPlotPane( LStream *inStream );
	virtual				~CPlotPane();

	virtual void		SetGraph(oofGraph* theGraph);
	void				SetPortFont();

protected:	
	virtual void		DrawSelf();

	oofGraph			*mGraphPtr;
	ResIDT				mTxtrID;
};
#endif
