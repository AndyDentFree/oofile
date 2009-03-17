// COPYRIGHT 1995 A.D. Software, All rights reserved

// Private interface of MFC Integration layer of OOFILE database

#ifndef H_OOFMFCX
#define H_OOFMFCX

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

#ifndef H_OOFMFC
	#include "oofmfc.h"
#endif
#ifndef H_OOF1
	#include "oof1.h"
#endif
class dbView;

/**
	Link a dbView to a single textual list control LListBox.
	\ingroup oofGUI oofWinSpecific
*/
class dbListBoxDisplay : public dbTableDisplay {
public:
	dbListBoxDisplay(COOFListBox* theLB, dbView* theView=0);
	virtual ~dbListBoxDisplay() {}
	virtual void loadData();
	virtual void refresh();
	virtual void changeViewTo(dbView*);
	virtual long highlightedRow() const;
	virtual long nextHighlightedRow(long afterRow) const;
	virtual void highlightRow(unsigned long zeroBasedRowNum, bool bSelect=TRUE);
	virtual void highlightNothing();
	virtual void appendRow() { assert(0); /* NOT YET IMPLEMENTED */};
	virtual void deleteRow(unsigned long zeroBasedRowNum);
	virtual bool displayUsesControl(void* inControl) const;
	virtual void setDoubleClickMessage(MessageT);

private:
	void DeleteAllData();
	bool MultipleSelectionEnabled() const;
//	void LoadRow(const int row, bool insertTheRow=false);
	void SetColWidths();

	COOFListBox* mDisplay;
	dbView* mView;
	bool mLoadedData;
};


/**
	Link a dbView to a single textual list control COOFListCtrl.
	\ingroup oofGUI oofWinSpecific
*/
class dbListCtrlDisplay : public dbTableDisplay {
public:
	dbListCtrlDisplay(COOFListCtrl* theLBCtrl, dbView* theView=0);
	virtual ~dbListCtrlDisplay() {}
	virtual void loadData();
	virtual void refresh();
	virtual void changeViewTo(dbView*);
	virtual long highlightedRow() const;
	virtual long nextHighlightedRow(long afterRow) const;
	virtual void highlightRow(unsigned long zeroBasedRowNum, bool bSelect=TRUE);
	virtual void highlightNothing();
	virtual void appendRow() { assert(0); /* NOT YET IMPLEMENTED */};
	virtual void deleteRow(unsigned long zeroBasedRowNum);
	virtual bool displayUsesControl(void* inControl) const;
	virtual void setDoubleClickMessage(MessageT);

private:
	void DeleteAllData();
//	void LoadRow(const int row, bool insertTheRow=false);
	void SetColWidths();
	void CheckColumns();
	
	COOFListCtrl* mDisplay;
	dbView* mView;
	bool mLoadedData;
};


/**
	Create the dbGUI for the specified dbTable for MFC.
	\ingroup oofGUI oofWinSpecific
*/
class OOFGUI_EXPORT dbGUIFactoryMFC : public dbGUIFactory {
public:
	dbGUIFactoryMFC();
public:
	virtual dbGUI*  concreteMakeGUI(dbTable*);	
	virtual ~dbGUIFactoryMFC();

#ifdef OOFMFC_STATIC
public:
	static HINSTANCE getResourceHandle();
protected:
	static HINSTANCE mResourceDLL;
#endif
};

#ifdef OOFMFC_STATIC
/**
	Load OOFILE resources from DLL in otherwise static MFC build.
	\todo research exactly how this works and confirm above description
	\note Elemir wrote this class.
	\ingroup oofWinSpecific oofGUI
*/
class SetOOFileResources {
public:
	SetOOFileResources();
	~SetOOFileResources();
protected:
	HINSTANCE mResourceHandle;
};
#endif


#ifdef OOFMFC_STATIC
// -------------------------------------------------------
//              d b G U I F a c t o r y M F C
// -------------------------------------------------------
inline HINSTANCE
dbGUIFactoryMFC::getResourceHandle()
{
	return mResourceDLL;
}
#endif

#endif  // !_Macintosh for doxygen
#endif
