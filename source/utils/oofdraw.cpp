//////////////////////////////////////////////////////////////////
// oofDraw.cpp - Implementation for oofCurrentDraw - holder of current
//				 drawing info & other drawing state and measurement classes
//
// COPYRIGHT 1996-1998 A.D. Software, All rights reserved

#include "oofpch_g.h"  // for precompilation of graph files

#include "oofDraw.h"
#ifndef H_OOF1
	#include "oof1.h"
#endif

#include <ctype.h>
#include <assert.h>

#ifdef _Macintosh
	#include <QuickdrawText.h>
	#include <Resources.h>
#endif

#ifdef OOF_MEM_DEBUG_LAST_INCLUDE
	#include OOF_MEM_DEBUG_LAST_INCLUDE
#endif

#ifdef _Windows
#ifndef _INC_WINDOWSX
	#include "windowsx.h"
#endif

HDC oofCurrentDraw::shdc	= 0;
HDC oofCurrentDraw::sInfoDC	= 0;
oofCurrentDraw oofCurrentDraw::sCurrentDraw;  // single instance to guarantee destruction of resources
int oofCurrentDraw::sX = 0;
int oofCurrentDraw::sY = 0;

// showdib prototypes & defs
/* struct to be passed in for the SETDIBSCALING printer escape */
typedef struct {
        SHORT ScaleMode;
        SHORT dx, dy;
} DIBPARAMS;

// Old MFC Mac portability macros
#define SWAPWORD(x) x
#define SWAPLONG(x) x

#define MAXREAD  32768                 /* Number of bytes to be read during */
                                       /* each read operation.              */
/* Header signatutes for various resources */
#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

/* macro to determine if resource is a DIB */
#define ISDIB(bft) ((bft) == BFT_BITMAP)

/* Macro to align given value to the closest DWORD (unsigned long ) */
#define ALIGNULONG(i)   ((i+3)/4*4)

/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)   ((i+31)/32*4)

#define PALVERSION      0x300
#define MAXPALETTE      256       /* max. # supported palette entries */

HANDLE OpenDIB (LPSTR szFile);
VOID FreeDib();
INT InitDIB();
BOOL StretchDibBlt (
    HDC hdc,
    INT x,
    INT y,
    INT dx,
    INT dy,
    HANDLE hdib,
    INT x0,
    INT y0,
    INT dx0,
    INT dy0,
    LONG rop);

VOID PrintDIB (
    HDC hDC,
    INT x,
    INT y,
    INT dx,
    INT dy);

DWORD PASCAL lread (
    INT       fh,
    VOID FAR      *pv,
    DWORD             ul);


HANDLE ReadDibBitmapInfo (INT fh);
BOOL DibInfo (
    HANDLE hbi,
    LPBITMAPINFOHEADER lpbi);
HPALETTE CreateDibPalette (HANDLE hbi);
HPALETTE CreateBIPalette (LPBITMAPINFOHEADER lpbi);
WORD DibNumColors (VOID FAR * pv);
WORD PaletteSize (VOID FAR * pv);
VOID ReadBitMapFileHeaderandConvertToDwordAlign(HFILE fh, 
												LPBITMAPFILEHEADER pbf, 
												LPDWORD lpdwoff);
INT ErrMsg (PSTR sz,...);
BOOL DrawBitmap (
    HDC    hdc,
    INT    x,
    INT    y,
    HBITMAP    hbm,
    DWORD          rop);
BOOL DrawBitmapScaled (
    HDC    hdc,
    INT    x,
    INT    y,
    INT    drawWidth,
    INT    drawHeight,
    HBITMAP    hbm,
    DWORD          rop);
HBITMAP BitmapFromDib (
    HANDLE         hdib,
    HPALETTE   hpal);

// showdib variables

DIBPARAMS      DIBParams;                  /* params for the SETSCALING escape */
CHAR           achFileName[128];
DWORD          dwOffset;
NPLOGPALETTE   pLogPal;
HPALETTE       hpalSave = NULL;
HANDLE         hInst ;
RECT           rcClip;
static         HCURSOR hcurSave;

BOOL    fPalColors  = FALSE;          /* TRUE if the current DIB's color table   */
                                      /* contains palette indexes not rgb values */
UINT    nAnimating  = 0;              /* Palette animation count                 */
WORD    UpdateCount = 0;

BOOL    bUpdateColors = TRUE;  /* Directly update screen colors                */
BOOL    bDIBToDevice  = FALSE; /* Use SetDIBitsToDevice() to BLT data.         */
BOOL    bNoUgly       = FALSE; /* Make window black on a WM_PALETTEISCHANGING  */
BOOL    bLegitDraw    = FALSE; /* We have a valid bitmap to draw               */

CHAR    szBitmapExt[] = "*.BMP; *.DIB; *.RLE";     /* possible file extensions */
WORD    wTransparent  = TRANSPARENT;               /* Mode of DC               */
CHAR    szAppName[]   = "ShowDIB" ;                /* App. name                */

HPALETTE hpalCurrent   = NULL;         /* Handle to current palette            */
HANDLE   hdibCurrent   = NULL;         /* Handle to current memory DIB         */
HBITMAP  hbmCurrent    = NULL;         /* Handle to current memory BITMAP      */
HANDLE   hbiCurrent    = NULL;         /* Handle to current bitmap info struct */
HWND     hWndApp;                      /* Handle to app. window                */


// end of showdib variables



// -------------------------------------------------------
//             o o f C u r r e n t D r a w
// -------------------------------------------------------
oofCurrentDraw::oofCurrentDraw()
{
}


oofCurrentDraw::~oofCurrentDraw()
{
	deleteInfoDC();
	assert(shdc==0);	// whoever last used us as a container should have cleaned up their private dc
}


void 
oofCurrentDraw::makeInfoDC()
{
	if (!sInfoDC) {
		sInfoDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
		assert(sInfoDC);
		makeDC72dpiLogical(sInfoDC);
	}
}


void
oofCurrentDraw::makeDC72dpiLogical(HDC hdc)
{
		// stuff because we want to work in 72dpi pts
		// and LOGPIXELSY always returns 96 regardless of actual dpi
		if (hdc==0)
			return;  // **** early exit - assume caller resetting their DC or similar action
			
		const int horzres = ::GetDeviceCaps(hdc, HORZRES);
		const int horzsize = ::GetDeviceCaps(hdc, HORZSIZE);  // always mm
		const int vertres = ::GetDeviceCaps(hdc, VERTRES);
		const int vertsize = ::GetDeviceCaps(hdc, VERTSIZE);
		
		const double devHorDPI = 25.4*horzres/horzsize;
		const double devVertDPI = 25.4*vertres/vertsize;
		
		const int horDP10i = 10 * devHorDPI;
		const int vertDP10i = 10 * devVertDPI;
		assert(horDP10i<32767);  // safe for Win95
		assert(vertDP10i<32767);  // safe for Win95
		SIZE oldViewExtent, oldWindowExtent;
		::SetMapMode(hdc, MM_ANISOTROPIC);
		::SetViewportExtEx(hdc, horDP10i, vertDP10i, &oldViewExtent);
		::SetWindowExtEx(hdc, 720, 720, &oldWindowExtent);
}


void 
oofCurrentDraw::deleteInfoDC()
{
	if (sInfoDC)
		::DeleteDC(sInfoDC);
	sInfoDC = 0;
}


BOOL 
oofCurrentDraw::moveToEx(HDC hdc, int X, int Y, LPPOINT lpPoint)
{
	BOOL ret = true;
	
	if (hdc) {
		ret = ::MoveToEx(hdc, X, Y, lpPoint);
	
	}  // if a DC
	if (lpPoint != NULL) {  // update the point with our cached last locn as metafile doesn't
		lpPoint->x = sX;
		lpPoint->y = sY;
	}
	sX = X;  // update our location regardless - important for recording draws on Windows
	sY = Y;

	return ret;
}


void
oofCurrentDraw::resetCoordinates()
{
	sX = 0;
	sY = 0;
}


void	
oofCurrentDraw::setCurrentDC(HDC hdc)
{ 
	if (hdc==0)
		oofTextDrawer::cleanupFont();
	shdc = hdc; 
	makeDC72dpiLogical(hdc);
}
#endif // _Windows


// -------------------------------------------------------
//             o o f T e x t D r a w e r
// -------------------------------------------------------

#ifdef _Windows
HFONT oofTextDrawer::sFont = NULL;
#endif
int oofTextDrawer::sLastCharHeight, oofTextDrawer::sLastCharHeightRequest, oofTextDrawer::sCurrentFontSize;


#ifdef _Macintosh
bool
oofTextDrawer::setFontSize(int textSize)
{
	bool	retVal;

	if (textSize == -1)
		textSize = 10;
//david debug 990810
//	if (textSize==sCurrentFontSize)
//		return true;  // early return optimisation
//david debug 990810		
	::TextSize(textSize);
	retVal = true;
	sCurrentFontSize = textSize;
	return retVal;
}


#elif defined _Windows
BOOL
oofTextDrawer::setFontSize(int textSize)
{
	bool	retVal;

	if (textSize == -1)
		textSize = 10;

	if (textSize==sCurrentFontSize)
		return true;  // early return optimisation
		
	HDC		dc = oofCurrentDraw::getCurrentDC();

		if (sFont) {
			LOGFONT		lf;

			// Create the new font based on the currently selected one
			::GetObject(sFont, sizeof(LOGFONT), &lf);
		HDC ic = oofCurrentDraw::getInfoDC();
		assert(ic);
		const int pixelsY = ::GetDeviceCaps(ic, LOGPIXELSY);
		lf.lfHeight = -textSize;  // -MulDiv(textSize, pixelsY, 72);
			HFONT newFont = oofTextDrawer::createFontIndirect(&lf);
		if (dc)
			selectFont(dc, newFont);  // also sets ic

		retVal = true;
	}
	else
		retVal = false;
	sCurrentFontSize = textSize;
	return retVal;
}
#endif	// OS

#ifdef _Macintosh
bool
oofTextDrawer::invariant() 

{
	bool	retVal = false;
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 
	GrafPtr currentPort;
	::GetPort(&currentPort);
	short theSize = GetPortTextFont (currentPort);
#else
	short theSize = (qd.thePort->txSize);
	short theFont = (qd.thePort->txFont);
#endif
	if(theSize == sCurrentFontSize)
		retVal = true;	
	return retVal;
}	
	
#elif defined _Windows // andy not much of an idea how to do this 
bool					
oofTextDrawer::invariant()
{
	bool	retVal = false;
	return true; // david debug to be fixed
} 
#endif // OS



		
int
oofTextDrawer::calcCharHeight(int textSize)
{
	int	charHeight = 0;

	if (textSize==sLastCharHeightRequest)
		return sLastCharHeight;  // early exit - cached value
		
#ifdef _Macintosh
	if (setFontSize(textSize)) {
		FontInfo fi;
		::GetFontInfo(&fi);
		charHeight	= (fi.ascent+fi.descent+fi.leading);
	}
#elif defined _Windows
	if (textSize == -1)
		textSize = 10;  // UGLY HARDCODED VALUE

	HDC ic = oofCurrentDraw::getInfoDC();

	if (sFont != NULL) {
		LOGFONT lf;
		::GetObject(sFont, sizeof(LOGFONT), &lf);
		lf.lfHeight = -textSize;  // -MulDiv(textSize, ::GetDeviceCaps(ic, LOGPIXELSY), 72);

		HFONT hTestFont = oofTextDrawer::createFontIndirect(&lf);
		HFONT hOldFont = (HFONT)::SelectObject(ic, hTestFont);

		TEXTMETRIC tm;
		memset(&tm, 0, sizeof(TEXTMETRIC));
		::GetTextMetrics(ic, &tm);

// select old or system font so our testFont is freed up for deletion
		if (hOldFont) 
			::SelectObject(ic, hOldFont);
		else 
			::SelectObject(ic, GetStockFont(SYSTEM_FONT));
				
		::DeleteObject(hTestFont);
		charHeight = tm.tmHeight + tm.tmExternalLeading;
	}
#endif

	// update cached values
	sLastCharHeightRequest = textSize;
	sLastCharHeight = charHeight;
	return charHeight;
}


#ifdef _Windows
void
oofTextDrawer::cleanupFont()
{
	sLastCharHeightRequest = 0;  // just to be safe
	sCurrentFontSize = 0;
// select stock fonts into the IC and DC so we can delete our cached font, if any
	if (sFont) {
		HFONT sysFont = GetStockFont(SYSTEM_FONT);
		{
			HDC ic = oofCurrentDraw::getInfoDC();
			if (ic)
				::SelectObject(ic, sysFont);
		}
		{
			HDC dc = oofCurrentDraw::getCurrentDC();
			if (dc)
				::SelectObject(dc, sysFont);
		}
		::DeleteObject(sFont);
		sFont = 0;
	}
}


BOOL  
oofTextDrawer::textOutCP(HDC hdc, LPCTSTR lpString, int cbString, int textSize)
{
	const BOOL retVal = textOut(hdc, oofCurrentDraw::x(), oofCurrentDraw::y(), lpString, cbString);
	oofCurrentDraw::incX(textSize);
	return retVal;
}


BOOL  
oofTextDrawer::textOutCP(HDC hdc, const oofString& inStr, int textSize)
{
	const BOOL retVal = textOut(hdc, oofCurrentDraw::x(), oofCurrentDraw::y(), inStr);
	oofCurrentDraw::incX(textSize);
	return retVal;
}


BOOL  
oofTextDrawer::textOut(HDC hdc, int x, int y, LPCTSTR lpString, int cbString)
{
	const BOOL retVal = ::TextOut(hdc, x, y, lpString, cbString);
	return retVal;
}


BOOL  
oofTextDrawer::textOut(HDC hdc, int x, int y, const oofString& inStr)
{
	const BOOL retVal = ::TextOut(hdc, x, y, inStr, inStr.length());
	return retVal;
}


SIZE 
oofTextDrawer::getTextExtent(const oofString& text)
{
	HDC ic = oofCurrentDraw::getInfoDC();
	assert(ic != NULL);
	assert(sFont != NULL);

	HFONT hOldFont = (HFONT)::SelectObject(ic, sFont);
	assert(hOldFont);

/* AD980519 not win32s compliant
	SIZE size;
	::GetTextExtentPoint32(ic, text, text.length(), &size);
*/
		SIZE size;
	const unsigned long textLen =  text.length();
	if (textLen==0)
		size.cx = 0;
	else {
		DWORD retSize = ::GetTabbedTextExtent(ic, text, textLen, 0, 0  /* no tab positions */);
		if (retSize==0) {
			DWORD lastErr = ::GetLastError();  // so we can check in debugger
			assert(!"Error in oofTextDrawer::getTextExtent() getting text size");
		}
		else 
			size.cx = LOWORD(retSize);  // assume don't care about cy - NOT YET IMPLEMENTED rewrite this call to return X value alone
	}
	::SelectObject(ic, hOldFont);

	return size;
}


HFONT 
oofTextDrawer::selectFont(HDC hdc, HFONT hFont)
{
// NOT YET IMPLEMENTED - compare hFont and sFont content to avoid changing font if the same
// (just what IS the overhead on repeated font creation anyway?)

	if (sFont==hFont)
		return hFont;  // trivial exit
		
	sLastCharHeightRequest = 0;  // clear caches - don't know if new font is same size
	sCurrentFontSize = 0;  // important flag to avoid resetting size
	
	HFONT prevICfont=0, prevDCfont=0;
	if (hdc)
		prevDCfont = (HFONT) ::SelectObject(hdc, hFont);
	HDC currentIC = oofCurrentDraw::getInfoDC();
	if (currentIC==hdc) 
		prevICfont = prevDCfont;
	else { // also implies not NULL 
		assert(currentIC);
		prevICfont = (HFONT) ::SelectObject(currentIC, hFont);
	}
	
	if (sFont) { // can delete old font after freeing from above
		// assert(sFont==prevICfont || sFont==prevDCfont); // user could trigger this by direct font setting in their code
		// so this is a good check that code called by report-writer or graph uses oofTextDrawer
		// REMOVED ASSERTION 990428 as it triggers in sample reports when go to print
		::DeleteObject(sFont);  // VITAL - avoid leaks!!!!!
	}
	sFont = hFont;
	return prevICfont;
}


HFONT 
oofTextDrawer::createFontIndirect(const LOGFONT* lpLogFont)
{ 
// incorporates dreadful bloody hack to cope with NT printing for now
	// to match the standard interface we have to take a const pointer
	// therefore we copy it if we need to hack values
	HFONT ret;
	LOGFONT copiedFont; // it's on the stack, don't fuss with heap alloc
	bool changedFontDesc = false;
	/*
	switch (oofRuntimeEnv::runtimeEnv()) {
	case oofRuntimeEnv::WinNT:
		changedFontDesc = true;
		memcpy(&copiedFont, lpLogFont, sizeof(LOGFONT));
		// only change fonts that specify char height, ie: negative heights
		if (copiedFont.lfHeight < -4)	 {// arbitrary, but seems reasonable
			const int reduceBy = (copiedFont.lfHeight-0)/7;  // ie: 2 for 14, 3 for 21
			copiedFont.lfHeight -=reduceBy;  
		}
		copiedFont.lfOutPrecision = OUT_OUTLINE_PRECIS;		// TT or other outline fonts
		copiedFont.lfQuality = PROOF_QUALITY;  // not allowed to scale raster fonts
		break;
		
	case oofRuntimeEnv::Win32s:
	// NOT YET IMPLEMENTD - dunno if we need to adjust yet
		break;
		
	default:
		;
	}
	*/
	if (changedFontDesc)
		ret = ::CreateFontIndirect(&copiedFont);
	else
		ret = ::CreateFontIndirect(lpLogFont);
	return ret; 
}
	
	
int 
oofTextDrawer::getCurrentFontDesc(LOGFONT* lf)
{
	return ::GetObject(sFont, sizeof(LOGFONT), lf);
}


HFONT 
oofTextDrawer::getCurrentFont()
{
	return sFont;
}


#endif // _Windows


// -------------------------------------------------------
//                 o o f T e x t S i z e r
// -------------------------------------------------------
//	A simple sizer which wraps to multiple rows
// This is designed to be used one-on-one by objects
// as it calculates and retains a LineStarts array
// for when you draw the text.



// ---------------------------------------------------------------------------
//		 oofTextSizer  - Default Constructor
// ---------------------------------------------------------------------------

oofTextSizer::oofTextSizer() :	
	mMeasureArray(0),
	mLineStarts(0),
	mLineLengths(0),
	mMeasureLen(0),
	mNumRows(0),
	mLineStartsCells(0)
{}


// ---------------------------------------------------------------------------
//		 ~oofTextSizer
// ---------------------------------------------------------------------------
//	Destructor

oofTextSizer::~oofTextSizer()
{
	delete[] mMeasureArray;
	delete[] mLineStarts;
	delete[] mLineLengths;
}


// ---------------------------------------------------------------------------
//		 StoreLineStart
// ---------------------------------------------------------------------------

void
oofTextSizer::StoreLineStart(unsigned long inLineStart) 
{
	if (mLineStartsCells <= mNumRows) {
		mLineStartsCells = mNumRows+4;	// minor optim - go up in four's
		unsigned long* newLines = new unsigned long[mLineStartsCells];	
		unsigned long* newLengths = new unsigned long[mLineStartsCells];	
		for (unsigned long i=0; i < mNumRows; i++) {
			newLines[i] = mLineStarts[i];	// copy words
			newLengths[i] = mLineLengths[i];	
		}
		delete[] mLineStarts;
		delete[] mLineLengths;
		mLineStarts = newLines;
		mLineLengths = newLengths;
	}
	mLineStarts[mNumRows] = inLineStart;
	mNumRows++;
}
	

// ---------------------------------------------------------------------------
//		 StoreLineLength
// ---------------------------------------------------------------------------

void
oofTextSizer::StoreLineLength(unsigned long inLineLength) 
{
	// always called after StoreLineStart, so mNumRows has been advanced
	assert(mLineStartsCells >= mNumRows);
	mLineLengths[mNumRows-1] = inLineLength;
}


// ---------------------------------------------------------------------------
//		 SkipWhiteForward
// ---------------------------------------------------------------------------

void
oofTextSizer::SkipWhiteForward(
								const char		*inValue,	// C string
								unsigned long&			ioWordStart,
								unsigned long&			ioCharsLeft) const
{
	if (ioCharsLeft==0)
		return;
		
	unsigned long ix = ioWordStart;
	unsigned long pastLastChar = ix + ioCharsLeft;
	while ((ix < pastLastChar) && /* isspace(inValue[ix]) */ ((inValue[ix]==' ') || (inValue[ix]=='\t')) /* Ken - modification to stop skipping linebreaks !*/)
		++ix;  // skip to start of partial word
	if (ix == pastLastChar) {
		ioCharsLeft = 0;
		ioWordStart = 0;
	}
	else {
		ioCharsLeft -= (ix-ioWordStart);
		ioWordStart = ix;	
	}
}
	

// ---------------------------------------------------------------------------
//		 CalculateWrapping
// ---------------------------------------------------------------------------
// WARNING: must have a port and the correct text traits current to work!

// wraps at the first character that fits in the current col width
// optionally trimming off a partial word if we do truncation
//
// WARNING: must have a port and the correct text traits current to work!
//



/**
Produce two arrays of line start (offset of character from start of string) and line length (pixels).

Each line is terminated at an embedded CR or LF (CRLF counts as one break) or wraps at the nearest
word boundary. Word gaps detected as spaces and tabs.

Line separator characters (CR etc) are skipped by adjustment of the line start positions.
An empty line will create an entry with a length of 0 and a line start of the terminator.

Calling StoreLineStart will expand both arrays as suited, it must therefore be called first.
This updates the mNumRows member which is used (via GetNumRows()) by all clients.

Note that each time we start a line we must re-measure it as proportional fonts will vary
width depending on context. We avoid remeasuring to the end of a very large block by making
an assumption that a line will exceed a threshold of 500 characters in a single line.
(kMaxCharsInSingleLine defined below)

We also pre-scan for terminators as it is more likely that typical text will have lines
much shorter than kMaxCharsInSingleLine.

\todo Could make it smarter when we have to wrap inside a terminator by looping up until that terminator
rather than going to the top of the main loop again.

MAIN ALGORITHM
Repeat
	if starts with terminator
		skip terminator
		make entry for blank line
	else
		pre-scan forward for line terminators, up to kMaxCharsInSingleLine
		if no terminators in this limit, 
			measure up to kMaxCharsInSingleLine
		else
			measure up to terminator
		scan array of sizes to see if fits in fixed line length (pixels, not number of chars)
		if line too long
			scan backwards from stopping point to first word break
		finish line entry
*/
void
oofTextSizer::CalculateWrapping(
								const char		*inValue,	// C string
								unsigned long	inItemSize,
								unsigned long	inColWidth,
								bool 			inWillTruncWords)
{
#ifdef OOF_GRAPH_DEBUG_DUMP
 	ofstream myStream("CalculateWrapping state",ios::app);  // 990806 david debug 
	
	myStream << " --- WRAPPING STATE "<< inValue << endl;

	describe(myStream);
	myStream << " input string: "<< inValue << endl;
	myStream << " Font size: " << qd.thePort->txSize << endl;
	myStream << " Text style: " << qd.thePort->txFace << endl;
	myStream << " Space extra: " << qd.thePort->spExtra << endl;
	myStream << " font: " << qd.thePort->txFont << endl;
	myStream << " --- END WRAPPING STATE "<< inValue << endl<< endl;

#endif
	

	const unsigned short kMaxCharsInSingleLine = 500;  // limit to avoid too much remeasuring
	mNumRows = 0;
	if (!inValue || inItemSize==0) 
		return;  // trivial case - 0 rows as set above
	
	unsigned long charsLeftInString = inItemSize;
	unsigned long measureFrom = 0;
	SkipWhiteForward(inValue, measureFrom, charsLeftInString); // skip leading white space, 

	if (charsLeftInString==0)
		return;	  // trivial case of totally empty line, just whitespace
		
	unsigned short maxCharsToMeasure;
// maintain array used for pixel measurement 
	if (charsLeftInString > kMaxCharsInSingleLine)
		maxCharsToMeasure = kMaxCharsInSingleLine;
	else
		maxCharsToMeasure = charsLeftInString;
	if (mMeasureLen <=maxCharsToMeasure ) {
		delete[] mMeasureArray;
		mMeasureLen = maxCharsToMeasure+1;
		mMeasureArray = new unsigned short[mMeasureLen];
		assert(mMeasureArray);
	}
	
	while (charsLeftInString > 0) {
	// check if line starts with CR, CRLF etc. - is blank line
		unsigned long truncAtChar = 0;
		unsigned long termLen = 0;
		if(inValue[measureFrom]==0x0D)  { // CR - for PC and Mac 
			if (inValue[measureFrom+1]==0x0A)
				termLen = 2;
			else 
				termLen = 1;
		}
		else {
			if (inValue[measureFrom]==0x0A)  // LF - for Unix
				termLen = 1;
		}

		StoreLineStart(measureFrom);	// we know it's not  white space so regardless of how we end, this is where we start
		if (termLen) {  // processing a line terminator, maybe at the start of a blank line or after an earlier line
			StoreLineLength(0);			
			measureFrom += termLen;
			charsLeftInString -= termLen;
		}
		else {  // some text on line
			// prescan forward to find terminator, to cut down on how much text we might measure
			// (makes a big difference with many short lines in a block)
			
			// reset maxCharsToMeasure otherwise stays at length of first short terminated line           
			if (charsLeftInString > kMaxCharsInSingleLine)
				maxCharsToMeasure = kMaxCharsInSingleLine;
			else
				maxCharsToMeasure = charsLeftInString;
		
			unsigned long terminatorAt = 0;
			for (unsigned short preOffset=0; preOffset<maxCharsToMeasure; preOffset++) {
					const char preCh = inValue[preOffset+measureFrom];
					if (preCh==0x0D || preCh==0x0A) {
						maxCharsToMeasure = preOffset;  // just shortens limit used below
						terminatorAt = preOffset;  // used as flag below for quick optimisation
						break;
					}
			}  // loop pre-scanning for terminators
// now measure the text on the line
#ifdef _Macintosh
			::MeasureText(maxCharsToMeasure, &inValue[measureFrom], mMeasureArray);
#elif defined _Windows
// WARNING - under Windows we get a width for the CR and LF characters (I suspect the width of 
// the 'missing character' box) unlike the Mac where they have 0 width

			// Provide equivalent of Mac API call MeasureText(), taking account of proportional fonts
			HDC	hdc = oofCurrentDraw::getInfoDC();  // works with old metafile, but doesn't wrap
			// get cumulative widths each iteration, which takes advantage of font ligature measurement etc.
			// the Mac call starts with a 0 cell - each cell is the accumulated length to its position
			mMeasureArray[0] = 0;
			const char* const startFrom = &inValue[measureFrom];
			const int numChars = maxCharsToMeasure;
			for (int cumChar=1; cumChar<=numChars; cumChar++) {
				DWORD retSize = ::GetTabbedTextExtent(hdc, startFrom, 
					cumChar, 0, 0  /* no tab positions */
				);
				if (retSize==0) {
					DWORD lastErr = ::GetLastError();  // so we can check in debugger
					assert(!"Error in oofTextSizer::CalculateWrapping() getting text size");
				}
				else {
					int cumLen = LOWORD(retSize);
					mMeasureArray[cumChar] = cumLen;
					if (cumLen>inColWidth) {
						maxCharsToMeasure = cumChar;  // optimisation - shorten comparison loop below
						truncAtChar = cumChar;
						break;	// EXIT LOOP - WE JUST EXCEEDED COL WIDTH
					}
				}  // no error measuring
			} // loop string measuring cumulatively


/*  nice approach that is unfortunately not supported under win32s
			// get all widths in one hit, which takes advantage of font ligature measurement etc.
			// the Mac call starts with a 0 cell - each cell is the accumulated length to its position
			SIZE stringSize;
			int charsThatFit = maxCharsToMeasure;
			int* measureArray = new int[maxCharsToMeasure+1];
			measureArray[0] = 0;
			BOOL measuredOK = ::GetTextExtentExPoint(hdc, &inValue[measureFrom], 
				maxCharsToMeasure, inColWidth, &charsThatFit, &measureArray[1], &stringSize
			);
			if (!measuredOK) 
				DWORD lastErr = ::GetLastError();
			if (charsThatFit < maxCharsToMeasure)
				maxCharsToMeasure = charsThatFit;
*/

/* old inaccurate way
			int	charWidth;
			{
				int n, accumWidth;
				for (n = 0, accumWidth = 0; n < maxCharsToMeasure; mMeasureArray[n] = accumWidth, accumWidth += charWidth, n++) 
					::GetCharWidth(hdc, inValue[measureFrom + n], inValue[measureFrom + n], &charWidth);
				// Put the width of the last character in
				mMeasureArray[n] = accumWidth;
			}
*/
#endif	// OS

		// quick optimisation - if we hit a terminator above it is quite likely that the entire line
		// will fit the column, so just check the total length to see
#ifdef _Macintosh
			if ((terminatorAt==0) || (mMeasureArray[terminatorAt] > inColWidth) ){
#elif defined _Windows
		// in our measurement we will have stopped (on Windows) if we pass the limit, so may have adjusted maxCharsToMeasure
			if ((terminatorAt==0) || ((terminatorAt <= maxCharsToMeasure) && (mMeasureArray[terminatorAt] > inColWidth)) ){
#endif	// OS
		// scan forward through text for line terminators
		// checking to see if we pass the pixel length allowed
				const unsigned long lengthLeft = maxCharsToMeasure;  // local const for optimal loop
				for (unsigned long i=0; i<lengthLeft; i++) {
					if (mMeasureArray[i] > inColWidth) {  // just went past allowed pixel length
						truncAtChar = i;
						break;
					}
					const char nextCh = inValue[i+measureFrom];
					if (nextCh==0x0D || nextCh==0x0A) {
						terminatorAt = i;
						break;
					}
				}  // loop through text looking for terminators
			} // if hit terminator in prescan and entire line is wider than column
			
		 if (truncAtChar) {  // truncated within terminator or remainder of line, don't care which
		// have to scan back from where we exceeded pixel length
		// to find a word on which to wrap
				truncAtChar--;
				assert(mMeasureArray[truncAtChar] <= inColWidth);  // because we stop just as we exceed
				if (inWillTruncWords) {  
				// start truncation from one char past fitted string, in case
				// word ended exactly at the boundary
					unsigned long nextWordStart;
					truncAtChar = TruncateTrailingWord(&inValue[measureFrom], truncAtChar, nextWordStart);  // ret is currently 1-based
					if (nextWordStart==0)	// haven't calculated it yet as boundary was in white space
						nextWordStart = truncAtChar+1;  // can assume at least one whitespace char exists
					measureFrom += nextWordStart;  // saves time skipping whitespace later
					StoreLineLength(mMeasureArray[truncAtChar] );
					charsLeftInString  -= nextWordStart;
				}  // inWillTruncWords so had to find word boundary
				SkipWhiteForward(inValue, measureFrom, charsLeftInString); // skip leading white space before next line
			}
			else if (terminatorAt) { // hit terminator after at least one character before line was too long
				StoreLineLength(mMeasureArray[terminatorAt]);
				measureFrom += terminatorAt;
				if (termLen==0) {
					termLen = 1;  // guaranteed a terminator due to checks above setting terminatorAt
					if((inValue[measureFrom]==0x0D) && (inValue[measureFrom+1]==0x0A))
							termLen = 2;
				}
				measureFrom += termLen;
				charsLeftInString -= terminatorAt+termLen;
			}
			else {  // remainder of line fits
				StoreLineLength(mMeasureArray[maxCharsToMeasure]);
				charsLeftInString = 0;			
			}
		} // line with some text
	}  // loop lines

	#ifdef OOF_GRAPH_DEBUG_DUMP
		describe(myStream);
		myStream.close();	
	#endif
}


// ---------------------------------------------------------------------------
//		 describe
// ---------------------------------------------------------------------------
// describe state of drawing engine

void
oofTextSizer::describe(ostream& os) const
{
    int index;
     os << "-=-=-=-= ooftext sizer state -=-=-=-=" << endl
  	<< "address of this: " << (unsigned long) this
    << " mMeasureLen : " << mMeasureLen
    << " mNumRows : " << mNumRows
    << " mLineStartsCells : " << mLineStartsCells 
 
    
    << " mMeasureArray : " ;
    for(index = 0;index < mMeasureLen;index++)
    	os <<  mMeasureArray[index] <<", ";
    os<< " mLineStarts : ";
  	 for(index = 0;index < mNumRows;index++)
    	os <<  mLineStarts[index] <<", ";
    os << endl 
   << " mLineLengths : " ;
   for(index = 0;index < mNumRows;index++)
    	os <<  mLineLengths[index] <<", ";	
   
  	os	 <<  endl << "-=-=-=-= sizer state -=-=-=-=" << endl << endl;
}


// ---------------------------------------------------------------------------
//		 TruncateTrailingWord
// ---------------------------------------------------------------------------
// trimming off a partial word if we do truncation

unsigned long	
oofTextSizer::TruncateTrailingWord(
								const char		*inValue,	// C string
								unsigned long			inEndWord,
								unsigned long&			outNextWordStart) const
{
	unsigned long ix = inEndWord;
	while (ix > 0 && !isspace(inValue[ix]))
		--ix;  // skip to start of partial word
	
	if (ix==0) {
		outNextWordStart = inEndWord+1;
		return inEndWord;	// were part-way through remaining word on line
	}
		
	if (ix < inEndWord)  // were part-way through a word
		outNextWordStart = ix+1;	
	else
		outNextWordStart = 0;
		
	while (ix > 0 && isspace(inValue[ix]))
		--ix;  // skip preceding whitespace
	
	if (ix > 0)  // was at least one word before partial word
		inEndWord = ix+1;
	else
		if (isspace(inValue[1]))  // degenerate case "*<space>BLAH"
			inEndWord = 1;
		
	return inEndWord;
}




// -------------------------------------------------------
//             o o f S i z e d T e x t A r r a y
// -------------------------------------------------------
oofSizedTextArray::oofSizedTextArray(unsigned short numItems) : 
	mSizers(0),
	mStrings(0),
	mCount(0),
	mMaxNumRows(0)
{
	if (numItems)
		initSizers(numItems);
}


oofSizedTextArray::~oofSizedTextArray()
{
	delete [] mSizers;
	delete [] mStrings;
}


void 
oofSizedTextArray::initSizers(unsigned short numItems)
{
// NOTE
// later could use Prototype pattern - init with a prototype sizer
// that we duplicate below, rather than hardcoding the type of sizer

	assert (mCount==0);	// should only call once if didn't alloc size at construction
	mCount = numItems;
	mSizers = new oofTextSizer[numItems];
	mStrings = new OOF_String[numItems];
}


oofString& 
oofSizedTextArray::text(unsigned short i) const
{
	return mStrings[i];
}


const unsigned long* 	
oofSizedTextArray::GetLineStarts(unsigned short index) const
{
	assert(index < mCount);
	return mSizers[index].GetLineStarts();
}


const unsigned long* 	
oofSizedTextArray::GetLineLengths(unsigned short index) const
{
	assert(index < mCount);
	return mSizers[index].GetLineLengths();
}


unsigned short 			
oofSizedTextArray::GetNumRows(unsigned short index) const
{
	assert(index < mCount);
	return mSizers[index].GetNumRows();
}


void		
oofSizedTextArray::CalculateWrapping(unsigned short index,
								unsigned short			inColWidth,
								bool				 		inTruncWords)
{
	assert(index < mCount);
	const char*	value = mStrings[index];
	unsigned short itemSize = mStrings[index].length();
	mSizers[index].CalculateWrapping(value, itemSize, inColWidth, inTruncWords);
	unsigned short numRows = GetNumRows(index);
	if (numRows > mMaxNumRows)
		mMaxNumRows = numRows;	// entire reason for this wrapper is maintaining our max!!
}								


// -------------------------------------------------------
//             o o f P i c t
// -------------------------------------------------------
#ifdef _Macintosh
oofPict::oofPict(unsigned long resourceID) :
	mFromFile(false), 
	mLoaded(false),
	mResID(resourceID)
{
}



#endif


oofPict::oofPict(const oofFileRef& inFile) :
	mHeight(0),
	mWidth(0),
	mFile(inFile),
	mFromFile(true), 
	mLoaded(false)
{
}


oofPict::oofPict(const oofPict& rhs) :
	mHeight(rhs.mHeight),
	mWidth(rhs.mWidth),
	mFile(rhs.mFile),
	mFromFile(rhs.mFromFile), 
	mLoaded(rhs.mLoaded)
#ifdef _Macintosh
	, mGWorld(rhs.mGWorld),
	mResID(rhs.mResID)
#endif
{
}


oofPict::~oofPict()
{
	// Mac:
	// GWorld is automatically taken care of by the concrete OOF_GWorldOwner member mGWorld
	
	// Windows:
	// The Bitmap is only loaded when drawing takes place, all memory
	// used is deleted immediately after drawing.
}


oofPict*
oofPict::clone() const
{
	return new oofPict(*this);
}


unsigned long 
oofPict::height() 
{
	if (!mLoaded)
		Load();
	assert(mHeight);
	return mHeight;
}


unsigned long
oofPict::width() 
{
	if (!mLoaded)
		Load();
	assert(mWidth);
	return mWidth;
}



oofString
oofPict::describe() const
{
	if (mFromFile)
		return mFile.path();
	else {
		oofString ret;
#ifdef _Macintosh
		// mResID only defined on Mac - can't load Windows picts from resource number
		ret.convertNumber(mResID, "Picture Resource ID: %d");
#endif
		return ret;
	}
}


#ifdef _Macintosh
void 
oofPict::Load() 
{
	mHeight = 0;
	mWidth = 0;
	

	CGrafPtr		curPort;
	GDHandle		curDevice;
	PixMapHandle	offPix;
	Rect			pictRect;
	PicHandle		pict;
	GWorldPtr		localGWorld;
	
	// Save current graphics device settings
	::GetGWorld( &curPort, &curDevice );
	
	// Get a handle to the picture
	if (mFromFile)
		pict = readPict(mFile);
	else
		pict = ::GetPicture( mResID );
	
	pictRect.top = 0;
	pictRect.bottom = (**pict).picFrame.bottom - (**pict).picFrame.top;
	pictRect.left = 0;
	pictRect.right = (**pict).picFrame.right - (**pict).picFrame.left;
	
	mHeight = pictRect.bottom;
	mWidth = pictRect.right;
	
	// Create GWorld
	const OSErr newErr = ::NewGWorld( &localGWorld, 0, &pictRect, nil, curDevice, nil );
	
	// Draw into GWorld
	if( newErr==noErr ) {
		offPix = GetGWorldPixMap( localGWorld );
		if( ::LockPixels( offPix ) ) {
			::SetGWorld( localGWorld, nil );
		
	#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 
			Rect theFrame;
			::GetPortBounds(localGWorld, &theFrame);
			::EraseRect(  &theFrame );
	#else
			::EraseRect( &(localGWorld->portRect) );
	#endif	
			::DrawPicture( pict, &pictRect );
			::UnlockPixels( offPix );
		}
	}
	
	// Restore the graphics device settings
	::SetGWorld( curPort, curDevice );
	
	// Remove memory for picture
	if( pict != NULL)
		::ReleaseResource( (Handle)pict );

	// Save the pointer to the concrete member
	mGWorld = localGWorld;
	
	mLoaded = true;
}


PicHandle  
oofPict::readPict(const oofFileRef& inRef)  // _Macintosh
{
	assert(inRef.isValid());
	short	fRef;
	const OSErr openErr = ::FSpOpenDF(&inRef.spec(), fsRdPerm, &fRef);
	long	fileLen;
	const long kPicHeaderLen = 512;
	::GetEOF(fRef, &fileLen);
	::SetFPos(fRef, fsFromStart, kPicHeaderLen);
	long pictLen = fileLen - kPicHeaderLen;
	Handle ret = ::NewHandle(pictLen);
	if (ret) {
		::HLock(ret);
		const OSErr readErr = ::FSRead(fRef, &pictLen, *ret);
		::HUnlock(ret);
	}
	::FSClose(fRef);
	return (PicHandle)ret;
}


void 
oofPict::draw(int x, int y, int right, int bottom, bool onPrinter)
{
	if (!mLoaded)
		Load();
		

	CGrafPtr		curPort;
	GDHandle		curDevice;
	PixMapHandle	offPix;
	Rect			srcRect, destRect;
	
	GWorldPtr theGWorld = mGWorld.gWorld();
	
	if( theGWorld ){
	
		// Save current graphics device settings
		::GetGWorld( &curPort, &curDevice );

		offPix = ::GetGWorldPixMap(theGWorld);

		if( LockPixels( offPix ) ){
			// from Load, we've set mHeight and mWidth to actual pic dimensions
			srcRect.top = 0;
			srcRect.left = 0;
			srcRect.bottom = mHeight;
			srcRect.right = mWidth;
			const short inWidth = right - x;
			const short inHeight = bottom - y;
			short drawWidth = mWidth;
			short drawHeight = mHeight;
			if (mWidth>inWidth) {  // scale down
				drawWidth = inWidth;
				drawHeight = mHeight * inWidth/mWidth;
			}
			else if (mWidth<inWidth) { // centre smaller picture horizontally, leave at top vertically
				x += (inWidth - mWidth)/2;
			}

			destRect.top = y;
			destRect.left = x;
			destRect.bottom = y+drawHeight;
			destRect.right = x+drawWidth;
			
#if defined(TARGET_API_MAC_CARBON) && TARGET_API_MAC_CARBON!=0 
				const BitMap* theBitMap = GetPortBitMapForCopyBits (curPort);
				CopyBits(	(BitMap *)*offPix,
						theBitMap ,
						&srcRect,
						&destRect,
						srcCopy, NULL );		
#else
			::CopyBits(	(BitMap *)*offPix,
						&((GrafPtr)curPort)->portBits,
						&srcRect,
						&destRect,
						srcCopy, NULL );
#endif
			::UnlockPixels( offPix );
		}
	}  // have a gWorld
}


#elif defined _Windows
void 
oofPict::draw(int x, int y, int right, int bottom, bool onPrinter, HDC drawDC)
{
	if (!mLoaded)
		Load();
	else { // hack to make picture bands draw, otherwise they only appear on redraws
		FreeDib();
		Load();
	}
	// draw appropriately
	assert(right>=x);
	assert(bottom>=y);
	const int drawWidth = right - x;
	const int drawHeight = bottom - y;
	if (onPrinter) {
			PrintDIB(drawDC, x, y, drawWidth, drawHeight);
	}
	else { // draw on screen
      if (hbmCurrent && !bDIBToDevice) 
				DrawBitmapScaled (drawDC, x, y, drawWidth, drawHeight, hbmCurrent, SRCCOPY);
	}
	FreeDib();
	mLoaded = false;  // initial test - always unload after drawing
}


void 
oofPict::Load() 
{
	mLoaded = true;
// load the bitmap
	strcpy(achFileName, mFile.path());
	InitDIB();

   BITMAPINFOHEADER bi;
   DibInfo (hbiCurrent, &bi);
	 mHeight = bi.biHeight;
	 mWidth = bi.biWidth;
}

#endif  // _Windows

// -------------------------------------------------------
//            O O F _ m i x P i c t O w n e r
// -------------------------------------------------------
OOF_mixPictOwner::OOF_mixPictOwner(oofPict* inPict) :
	mPict(inPict)
{
}


OOF_mixPictOwner::OOF_mixPictOwner(const OOF_mixPictOwner& rhs) 
{
	if (rhs.mPict)
		mPict = rhs.mPict->clone();
	else
		mPict = 0;
}


OOF_mixPictOwner::~OOF_mixPictOwner()
{
	delete mPict;
}



// -------------------------------------------------------
//      O O F _ G W o r l d O w n e r
// -------------------------------------------------------
#ifdef _Macintosh

OOF_GWorldOwner::OOF_GWorldOwner() :
	mGWorld(0)
{
	mRefs = new unsigned long(1);
}
	
OOF_GWorldOwner::~OOF_GWorldOwner()
{
	if (*mRefs<=1) {
		if( mGWorld != NULL) {
			DisposeGWorld( mGWorld );
			mGWorld = NULL;
		}
		delete mRefs;
	}
	else
	{
		(*mRefs)--;
	}
}

OOF_GWorldOwner::OOF_GWorldOwner(const OOF_GWorldOwner& rhs) :
	mGWorld( rhs.mGWorld ),
	mRefs(rhs.mRefs)
{
	(*mRefs)++;
}

OOF_GWorldOwner&
OOF_GWorldOwner::operator=(GWorldPtr adoptedGWorld)
{
	mGWorld = adoptedGWorld;
	return *this;
}

#endif  // _Macintosh



#ifdef _Windows

// -------------------------------------------------------
//        S H O W D I B   M e t h o d s
// -------------------------------------------------------
DWORD PASCAL lread (
    INT       fh,
    VOID FAR      *pv,
    DWORD             ul)
{
    DWORD     ulT = ul;
    BYTE* hp = (BYTE*)pv;

    while (ul > (DWORD)MAXREAD) {
        if (_lread(fh, (LPSTR)hp, (UINT)MAXREAD) != MAXREAD)
                return 0;
        ul -= MAXREAD;
        hp += MAXREAD;
    }
    if (_lread(fh, (LPSTR)hp, (UINT)ul) != (UINT)ul)
        return 0;
    return ulT;
}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitDIB(hWnd)                                              *
 *                                                                          *
 *  PURPOSE    : Reads a DIB from a file, obtains a handle to it's          *
 *               BITMAPINFO struct., sets up the palette and loads the DIB. *
 *                                                                          *
 *  RETURNS    : TRUE  - DIB loads ok                                       *
 *               FALSE - otherwise                                          *
 *                                                                          *
 ****************************************************************************/
INT InitDIB()
{
    HFILE              fh;
    LPBITMAPINFOHEADER lpbi;
    WORD FAR *         pw;
    INT                i;
    BITMAPINFOHEADER   bi;
    OFSTRUCT           of;

    FreeDib();

    /* Open the file and get a handle to it's BITMAPINFO */

    fh = OpenFile(achFileName, (LPOFSTRUCT)&of, (UINT)OF_READ);
    if (fh == -1) {
        ErrMsg("Can't open file '%ls'", (LPSTR)achFileName);
        return FALSE;
    }
    hbiCurrent = ReadDibBitmapInfo(fh);

    dwOffset = _llseek(fh, 0L, (UINT)SEEK_CUR);
    _lclose(fh);

    if (hbiCurrent == NULL) {
        ErrMsg("%ls is not a Legitimate DIB File!", (LPSTR)achFileName);
        return FALSE;
    }
    DibInfo(hbiCurrent,&bi);

    /* Set up the palette */
    hpalCurrent = CreateDibPalette(hbiCurrent);
    if (hpalCurrent == NULL) {
        ErrMsg("CreatePalette() Failed");
        return FALSE;
    }

    /*  Convert the DIB color table to palette relative indexes, so
     *  SetDIBits() and SetDIBitsToDevice() can avoid color matching.
     *  We can do this because the palette we realize is identical
     *  to the color table of the bitmap, ie the indexes match 1 to 1
     *
     *  Now that the DIB color table is palette indexes not RGB values
     *  we must use DIB_PAL_COLORS as the wUsage parameter to SetDIBits()
     */
    lpbi = (tagBITMAPINFOHEADER*)GlobalLock(hbiCurrent);  //(VOID FAR *)GlobalLock(hbiCurrent);
    if (lpbi->biBitCount != 24) {
        fPalColors = TRUE;

        pw = (WORD FAR *)((LPSTR)lpbi + lpbi->biSize);

        for (i=0; i<(INT)lpbi->biClrUsed; i++)
            *pw++ = (WORD)i;
    }
    GlobalUnlock(hbiCurrent);
    bLegitDraw = TRUE;

    /*  If the input bitmap is not in RGB FORMAT the banding code will
     *  not work!  we need to load the DIB bits into memory.
     *  if memory DIB, load it all NOW!  This will avoid calling the
     *  banding code.
     */
    hdibCurrent = OpenDIB(achFileName);

    /*  If the RLE could not be loaded all at once, exit gracefully NOW,
     *  to avoid calling the banding code
     */
    if ((bi.biCompression != BI_RGB) && !hdibCurrent){
        ErrMsg ("Could not load RLE!");
        FreeDib();
        return FALSE;
    }

    if (hdibCurrent && !bDIBToDevice){
                hbmCurrent = BitmapFromDib(hdibCurrent,hpalCurrent);
                if (!hbmCurrent){
                ErrMsg ("Could not create bitmap!");
                FreeDib();
                return FALSE;
                }
    }
//    SizeWindow(hWnd);

    return TRUE;
}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  PrintDIB(HDC hDC, int x, int y, int dx, int dy)*
 *                                                                          *
 *  PURPOSE    :  Set the DIB bits to the printer DC.                       *
 *                                                                          *
 ****************************************************************************/
VOID PrintDIB (
    HDC hDC,
    INT x,
    INT y,
    INT dx,
    INT dy)

{
    BITMAPINFOHEADER bi;
    INT dibX,  dibY;
    INT dibDX, dibDY;

    if (!bLegitDraw)
        return;

    DibInfo (hbiCurrent, &bi);

    if (IsRectEmpty (&rcClip)){
        dibX  = 0;
        dibY  = 0;
        dibDX = (INT)bi.biWidth;
        dibDY = (INT)bi.biHeight;
    }
    else{
        dibX  = rcClip.left;
        dibY  = (INT)bi.biHeight - 1 - rcClip.bottom;
        dibDX = rcClip.right  - rcClip.left;
        dibDY = rcClip.bottom - rcClip.top;
    }

    if (hdibCurrent){
        /* Stretch the DIB to printer DC */
        StretchDibBlt ( hDC,
                        x,
                        y,
                        dx,
                        dy,
                        hdibCurrent,
                        dibX,
                        dibY,
                        dibDX,
                        dibDY,
                        SRCCOPY);
    }
/*    else if (achFileName[0]) {

        SetMapMode (hDC, MM_ANISOTROPIC);
        (VOID)SetViewportOrgEx (hDC, x, y, NULL);
        (VOID)SetViewportExtEx (hDC, dx, dy, NULL);

        BandDIB (hWnd, hDC, 0, 0);
    }
*/
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : FreeDib(void)                                              *
 *                                                                          *
 *  PURPOSE    : Frees all currently active bitmap, DIB and palette objects *
 *               and initializes their handles.                             *
 *                                                                          *
 ****************************************************************************/
VOID FreeDib()
{
    if (hpalCurrent)
        DeleteObject(hpalCurrent);

    if (hbmCurrent)
        DeleteObject(hbmCurrent);

    if (hdibCurrent)
        GlobalFree(hdibCurrent);

    if (hbiCurrent && hbiCurrent != hdibCurrent)
        GlobalFree(hbiCurrent);

    fPalColors  = FALSE;
    bLegitDraw  = FALSE;
    hpalCurrent = NULL;
    hdibCurrent = NULL;
    hbmCurrent  = NULL;
    hbiCurrent  = NULL;
    SetRectEmpty (&rcClip);
}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : StretchDibBlt( HDC hdc,                                    *
 *                              int x, int y,                               *
 *                              int dx, int dy,                             *
 *                              HANDLE hdib,                                *
 *                              int x0, int y0,                             *
 *                              int dx0, int dy0,                           *
 *                              LONG rop)                                   *
 *                                                                          *
 *  PURPOSE    : Draws a bitmap in CF_DIB format, using StretchDIBits()     *
 *               taking the same parameters as StretchBlt().                *
 *                                                                          *
 *  RETURNS    : TRUE  - if function succeeds.                              *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
BOOL StretchDibBlt (
    HDC hdc,
    INT x,
    INT y,
    INT dx,
    INT dy,
    HANDLE hdib,
    INT x0,
    INT y0,
    INT dx0,
    INT dy0,
    LONG rop)

{
    LPBITMAPINFOHEADER lpbi;
    LPSTR        pBuf;
    BOOL         f;

    if (!hdib)
        return PatBlt(hdc,x,y,dx,dy,rop);

    lpbi = (tagBITMAPINFOHEADER*)GlobalLock(hdib);

    if (!lpbi)
        return FALSE;

    pBuf = (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi);

    f = StretchDIBits ( hdc,
                        x, y,
                        dx, dy,
                        x0, y0,
                        dx0, dy0,
                        pBuf, (LPBITMAPINFO)lpbi,
                        DIB_RGB_COLORS,
                        rop);

    GlobalUnlock(hdib);
    return f;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : ReadDibBitmapInfo(int fh)                                  *
 *                                                                          *
 *  PURPOSE    : Will read a file in DIB format and return a global HANDLE  *
 *               to it's BITMAPINFO.  This function will work with both     *
 *               "old" (BITMAPCOREHEADER) and "new" (BITMAPINFOHEADER)      *
 *               bitmap formats, but will always return a "new" BITMAPINFO  *
 *                                                                          *
 *  RETURNS    : A handle to the BITMAPINFO of the DIB in the file.         *
 *                                                                          *
 ****************************************************************************/
HANDLE ReadDibBitmapInfo (INT fh)
{
    DWORD     off;
    HANDLE    hbi = NULL;
    INT       size;
    INT       i;
    WORD      nNumColors;

    RGBQUAD FAR       *pRgb;
    BITMAPINFOHEADER   bi;
    BITMAPCOREHEADER   bc;
    LPBITMAPINFOHEADER lpbi;
    BITMAPFILEHEADER   bf;
    DWORD              dwWidth = 0;
    DWORD              dwHeight = 0;
    WORD               wPlanes, wBitCount;

    if (fh == -1)
        return NULL;
#ifdef FIXDWORDALIGNMENT
    /* Reset file pointer and read file header */
    off = _llseek(fh, 0L, (UINT)SEEK_CUR);
    if ((SIZEOF_BITMAPFILEHEADER_PACKED)  != _lread(fh, (LPSTR)&bf, (UINT)sizeof (SIZEOF_BITMAPFILEHEADER_PACKED)))
        return FALSE;
#else
        ReadBitMapFileHeaderandConvertToDwordAlign(fh, &bf, &off);
        /* at this point we have read the file into bf*/
#endif

#if defined(_MAC)
   // swap the BITMAPFILEHEADER struct members around
   bf.bfType      = SWAPWORD(bf.bfType);
   bf.bfSize      = SWAPLONG(bf.bfSize);
   bf.bfReserved1 = SWAPWORD(bf.bfReserved1);
   bf.bfReserved2 = SWAPWORD(bf.bfReserved2);
   bf.bfOffBits   = SWAPLONG(bf.bfOffBits);
#endif

    /* Do we have a RC HEADER? */
    if (!ISDIB (bf.bfType)) {
        bf.bfOffBits = 0L;
                _llseek(fh, off, (UINT)SEEK_SET); /*seek back to beginning of file*/
    }
    if (sizeof (bi) != _lread(fh, (LPSTR)&bi, (UINT)sizeof(bi)))
        return FALSE;


    /* Check the nature (BITMAPINFO or BITMAPCORE) of the info. block
     * and extract the field information accordingly. If a BITMAPCOREHEADER,
     * transfer it's field information to a BITMAPINFOHEADER-style block
     */
    switch (size = (INT)(SWAPLONG(bi.biSize))){
        case sizeof (BITMAPINFOHEADER):
            #if defined(_MAC)
                bi.biSize           = SWAPLONG(bi.biSize);
                bi.biWidth          = SWAPLONG(bi.biWidth);
                bi.biHeight         = SWAPLONG(bi.biHeight);
                bi.biPlanes         = SWAPWORD(bi.biPlanes);
                bi.biBitCount       = SWAPWORD(bi.biBitCount);
                bi.biCompression    = SWAPLONG(bi.biCompression);
                bi.biSizeImage      = SWAPLONG(bi.biSizeImage);
                bi.biXPelsPerMeter  = SWAPLONG(bi.biXPelsPerMeter);
                bi.biYPelsPerMeter  = SWAPLONG(bi.biYPelsPerMeter);
                bi.biClrUsed        = SWAPLONG(bi.biClrUsed);
                bi.biClrImportant   = SWAPLONG(bi.biClrImportant);
            #endif
            break;

        case sizeof (BITMAPCOREHEADER):
            bc = *(BITMAPCOREHEADER*)&bi;
            #if defined(_MAC)
                bc.bcSize           = SWAPLONG(bc.bcSize);
                bc.bcWidth          = SWAPWORD(bc.bcWidth);
                bc.bcHeight         = SWAPWORD(bc.bcHeight);
                bc.bcPlanes         = SWAPWORD(bc.bcPlanes);
                bc.bcBitCount       = SWAPWORD(bc.bcBitCount);
            #endif

            nNumColors = DibNumColors (&bi);

            dwWidth   = (DWORD)bc.bcWidth;
            dwHeight  = (DWORD)bc.bcHeight;
            wPlanes   = bc.bcPlanes;
            wBitCount = bc.bcBitCount;

            bi.biSize               = sizeof(BITMAPINFOHEADER);
            bi.biWidth              = dwWidth;
            bi.biHeight             = dwHeight;
            bi.biPlanes             = wPlanes;
            bi.biBitCount           = wBitCount;

            bi.biCompression        = BI_RGB;
            bi.biSizeImage          = 0;
            bi.biXPelsPerMeter      = 0;
            bi.biYPelsPerMeter      = 0;
            bi.biClrUsed            = nNumColors;
            bi.biClrImportant       = nNumColors;

            _llseek(fh, (LONG)sizeof (BITMAPCOREHEADER) - sizeof (BITMAPINFOHEADER), (UINT)SEEK_CUR);
            break;

        default:
            /* Not a DIB! */
            return NULL;
    }

    nNumColors = DibNumColors (&bi);

    /*  Fill in some default values if they are zero */
    if (bi.biSizeImage == 0){
        bi.biSizeImage = WIDTHBYTES ((DWORD)bi.biWidth * bi.biBitCount)
                         * bi.biHeight;
    }
    if (bi.biClrUsed == 0)
        bi.biClrUsed = DibNumColors(&bi);

    /* Allocate for the BITMAPINFO structure and the color table. */
    hbi = GlobalAlloc (GHND, (LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));
    if (!hbi)
        return NULL;
    lpbi = (tagBITMAPINFOHEADER*)GlobalLock (hbi);
    *lpbi = bi;

    /* Get a pointer to the color table */
    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + bi.biSize);
    if (nNumColors){
        if (size == sizeof(BITMAPCOREHEADER)){
            /* Convert a old color table (3 byte RGBTRIPLEs) to a new
             * color table (4 byte RGBQUADs)
             */
            _lread(fh, (LPSTR)pRgb, (UINT)nNumColors * sizeof(RGBTRIPLE));

            for (i = nNumColors - 1; i >= 0; i--){
                RGBQUAD rgb;

                rgb.rgbRed      = ((RGBTRIPLE FAR *)pRgb)[i].rgbtRed;
                rgb.rgbBlue     = ((RGBTRIPLE FAR *)pRgb)[i].rgbtBlue;
                rgb.rgbGreen    = ((RGBTRIPLE FAR *)pRgb)[i].rgbtGreen;
                rgb.rgbReserved = (BYTE)0;

                pRgb[i] = rgb;
            }
        }
        else
            _lread(fh, (LPSTR)pRgb, (UINT)nNumColors * sizeof(RGBQUAD));
    }

    if (bf.bfOffBits != 0L){
        _llseek(fh, off + bf.bfOffBits, (UINT)SEEK_SET);
        }
    GlobalUnlock(hbi);
    return hbi;
}



/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibInfo(HANDLE hbi,LPBITMAPINFOHEADER lpbi)                *
 *                                                                          *
 *  PURPOSE    : Retrieves the DIB info associated with a CF_DIB            *
 *               format memory block.                                       *
 *                                                                          *
 *  RETURNS    : TRUE  - if successful.                                     *
 *               FALSE - otherwise                                          *
 *                                                                          *
 ****************************************************************************/
BOOL DibInfo (
    HANDLE hbi,
    LPBITMAPINFOHEADER lpbi)
{
    if (hbi){
        *lpbi = *(LPBITMAPINFOHEADER)GlobalLock (hbi);

        /* fill in the default fields */
        if (lpbi->biSize != sizeof (BITMAPCOREHEADER)){
            if (lpbi->biSizeImage == 0L)
                                lpbi->biSizeImage = WIDTHBYTES(lpbi->biWidth*lpbi->biBitCount) * lpbi->biHeight;

            if (lpbi->biClrUsed == 0L)
                                lpbi->biClrUsed = DibNumColors (lpbi);
    }
        GlobalUnlock (hbi);
        return TRUE;
    }
    return FALSE;
}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : CreateBIPalette(LPBITMAPINFOHEADER lpbi)                   *
 *                                                                          *
 *  PURPOSE    : Given a Pointer to a BITMAPINFO struct will create a       *
 *               a GDI palette object from the color table.                 *
 *                                                                          *
 *  RETURNS    : A handle to the palette.                                   *
 *                                                                          *
 ****************************************************************************/
HPALETTE CreateBIPalette (LPBITMAPINFOHEADER lpbi)
{
    LOGPALETTE          *pPal;
    HPALETTE            hpal = NULL;
    WORD                nNumColors;
    BYTE                red;
    BYTE                green;
    BYTE                blue;
    WORD                i;
    RGBQUAD        FAR *pRgb;

    if (!lpbi)
        return NULL;

    if (lpbi->biSize != sizeof(BITMAPINFOHEADER))
        return NULL;

    /* Get a pointer to the color table and the number of colors in it */
    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
    nNumColors = DibNumColors(lpbi);

    if (nNumColors){
        /* Allocate for the logical palette structure */
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
        if (!pPal)
            return NULL;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        /* Fill in the palette entries from the DIB color table and
         * create a logical color palette.
         */
        for (i = 0; i < nNumColors; i++){
            pPal->palPalEntry[i].peRed   = pRgb[i].rgbRed;
            pPal->palPalEntry[i].peGreen = pRgb[i].rgbGreen;
            pPal->palPalEntry[i].peBlue  = pRgb[i].rgbBlue;
            pPal->palPalEntry[i].peFlags = (BYTE)0;
        }
        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
    }
    else if (lpbi->biBitCount == 24){
        /* A 24 bitcount DIB has no color table entries so, set the number of
         * to the maximum value (256).
         */
        nNumColors = MAXPALETTE;
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
        if (!pPal)
            return NULL;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        red = green = blue = 0;

        /* Generate 256 (= 8*8*4) RGB combinations to fill the palette
         * entries.
         */
        for (i = 0; i < pPal->palNumEntries; i++){
            pPal->palPalEntry[i].peRed   = red;
            pPal->palPalEntry[i].peGreen = green;
            pPal->palPalEntry[i].peBlue  = blue;
            pPal->palPalEntry[i].peFlags = (BYTE)0;

            if (!(red += 32))
                if (!(green += 32))
                    blue += 64;
        }
        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
    }
    return hpal;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : CreateDibPalette(HANDLE hbi)                               *
 *                                                                          *
 *  PURPOSE    : Given a Global HANDLE to a BITMAPINFO Struct               *
 *               will create a GDI palette object from the color table.     *
 *               (BITMAPINFOHEADER format DIBs only)                                     *
 *                                                                          *
 *  RETURNS    : A handle to the palette.                                   *
 *                                                                          *
 ****************************************************************************/
HPALETTE CreateDibPalette (HANDLE hbi)
{
    HPALETTE hpal;

    if (!hbi)
        return NULL;
    hpal = CreateBIPalette((LPBITMAPINFOHEADER)GlobalLock(hbi));
    GlobalUnlock(hbi);
    return hpal;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :OpenDIB(LPSTR szFile)                                       *
 *                                                                          *
 *  PURPOSE    :Open a DIB file and create a MEMORY DIB, a memory handle    *
 *              containing BITMAPINFO, palette data and the bits.           *
 *                                                                          *
 *  RETURNS    :A handle to the DIB.                                        *
 *                                                                          *
 ****************************************************************************/
HANDLE OpenDIB (LPSTR szFile)
{
    HFILE               fh;
    BITMAPINFOHEADER    bi;
    LPBITMAPINFOHEADER  lpbi;
    DWORD               dwLen = 0;
    DWORD               dwBits;
    HANDLE              hdib;
    HANDLE              h;
    OFSTRUCT            of;

    /* Open the file and read the DIB information */
    fh = OpenFile(szFile, &of, (UINT)OF_READ);
    if (fh == -1)
        return NULL;

    hdib = ReadDibBitmapInfo(fh);
    if (!hdib)
        return NULL;
    DibInfo(hdib,&bi);

    /* Calculate the memory needed to hold the DIB */
    dwBits = bi.biSizeImage;
    dwLen  = bi.biSize + (DWORD)PaletteSize (&bi) + dwBits;

    /* Try to increase the size of the bitmap info. buffer to hold the DIB */
    h = GlobalReAlloc(hdib, dwLen, GHND);
    if (!h){
        GlobalFree(hdib);
        hdib = NULL;
    }
    else
        hdib = h;

    /* Read in the bits */
    if (hdib){

        lpbi = (tagBITMAPINFOHEADER*)GlobalLock(hdib);
        lread(fh, (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi), dwBits);
        GlobalUnlock(hdib);
    }
    _lclose(fh);

    return hdib;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  PaletteSize(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  *
 *                is of the BITMAPCOREHEADER type, the number of colors is  *
 *                multiplied by 3 to give the palette size, otherwise the   *
 *                number of colors is multiplied by 4.                                                          *
 *                                                                          *
 *  RETURNS    :  Palette size in number of bytes.                          *
 *                                                                          *
 ****************************************************************************/
WORD PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DibNumColors(VOID FAR * pv)                                *
 *                                                                          *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *               the BitCount filed in the info block.                      *
 *                                                                          *
 *  RETURNS    : The number of colors in the DIB.                           *
 *                                                                          *
 ****************************************************************************/
WORD DibNumColors (VOID FAR * pv)
{
    INT                 bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    }
    else
        bits = lpbc->bcBitCount;

    switch (bits){
        case 1:
                return 2;
        case 4:
                return 16;
        case 8:
                return 256;
        default:
                /* A 24 bitcount DIB has no color table */
                return 0;
    }
}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : ReadBitMapFileHeaderandConvertToDwordAlign(HFILE fh, LPBITMAPFILEHEADER pbf)
 *                                                                          *
 *  PURPOSE    : read file header (which is packed) and convert into unpacked BITMAPFILEHEADER strucutre
 *                                                                          *
 *  RETURNS    : VOID
 *                                                                          *
 ****************************************************************************/

VOID ReadBitMapFileHeaderandConvertToDwordAlign(HFILE fh, LPBITMAPFILEHEADER pbf, LPDWORD lpdwoff)
{
        DWORD off;

        off = _llseek(fh, 0L, (UINT) SEEK_CUR);
        *lpdwoff = off;

/*              BITMAPFILEHEADER STRUCUTURE is as follows
 *              BITMAPFILEHEADER
 *              WORD    bfType
 >          ....                  <     add WORD if packed here!
 *              DWORD   bfSize
 *              WORD    bfReserved1
 *              WORD    bfReserved2
 *              DWORD   bfOffBits
 *                      This is the packed format, unpacked adds a WORD after bfType
 */

        /* read in bfType*/
        _lread(fh, (LPSTR) &pbf->bfType, sizeof(WORD));
        /* read in last 3 dwords*/
        _lread(fh, (LPSTR) &pbf->bfSize, sizeof(DWORD) * 3);

}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : ErrMsg (PSTR sz,...)                                       *
 *                                                                          *
 *  PURPOSE    : Opens a Message box with a error message in it.The user can*
 *               select the OK button to continue                           *
 *                                                                          *
 *  RETURNS    : FALSE to indicate an error has occured.                    *
 *                                                                          *
 ****************************************************************************/
INT ErrMsg (PSTR sz,...)
{
    CHAR ach[512];

    va_list args; 
	
    va_start(args, sz); 

    wvsprintf (ach, sz, args);   /* Format the string */
    MessageBox (NULL, ach, NULL, MB_OK|MB_ICONEXCLAMATION|MB_APPLMODAL);
    return FALSE;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DrawBitmap(HDC hdc, int x, int y, HBITMAP hbm, DWORD rop)  *
 *                                                                          *
 *  PURPOSE    : Draws bitmap <hbm> at the specifed position in DC <hdc>    *
 *                                                                          *
 *  RETURNS    : Return value of BitBlt()                                   *
 *                                                                          *
 ****************************************************************************/
BOOL DrawBitmap (
    HDC    hdc,
    INT    x,
    INT    y,
    HBITMAP    hbm,
    DWORD          rop)
{
    HDC       hdcBits;
    BITMAP    bm;
//    HPALETTE  hpalT;
    BOOL      f;

    if (!hdc || !hbm)
        return FALSE;

    // Made this code a little more robust
    // by checking the results of a few calls.
    // memory, esp on the mac, can get sucked
    // up really fast and make these fail
    // 2-9-94 Garth Brown
    //
    if(hdcBits = CreateCompatibleDC(hdc))
    {
       if(GetObject(hbm,sizeof(BITMAP),(LPSTR)&bm) != 0)
       {
          HGDIOBJ hObj;

          hObj = SelectObject(hdcBits,hbm);
          if ((hObj != NULL) && (hObj != (HGDIOBJ)GDI_ERROR))
          {
             f = BitBlt(hdc,x, y,bm.bmWidth,bm.bmHeight,hdcBits,0,0,rop);
          }
          else
          {
             // most likely to fail, so we'll put a message box here
             //
             MessageBox(NULL, "Select Object failed: out of memory?",
                        "Error Selecting Bitmap!", MB_OK);
             return(FALSE);
          }
       }
       DeleteDC(hdcBits);
    }

    return f;
        UNREFERENCED_PARAMETER(y);
        UNREFERENCED_PARAMETER(x);
}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DrawBitmap(HDC hdc, int x, int y, int width, int height, HBITMAP hbm, DWORD rop)  *
 *                                                                          *
 *  PURPOSE    : Draws bitmap <hbm> at the specifed position in DC <hdc>    *
 *               Andy's copy of DrawBitmap - scales or centres as needed    *
 *                                                                          *
 *  RETURNS    : Return value of BitBlt()                                   *
 *                                                                          *
 ****************************************************************************/
BOOL DrawBitmapScaled (
    HDC    hdc,
    INT    x,
    INT    y,
    INT    inWidth,
    INT    inHeight,
    HBITMAP    hbm,
    DWORD          rop)
{
    HDC       hdcBits;
    BITMAP    bm;
//    HPALETTE  hpalT;
    BOOL      f;

    if (!hdc || !hbm)
        return FALSE;

    // Made this code a little more robust
    // by checking the results of a few calls.
    // memory, esp on the mac, can get sucked
    // up really fast and make these fail
    // 2-9-94 Garth Brown
    //
    if(hdcBits = CreateCompatibleDC(hdc))
    {
       if(GetObject(hbm,sizeof(BITMAP),(LPSTR)&bm) != 0)
       {
          HGDIOBJ hObj;
					
					const int actualWidth = bm.bmWidth;
					const int actualHeight = bm.bmHeight;
					int drawHeight = actualHeight;
					int drawWidth = actualWidth;
					bool changedSize = false;
// check scaling according to widths
// calling logic in oofRepPictBlock and oofRepPictBand sets height
// from actual height
					if (actualWidth>inWidth) {  // scale down
						drawWidth = inWidth;
						drawHeight = actualHeight * inWidth/actualWidth;
						changedSize = true;
					}
					else if (actualWidth<inWidth) { // centre smaller picture horizontally, leave at top vertically
						x += (inWidth - actualWidth)/2;
					}
          hObj = SelectObject(hdcBits,hbm);
          if ((hObj != NULL) && (hObj != (HGDIOBJ)GDI_ERROR))
          {
             if (changedSize)
							 f = StretchBlt(hdc,x, y, drawWidth, drawHeight, hdcBits,0,0,actualWidth, actualHeight, rop);
						 else
							 f = BitBlt(hdc,x, y, drawWidth, drawHeight, hdcBits,0,0,rop);
          }
          else
          {
             // most likely to fail, so we'll put a message box here
             //
             MessageBox(NULL, "Select Object failed: out of memory?",
                        "Error Selecting Bitmap!", MB_OK);
             return(FALSE);
          }
       }
       DeleteDC(hdcBits);
    }

    return f;
        UNREFERENCED_PARAMETER(y);
        UNREFERENCED_PARAMETER(x);
}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : BitmapFromDib(HANDLE hdib, HPALETTE hpal)                  *
 *                                                                          *
 *  PURPOSE    : Will create a DDB (Device Dependent Bitmap) given a global *
 *               handle to a memory block in CF_DIB format                  *
 *                                                                          *
 *  RETURNS    : A handle to the DDB.                                       *
 *                                                                          *
 ****************************************************************************/
HBITMAP BitmapFromDib (
    HANDLE         hdib,
    HPALETTE   hpal)
{
    LPBITMAPINFOHEADER  lpbi;
    HPALETTE            hpalT;
    HDC                 hdc;
    HBITMAP             hbm;

//    StartWait();

    if (!hdib)
        return NULL;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    if (!lpbi)
        return NULL;

    hdc = GetDC(NULL);

    if (hpal){
        hpalT = SelectPalette(hdc,hpal,FALSE);
        RealizePalette(hdc);     // GDI Bug...????
    }

    hbm = CreateDIBitmap(hdc,
                (LPBITMAPINFOHEADER)lpbi,
                (LONG)CBM_INIT,
                (LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi),
                (LPBITMAPINFO)lpbi,
                DIB_RGB_COLORS );

    if (hpal)
        SelectPalette(hdc,hpalT,FALSE);

    ReleaseDC(NULL,hdc);
    GlobalUnlock(hdib);

//    EndWait();

    return hbm;
}


#endif  // _Windows
