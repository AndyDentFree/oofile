// OOFILE c-tree Plus backend server support implementation
// 
// COPYRIGHT 2000 A.D. Software, All rights reserved


#include "oofpch_c.h"  // for precompilation of core files


//===========================================================================
//					user
//===========================================================================
#ifndef H_OOFctreeX
	#include "oofctrex.h"
#endif

#ifndef H_OOFdbUser
	#include "dbUser.h"
#endif
#ifndef H_OOFEXCEP
	#include "oofexcep.h"
#endif
#include "oofios.h"	// an easy way to get your stream libraries, regardless of platform


dbUser::dbUser(const char* userID,const char* psWord,const char * serverName)
{
	ID(userID);
	passWord(psWord);
	mServerName = serverName;
} 


const char* 
dbUser::ID()
{
	 return mUserID;
} 



void 
dbUser::ID(const char* theID)
{
	mUserID = theID;
	if(mUserID.length() > 32){
		oofE_dbUser err("User password must be less than or equal to 32 characters",eIDToLong);
		RAISE_EXCEPTION(err);
	}	
}

const char* 
dbUser::passWord()
{
 	return mUserPassword;
}
 

void
dbUser::passWord(const char* passWord)
{
	mUserPassword = passWord;
	if(mUserPassword.length() > 10){
		oofE_dbUser err("User password must be less than or equal to 10 characters",ePasWDToLong);
		RAISE_EXCEPTION(err);
	}	
}



const char* // should this be an inline ?
dbUser::serverName()
{
 	return mServerName;
}
 

void // and this
dbUser::serverName(const char*servName)
{
	mServerName = servName;
}


int 
dbUser::maxPassWordLen()
{
	return 10;
}

int 
dbUser::maxIDLen()
{
	return 10;
}
