// COPYRIGHT 1995 A.D. Software, All rights reserved

// Private interface of PowerPlant Integration layer of OOFILE database

#ifndef H_OOFPowerPlantX
#define H_OOFPowerPlantX

#ifndef H_OOFPowerPlant
	#include "oofpp.h"
#endif
#ifndef H_OOF1
	#include "oof1.h"
#endif
class dbView;

#include "LTextTableView.h"
#include <UTableHelpers.h>
#include <Lattachment.h>
#include <PP_Messages.h>

// copied from LTabGroup.cp
const UInt16	mask_NonShiftModifiers	= cmdKey + optionKey + controlKey;


/**
	Link a dbView to a single textual list control LListBox.
	\ingroup oofGUI oofMacSpecific
*/
class dbListBoxDisplay : public dbTableDisplay {
public:
	dbListBoxDisplay(LListBox* theLB, dbView* theView=0);
	virtual ~dbListBoxDisplay() {};
	virtual void loadData();
	virtual void refresh();
	virtual void changeViewTo(dbView*);
	virtual long highlightedRow() const;
	virtual long nextHighlightedRow(long afterRow) const { assert(0); /* NOT YET IMPLEMENTED */ return -1;};
	virtual void highlightRow(unsigned long zeroBasedRowNum, bool bSelect=TRUE) { assert(0); /* NOT YET IMPLEMENTED */};
	virtual void highlightNothing() { assert(0); /* NOT YET IMPLEMENTED */};
	virtual void appendRow() { assert(0); /* NOT YET IMPLEMENTED */};
	virtual void deleteRow(unsigned long zeroBasedRowNum) { assert(0); /* NOT YET IMPLEMENTED */};
	virtual bool displayUsesControl(void*) const { assert(0); return false;/* NOT YET IMPLEMENTED */};
	virtual void setDoubleClickMessage(MessageT);
	
private:
	void DeleteAllData();
	void LoadRow(const int row, bool insertTheRow=false);
	void SetColWidths();
	
	LListBox* mDisplay;
	dbView* mView;
	bool mLoadedData;
};


/**
	Link a dbView to a single textual list control LTextTableView.
	\see dbViewAsTableStorage for the actual data transfer
	\ingroup oofGUI oofMacSpecific
*/
class dbTableViewDisplay : public dbTableDisplay, public oofSingleListener {
public:
	dbTableViewDisplay(LTextTableView* inTable, dbView* inView=0, unsigned short headingRows=0);
	virtual ~dbTableViewDisplay() {};
	virtual void loadData();
	virtual void refresh();
	virtual void changeViewTo(dbView*);
	virtual long highlightedRow() const;
	virtual long nextHighlightedRow(long afterRow) const;
	virtual void highlightRow(unsigned long zeroBasedRowNum, bool bSelect=TRUE);
	virtual void highlightNothing();
	virtual void appendRow();
	virtual void deleteRow(unsigned long zeroBasedRowNum);
	virtual void setDoubleClickMessage(MessageT);
	virtual bool displayUsesControl(void*) const;

	virtual bool receiveMsg(OOFmsgT msg, unsigned long senderDefined);

private:
	
	LTextTableView* mDisplay;
	LTableStorage* mStorage;  ///< created but not owned
	dbView* mView;
	bool mLoadedData;
};


/**
	Provide dbView as storage for a plain list control LTableView.
	Abstracts the data as an two-dimensional array where columns are fields in the view.
	Rows correspond to iterating the dbView through its records.
	Remember that a dbView can point to related fields or standalone fields to display
	constant or independently calculated data.
	\see dbCalculator oofStandaloneFields
	\ingroup oofGUI oofMacSpecific
*/
class	dbViewAsTableStorage : public LTableStorage {
public:
						dbViewAsTableStorage(
								LTableView			*inTableView,
								dbView				*inView, 
								unsigned short 	headingRows=0);
	virtual				~dbViewAsTableStorage() {};

	virtual void		SetCellData(
								const STableCell	&inCell,
								const void			*inDataPtr,
								UInt32				inDataSize);
	virtual void		GetCellData(
								const STableCell	&inCell,
								void				*outDataPtr,
								UInt32				&ioDataSize) const;
	virtual Boolean		FindCellData(
								STableCell			&outCell, 
								const void			*inDataPtr,
								UInt32				inDataSize) const;
								
	virtual void		InsertRows(
								UInt32				inHowMany,
								TableIndexT			inAfterRow,
								const void			*inDataPtr,
								UInt32				inDataSize);
	virtual void		InsertCols(
								UInt32				inHowMany,
								TableIndexT			inAfterCol,
								const void			*inDataPtr,
								UInt32				inDataSize);
								
	virtual void		RemoveRows(
								UInt32				inHowMany,
								TableIndexT			inFromRow);
	virtual void		RemoveCols(
								UInt32				inHowMany,
								TableIndexT			inFromCol);
									
	virtual void		GetStorageSize(
								TableIndexT			&outRows,
								TableIndexT			&outCols);

protected:
	dbView	*mView;  // ref only
	unsigned short mHeadingRows;
};


/**
	Create the dbGUI for the specified dbTable for PowerPlant.
	\ingroup oofGUI oofMacSpecific
*/
class dbGUIFactoryPP : public dbGUIFactory {
public:
	virtual dbGUI*  concreteMakeGUI(dbTable*);	
	
};


#endif

