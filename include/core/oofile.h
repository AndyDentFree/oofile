// oofile.h

// This is the only file you need to include in a user program to use the database
// if you are using the report-writer, you also need oofrep.h

// Note: if you relied on OOFILE in previous versions to include stream headers 
// defining ostringstream and cout etc. you now need to also #include "oofios.h"
#ifndef H_OOFFILE
#define H_OOFFILE

// basic OOFILE files
#ifndef H_OOF4
	#include "oof4.h"
#endif
#ifndef H_OOFQUERY
	#include "oofquery.h" 
#endif
#ifndef H_OOFREL
	#include "oofrel.h"
#endif
#ifndef H_OOFVIEW
	#include "oofview.h"
#endif
#ifndef H_OOFWORDS
	#include "oofwords.h"
#endif
#ifndef H_OOFMACRO
	#include "oofmacro.h"
#endif

// include stream libraries, which means you can't include this oofile.h in 
// a precompiled header
#ifndef H_OOFIOS
	#include "oofios.h"
#endif

// mainly used in demo progs
#ifndef EXIT_SUCCESS
  #define EXIT_SUCCESS 0
#endif


//****** UNCOMMENT ONE OR MORE OF THE FOLLOWING TO USE THAT BACKEND *******
// the following are included in OOFILE Professional server support, which includes Personal OOFILE ???? andy to fill
#ifndef H_OOFFServ
//	#include "ooffsrv.h" // Faircom's server
#endif

// the following are included in OOFILE Professional, which includes Personal OOFILE
#ifndef H_OOFctree
	#include "oofctree.h"   // Faircom's c-tree Plus
#endif

// the following are included in Personal OOFILE
#ifndef H_OOFDBASE
	#include "oofdbase.h"   // dBase III+ and IV
#endif
#ifndef H_OOFRAM
	#include "oofram.h"
#endif
#ifndef H_OOFramp
	#include "ooframp.h"    // RAM backend with persistence
#endif

#endif

