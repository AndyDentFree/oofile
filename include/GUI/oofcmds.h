// oofcmds.h
// part of the OOFILE MFC GUI integration layer
// Copyright 1998 A.D. Software, All Rights Reserved

#ifndef H_oofCmds
#define H_oofCmds

#include "oofplat.h"
#ifndef _Macintosh  // so doxygen will avoid processing

/**
	\name oofPPCommands MFC command constants
	commands may be used on buttons so keep in resource range 100-0x7FFF
	keep a contiguous range
	\ingroup oofMacros
*/
//@{

#define	kFirstOOFILEcmd	0x0DB1
#define	cmd_dbFirst	kFirstOOFILEcmd  // Mac version		= 'dFst';	// 1682338676
#define	cmd_dbPrev	0x0DB2  // Mac version		= 'dPrv';	// 1682993782
#define	cmd_dbNext	0x0DB3  // Mac version		= 'dNxt';	// 1682864244
#define	cmd_dbLast	0x0DB4  // Mac version		= 'dLst';	// 1682731892

#define	cmd_dbNew	0x0DB5  // Mac version		= 'dNew';	// 1682859383
#define	cmd_dbClone	0x0DB6  // Mac version		= 'dCln';	// 1682140270
#define	cmd_dbEdit	0x0DB7  // Mac version		= 'dEdt';	// 1682269300
#define	cmd_dbView	0x0DB8  // Mac version		= 'dVuw';	// 1683387767
#define	cmd_dbDel 	0x0DB9  // Mac version		= 'dDel';	// 1682204012
#define	cmd_dbDoubleClick	0x0DBA  // Mac version		= 'd2Cl';	// 1681015660
#define	cmd_dbBye	0x0DBB  // Mac version		= 'dBye';	// 1682078053

#define	cmd_dbSave	0x0DBC  // Mac version		= 'dSav';
#define	cmd_dbRevert	0x0DBD  // Mac version		= 'dRev';

#define cmd_dbLockedOnEntry	0x0DBE  // Mac version		= 'dLOE';

#define	kLastOOFILEcmd	cmd_dbLockedOnEntry
//@}

#endif  // !_Macintosh for doxygen
#endif

