#ifndef H_OOFGRWIN
#define H_OOFGRWIN
// ===========================================================================
//	oofGrPP.h					
// ===========================================================================

#include <LWindow.h>
class 	LCommander;
class	dbView;
class	oofGraph;
class	LStream;

/**
	Preview window used to show an oofGraph in PowerPlant framework.
	\ingroup oofGraph  oofMacSpecific
*/
class	oofGraphWindow : public LWindow{
public:
	enum { class_ID = 'GrWn' };
	
						oofGraphWindow(LStream	*inStream);
	virtual				~oofGraphWindow();
	static oofGraphWindow* CreateFromStream(LStream*);
	
	virtual Boolean		ObeyCommand(CommandT inCommand, void *ioParam = nil);
	virtual void		FindCommandStatus(CommandT inCommand,
							Boolean &outEnabled, Boolean &outUsesMark,
							UInt16 &outMark, Str255 outName);
protected:
	void				DoPageSetup();
	void				DoPrint();
	void				CopyToScrap();

protected:
	oofGraph*			mGraphPtr;

public:
	static void RegisterClass();
	
	static oofGraphWindow* CreateWindow(
		LCommander* inSuperCommander, 
		oofGraph* inGraphPtr,
		dbView *theView, 
		const char* theTitle, 
		unsigned long AxisLength=0);
	
	static oofGraphWindow* CreateBarGraphWindow(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle, 
		unsigned long AxisLength=0);

	static oofGraphWindow* CreateStackedBarGraphWindow(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle, 
		unsigned long AxisLength=0);

	static oofGraphWindow* CreateStackedBarGraphWindowWithOverlay(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle,
		unsigned long NumOverlaid,
		unsigned long AxisLength=0);

	static oofGraphWindow* CreateStackedBarGraphWindowWithOverlayView(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle,
		dbView *overlayView,
		unsigned long AxisLength=0);

	static oofGraphWindow* CreateOrdinalStackedBarGraphWindow(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle, 
		unsigned long AxisLength=0);

	static oofGraphWindow* CreateColumnGraphWindow(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle, 
		unsigned long AxisLength=0);

	static oofGraphWindow* CreatePointGraphWindow(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle, 
		unsigned long AxisLength=0);

	static oofGraphWindow* CreateLineGraphWindow(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle, 
		unsigned long AxisLength=0);

	static oofGraphWindow* CreateXYGraphWindow(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle, 
		unsigned long AxisLength=0);

	static oofGraphWindow* CreatePieChartWindow(
		LCommander* inSuperCommander, 
		dbView *theView, 
		const char* theTitle);
};
#endif

