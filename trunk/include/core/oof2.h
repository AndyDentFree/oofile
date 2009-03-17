#ifndef H_OOF2
#define H_OOF2

// COPYRIGHT 1994 A.D. Software, All rights reserved

// backend parent layer of OOFILE database
// this header just contains various useful classes for databases
// mainly used internally

// NOTE inline definitions included at end of this header file

#ifndef H_OOF1
	#include "oof1.h"
#endif
#ifndef H_OOFARRAY
	#include "oofarray.h"
#endif

// forward declarations
class OOF_bit;
class dbField;
class dbBLOB;
class dbTable;
class OOF_simpleRecordBackend;

class dbRelRefBase;
class dbRelationship;


/**
	abstract base for handling record contexts
	\ingroup oofDatabase
*/
class OOFILE_EXPORT dbContext 
{
public:
	virtual ~dbContext() {};
};


/**
	Array of longs which expands automatically with refcount so easily shared.
	\ingroup oofUtils
*/
class OOFILE_EXPORT OOF_SharedArray : public OOF_ExpandableLongArray,  public OOF_mixRefCount {	
public:
	OOF_SharedArray(unsigned long defaultValue=0, unsigned long numSlots=0, unsigned int expandBySlots=4);
};


/**
	Array of dummy name strings including generator function.
	Used solely by OOF_DummyString and can't be instantiated by others.
	\ingroup oofUtils
*/
class OOFILE_EXPORT OOF_DummyStorage
{
private:	// yes, wholly owned by OOF_DummyString and not used elsewhere!
	OOF_DummyStorage();
public:	// g++ won't let a static have private dtor
	~OOF_DummyStorage();
	
private:
	void ReleaseAllStorage();  // to be called by OOF_DummyString
	void SetPrefix(const OOF_String&);
	const OOF_String& Dummy(unsigned long);
	void DeleteAllStrings();

// data storage	
	OOF_ExpandableLongArray	mStringCache;
	OOF_String	mPrefix;
	
friend class OOF_DummyString;
};



/**
	statically stored self-incrementing const char*'s
	\ingroup oofUtils
*/
class OOFILE_EXPORT OOF_DummyString : public oofString {
public:
// constructors
	OOF_DummyString(const char* dummyPrefix=0);

// getters	
	const OOF_String& nextDummy(); 

	static void releaseAllStorage();

protected:
// data storage
	unsigned long mSuffix;
	static OOF_DummyStorage sStrings;
};
typedef OOF_DummyString oofDummyString; // for external use


/**
	abstract base for public interface to selections.	
	\ingroup oofInternals
*/
class OOFILE_EXPORT  OOF_Selection {  
public:
	virtual ~OOF_Selection() {};
  virtual OOF_Selection* clone() const=0;
	
/// \name combinatorial operators 
//@{
  virtual void difference_with(const OOF_Selection*)=0;
  virtual void intersection_with(const OOF_Selection*)=0;
  virtual void intersection_with(oidT)=0;
  virtual void union_with(const OOF_Selection*)=0;
  virtual void union_with_no_overlap(const OOF_Selection*)=0;
  virtual void union_with(oidT)=0;
  virtual void invert()=0;
//@}

/// \name reflective operators  
//@{
	virtual bool isEmpty() const=0;
	virtual bool contains(oidT) const=0;
	virtual bool find(oidT, unsigned long& foundAt) const=0;
	virtual unsigned long count() const=0;
	virtual bool isAllRecs() const=0;
	virtual const dbTable* prototypicalTable() const=0;
//@}
};


// -------------------------------------------------------
//                    d b S e l e c t i o n
// -------------------------------------------------------
inline void 
dbSelection::difference_with(dbTable* rhs)
{
	difference_with(*rhs);
}


inline void 
dbSelection::operator -=(const dbSelection& rhs)
{
	difference_with(rhs);
}


inline void 
dbSelection::operator -=(dbTable& rhs)
{
	difference_with(rhs);
}


inline void 
dbSelection::operator -=(dbTable* rhs)
{
	difference_with(*rhs);
}


inline void 
dbSelection::operator %=(dbTable& rhs)
{
	difference_with(rhs);
}


inline void 
dbSelection::operator %=(const dbSelection& rhs)
{
	difference_with(rhs);
}


inline void 
dbSelection::operator %=(dbTable* rhs)
{
	difference_with(*rhs);
}


inline void 
dbSelection::operator &=(const dbSelection& rhs)
{
	intersection_with(rhs);
}


inline void 
dbSelection::intersection_with(dbTable* rhs)
{
	intersection_with(*rhs);
}


inline void 
dbSelection::operator &=(dbTable& rhs)
{
	intersection_with(rhs);
}


inline void 
dbSelection::operator &=(dbTable* rhs)
{
	intersection_with(*rhs);
}


inline void 
dbSelection::operator +=(const dbSelection& rhs)
{
	union_with(rhs);
}


inline void 
dbSelection::operator |=(const dbSelection& rhs)
{
	union_with(rhs);
}


inline void 
dbSelection::union_with(dbTable* rhs)
{
	union_with(*rhs);
}


inline void 
dbSelection::union_with_no_overlap(dbTable* rhs)
{
	union_with_no_overlap(*rhs);
}


inline void 
dbSelection::operator +=(dbTable& rhs)
{
	union_with(rhs);
}


inline void 
dbSelection::operator +=(dbTable* rhs)
{
	union_with(*rhs);
}


inline void 
dbSelection::operator |=(dbTable& rhs)
{
	union_with(rhs);
}


inline void 
dbSelection::operator |=(dbTable* rhs)
{
	union_with(*rhs);
}


inline void 
dbSelection::operator+=(oidT offset)
{
	union_with(offset);
}


inline void 
dbSelection::operator~()
{
	invert();
}


inline void 
dbSelection::operator!()
{
	invert();
}


inline const OOF_Selection* 
dbSelection::internalSelection() const
{
	return mSelection;
}


inline const dbTable* 
dbSelection::prototypicalTable() const
{
	return mSelection->prototypicalTable();
}

#endif

