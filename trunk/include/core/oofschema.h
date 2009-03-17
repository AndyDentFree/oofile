#ifndef H_OOFSchema
#define H_OOFSchema

#ifndef H_OOFFILES
	#include "ooffiles.h"
#endif

#ifndef H_OOFMACRO
	#include "oofmacro.h"
#endif


// OOFILE  schema inport and connect
// 
// COPYRIGHT 2000 A.D. Software, All rights reserved

// forward defs
class unResolvedRelationship;

/**
	Class of David's.
	\todo remove class when add OOFILE_METHODS to dbTableOwningFields
	\ingroup experimental
*/
class dbTableOwningFieldsSOoFSchema : public dbTableOwningFields {
	OOFILE_METHODS(dbTableOwningFieldsSOoFSchema)
	dbTableOwningFieldsSOoFSchema(const char* name=0) :  
		dbTableOwningFields(name)
	{};  
};


/**
	Holds a relationship that has been found by the schema parser, but has yet to be resolved and instantiated.
	\sa oofSchemaParser
	\todo rename with standard oof or OOF_prefix
	\ingroup experimental
*/
 class relationshipsToInstantiate{
 
 	public:
 		//relationshipsToInstantiate();
 		~relationshipsToInstantiate();
 		
		//~relationshipsToInstantiate();
 		//unResolvedRelationship(oofString rhsFieldName, dbField *lhsField);
 		void append(unResolvedRelationship *);
 		void findMatch(const oofString& tableName, const oofString& fieldName);
 		void	resolveRelationships();
 		void 		rhsTableName(const oofString& );
 		//oofString	rhsTableName();
 		//oofString 	rhsFieldName();// do not think i need this
 		dbField * 	lhsField();
 		unsigned long mNumber;
 		bool		isSet();
 		bool operator==(const unResolvedRelationship& inRhs);
 	
 	private:
 		void	findMatch(const oofString& tableName, fieldNumT fieldNum);
 		
 		OOF_ExpandableLongArray mList;
 	
 		dbField * mLhsField;
 		bool mIsSet;
 };


/**
	This class parses a saved schema and builds an oofile database schema in a connection.
	Exists to allow connection to databases without fore knowledge of the schema.
	When loading a schema, it will examine the file and attempt to figure out the ooffile connection type
	which created the file and open it accordingly.
	\todo Use neater encoding rather than relying on describe() output.
	\ingroup experimental
*/
class  oofSchemaParser{
public:
	// use default ctor, dtor, copy ctor
	void parseSchemaText(const oofString&/*, oofRepTextStyle**/);
	void parseSchemaText();
	void loadSchema(oofFileRef &schemaFile);
	void loadSchema(const oofString &schemaFileName);
	//void loadSchema(oofString &inFile, oofString& ext, oofString& iExt,dbConnect_ctree &theConnection);
	//void	loadSchema(const oofString &schemaFile, /*const oofString& ext, const oofString& iExt,*/dbConnect_ctree &theConnection);
	//void	loadSchema(const oofString &schemaFile, /*const oofString& ext, const oofString& iExt,*/dbConnect_ctree &theConnection);

	bool isOOFSchema(oofFileRef &schemaFile);
	
	#ifdef _Macintosh
		//	loadSchema(const FSSpec&);
	#endif
	//void loadSchema(oofFileRef schemaFile)
	
	//bool isOofReptxtStyle(const oofString&);
	
private:
	
	enum eRelType{e1_1,eM_1,eN_1,e1_N,eM_N,eN_N,eRelError};
	
	void		MakeTable(const oofString &tableString);
	oofString 	FindTableString();
	oofString 	FindTableName(const oofString &TableString);
	
	dbField*	makeField(const oofString &oofString);
	oofString 	FindFieldString(const oofString &TableString);
	oofString	FindFieldName(const oofString &fieldString);
	oofString  FindJoinFieldName(const oofString &fieldString);
	oofString 	FindTypeString(const oofString &fieldString);
	OOF_fieldTypes FindType(const oofString &typeString);
	oofString	FindIndexString(const oofString &FieldString);
	OOF_IndexOptions ParseIndexType(const oofString &indexString);
	eRelType 	FindRelType(const oofString &fieldString);
	unsigned long FindFieldLength(const oofString &indexString);
	
	bool 		FieldIsIndexed(const oofString &fieldString);
	bool 		IsJoinField(const oofString &fieldString);
	bool 	  	IsRel(const oofString &fieldString);
	bool		PropagatesDeletes( const oofString &fieldString);

	oofString		FindRelationship();
	oofString 		FindTable();


// data storage
	char* mEndOfTable;	
	long mCurrentPos; // init at 0
	long nextTable;
	long mCurrentFieldPos;
	//stateParse;
	bool mTableFound;
	bool mFieldFound;
	bool mFieldHasLength;
	relationshipsToInstantiate mUnResolvedRels;
	
protected:
	oofString mParseBuffer;	 

	//bool mNoTableFound;
	
	//unsigned long mFieldLength;
};


/** 
	A container for a list of relationshipsToInstantiate.
	\sa relationshipsToInstantiate
	\ingroup experimental
*/
class unResolvedRelationship{
 
 	public:
 		//unResolvedRelationship(oofString rhstableName,oofString rhsFieldName, dbRelRefBase *lhsField, dbField JoinField, bool isSet);
 		//unResolvedRelationship(oofString rhsTableName,oofString rhsFieldName, dbRelRefBase *lhsField, bool isSet);
 		//unResolvedRelationship(oofString rhsFieldName, dbField *lhsField);
 		unResolvedRelationship(const oofString rhstableName,oofString joinFieldName, dbRelRefBase *lhsField, bool isSet);

 		void	rhsTableName(const oofString& );
 		const oofString&	rhsTableName();
 		const oofString& 	rhsFieldName();
 		dbRelRefBase *rhsField();
 		void		rhsField(dbRelRefBase*);
 		dbRelRefBase * 	lhsField();
 		void resolve();
 		bool		isSet();
 		bool		isLhs();
 		void		setIsRhs();
 		bool		isRhs();
 		//bool operator==(const unResolvedRelationship& inRhs);
 		//void 	resolve();
 	private:
 		oofString mRhsTableName;
 		oofString mRhsFieldName;
 		//oofString dbRelationship;
		oofString mJoinFieldName;
		dbRelRefBase * mLhsField;
 		dbRelRefBase * mRhsField;
 		
		dbField *mLhsJoinField;
		dbField *mRhsJoinField;
		
		unsigned long mNumber;
 		bool mIsLhs;
 		bool mIsSet;
	
 };
 
 
/* class relationshipsToInstantiate{
 
 	public:
 		relationshipsToInstantiate();
 		~relationshipsToInstantiate();
 		
		//~relationshipsToInstantiate();
 		//unResolvedRelationship(oofString rhsFieldName, dbField *lhsField);
 		void apphend(unResolvedRelationship *);
 		void findMatch(const oofString& tableName, const oofString& fieldName);
 		void	resolveRelationships();
 		void 		rhsTableName(const oofString& );
 		//oofString	rhsTableName();
 		//oofString 	rhsFieldName();// do not think i need this
 		dbField * 	lhsField();
 		unsigned long mNumber;
 		bool		isSet();
 		bool operator==(const unResolvedRelationship& inRhs);
 	
 	private:
 		void	findMatch(const oofString& tableName, fieldNumT fieldNum);
 		
 		OOF_ExpandableLongArray mList;
 	
 		dbField * mLhsField;
 		bool mIsSet;
 };*/

#endif
