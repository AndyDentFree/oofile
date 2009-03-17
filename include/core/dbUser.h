// OOFILE c-tree Plus backend server support 
// 
// COPYRIGHT 2000 A.D. Software, All rights reserved

#ifndef H_OOFdbUser
#define H_OOFdbUser
#ifndef H_OOFSTR
	#include "oofstr.h"
#endif

/**
	Specify a user for databases that require logging in.
	\ingroup oofDatabase
*/
class dbUser{
 public:
 enum userError {eIDToLong,ePasWDToLong,BadID,badServerName,badPassword};
 dbUser(const char* userID,const char*PassWord,const char*serverName);
 void ID(const char*);
 const char* ID();
 void passWord(const char*);
 const char* passWord();

 void serverName(const char*);
 const char* serverName();
 virtual int maxPassWordLen();
 virtual int maxIDLen();
	

private:
	oofString mUserID;
	oofString mUserPassword;
	oofString mServerName;
	

};

#endif
	
	
