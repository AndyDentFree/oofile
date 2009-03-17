// COPYRIGHT 1994 A.D. Software, All rights reserved
#ifndef H_OOFLIST
#define H_OOFLIST

#ifndef H_OOF0
	#include "oof0.h"
#endif

#ifdef OOF_DEBUG
	 #include <assert.h>
#endif

/**
	\file ooflist.h 
	Defines varying list types because we run on older compilers instead of templating lists.
	(yes, even in 2001) some really old ones do not provide templates.
	
	Partitioning lists are used for nested sorts - OOFILE provides sorting
	down to arbitrary levels of nesting so you can have expressions on many
	nested sort fields.
	
	\ingroup oofUtils
*/

/**
	Defines a singly linked list of	list elements.
	Normally each of which points to a single item on the list.

	By using the "Sorted" functions, the list can be kept in sorted
	in increasing order by "key" in ListElement.
	\ingroup oofUtils
*/
class OOF_ListBase {
  public:
    OOF_ListBase():
    	mCount(0) {};				// initialize the list
// use default copy ctor
    virtual ~OOF_ListBase() {
#ifdef OOF_DEBUG
	 assert(!mCount); // make sure there are no elements
#endif
	 };		// It's our descendant's responsibility !

    bool isEmpty() { return (mCount==0); };			// is the list empty?
	
	unsigned long count() { return mCount; };	// Return the number of items
		
	virtual unsigned long remove() = 0;
	
  protected:
		unsigned long	mCount;			// number of items
};


/**
	Elements used in OOF_ListBase.
	\ingroup oofUtils
*/
class OOF_ListElement {
   public:
     OOF_ListElement(unsigned long theItem):
     		mNext(0),
     		mItem(theItem) {};	// initialize a list element
// use default copy ctor
     ~OOF_ListElement() {};

     OOF_ListElement *mNext;		// next element on list, 
																// NULL if this is the last
     unsigned long mItem; 	    // item on the list
};


/**
	Default list.
	\ingroup oofUtils
*/
class OOF_List : public OOF_ListBase {
  public:
    OOF_List():
    	OOF_ListBase(),
    	mFirst(0),
    	mLast(0) {};			// initialize the list
// use default copy ctor
    virtual ~OOF_List() { while (mCount) remove(); };

    void prepend(unsigned long theItem); 	///< Put item at the beginning of the list
    void append(unsigned long theItem);		///< Put item at the end of the list
    virtual unsigned long remove(); 	 		///< Take item off the front of the list
	
	bool member(unsigned long theItem) const;		///< Is theItem in the list
    
    void sortedInsert(unsigned long theItem);	  			///< Put item into list sorted
    void sortedInsertNoDups(unsigned long theItem);	  ///< Put item into list sorted only if not already there
	
  private:
    OOF_ListElement *mFirst;  ///< Head of the list, NULL if list is empty // owned - special case as linked list of owned items
    OOF_ListElement *mLast;		///< Last element of list
};


/**
	Sortable list element for one unsigned long.
	\todo document use and comparison of list element types
	\ingroup oofUtils
*/
class OOF_ULSorterElement {
   public:
     OOF_ULSorterElement(unsigned long theItem, unsigned long theData):
     		mNext(0),
     		mItem(theItem),
     		mData(theData) {};	// initialize a list element
// use default copy ctor
     ~OOF_ULSorterElement() {};

     OOF_ULSorterElement *mNext;		// next element on list, 
																				// NULL if this is the last
     unsigned long mItem; 	   				 	// item on the list
     unsigned long mData;								// 4-byte list container
};


/**
	Sortable list of unsigned longs.
	\ingroup oofUtils
*/
class OOF_ULSorter : public OOF_ListBase {
  public:
    OOF_ULSorter():
    	OOF_ListBase(),
    	mFirst(0),
    	mLast(0),
    	mLastInsertEqual(0),
    	mEqualInserted(0) {};			// initialize the list
// use default copy ctor
	virtual ~OOF_ULSorter() { while (mCount) remove(); };
	
    virtual unsigned long remove(); ///< Take item off the front of the list
    																///< NOTE: This now returns the container data and not the Key

    void sortedInsert(unsigned long theItem, unsigned long theData);
    void sortedInsertReverse(unsigned long theItem, unsigned long theData);
    													  			///< Put item into list sorted
    													  			
    // Special Members for the string sorting stuff !
    bool lastEqual() { return mLastInsertEqual; };
    inline OOF_ULSorterElement *theEqual() { return mEqualInserted; };
    void exchange(OOF_ULSorterElement *theItem);
	
  private:
    OOF_ULSorterElement *mFirst;  // Head of the list, NULL if list is empty // owned - special case as linked list of owned items
    OOF_ULSorterElement *mLast;		// Last element of list
    
    // Special Members for the string sorting stuff !
    bool mLastInsertEqual;
    OOF_ULSorterElement *mEqualInserted;
};



/**
	Element for sortable list of longs.
	\ingroup oofUtils
*/
class OOF_LSorterElement {
   public:
     OOF_LSorterElement(long theItem, unsigned long theData):
     		mNext(0),
     		mItem(theItem),
     		mData(theData) {};	// initialize a list element
// use default copy ctor
     	~OOF_LSorterElement() {};

     OOF_LSorterElement *mNext;		// next element on list, 
																	// NULL if this is the last
     long mItem;		 	   				 	// item on the list
     unsigned long mData;								// 4-byte list container
};


/**
	Sortable list of longs.
	\ingroup oofUtils
*/
class OOF_LSorter : public OOF_ListBase {
  public:
    OOF_LSorter():
    	OOF_ListBase(),
    	mFirst(0),
    	mLast(0) {};			// initialize the list
// use default copy ctor
	virtual ~OOF_LSorter() { while (mCount) remove(); };

    virtual unsigned long remove(); // Take item off the front of the list
    																// NOTE: This now returns the container data and not the Key

    void sortedInsert(long theItem, unsigned long theData);
    void sortedInsertReverse(long theItem, unsigned long theData);
    													  			// Put item into list sorted
	
  private:
    OOF_LSorterElement *mFirst;  // Head of the list, NULL if list is empty // owned - special case as linked list of owned items
    OOF_LSorterElement *mLast;		// Last element of list
};


/**
	Element for sortable list of doubles.
	\ingroup oofUtils
*/
class OOF_DSorterElement {
   public:
     OOF_DSorterElement(double theItem, unsigned long theData):
     		mNext(0),
     		mItem(theItem),
     		mData(theData) {};	// initialize a list element
// use default copy ctor
     	~OOF_DSorterElement() {};

     OOF_DSorterElement *mNext;		// next element on list, 
																				// NULL if this is the last
     double mItem; 	   				 	// item on the list
     unsigned long mData;								// 4-byte list container
};


/**
	Sortable list of doubles.
	\ingroup oofUtils
*/
class OOF_DSorter : public OOF_ListBase {
  public:
    OOF_DSorter():
    	OOF_ListBase(),
    	mFirst(0),
    	mLast(0) {};			// initialize the list
// use default copy ctor
	virtual ~OOF_DSorter() { while (mCount) remove(); };

    virtual unsigned long remove(); // Take item off the front of the list
    																// NOTE: This now returns the container data and not the Key

    void sortedInsert(double theItem, unsigned long theData);
    void sortedInsertReverse(double theItem, unsigned long theData);
    													  			// Put item into list sorted
	
  private:
    OOF_DSorterElement *mFirst;  // Head of the list, NULL if list is empty // owned - special case as linked list of owned items
    OOF_DSorterElement *mLast;		// Last element of list
};


//--- Partitioning Sorters to cope with nested sorts ---//

/**
	Element for partitionable, sortable list of unsigned longs.
	\ingroup oofUtils
*/
class OOF_ULPSorterElement {
   public:
     OOF_ULPSorterElement(unsigned long theItem, unsigned long theData):
     		mNext(0),
     		mItem(theItem),
     		mData(theData),
			mPart(0) {};	// initialize a list element
// use default copy ctor
     ~OOF_ULPSorterElement() {};

     OOF_ULPSorterElement *mNext;		// next element on list, 
																				// NULL if this is the last
     unsigned long mItem; 	   				 	// item on the list
     unsigned long mData;						// 4-byte list container
     unsigned short mPart;						// Partition Counter
};


/**
	Sortable list of unsigned longs with partitioning for nested sorts.
	\ingroup oofUtils
*/
class OOF_ULPSorter : public OOF_ListBase {
  public:
    OOF_ULPSorter():
    	OOF_ListBase(),
    	mFirst(0),
    	mLast(0),
    	mLastInsertEqual(0),
    	mEqualInserted(0) {};			// initialize the list
// use default copy ctor
	virtual ~OOF_ULPSorter() { while (mCount) remove(); };
	
    virtual unsigned long remove(); // Take item off the front of the list
    																// NOTE: This now returns the container data and not the Key

    void sortedInsert(unsigned long theItem, unsigned long theData);
    void sortedInsertReverse(unsigned long theItem, unsigned long theData);
    													  			// Put item into list sorted
    													  			
    // Special Members for the string sorting stuff !
    bool lastEqual() { return mLastInsertEqual; };
    inline OOF_ULPSorterElement *theEqual() { return mEqualInserted; };
    inline OOF_ULPSorterElement *theFirst() { return mFirst; };
    void exchange(OOF_ULPSorterElement *theItem);
	
  private:
    OOF_ULPSorterElement *mFirst;  // Head of the list, NULL if list is empty // owned - special case as linked list of owned items
    OOF_ULPSorterElement *mLast;		// Last element of list
    
    // Special Members for the string sorting stuff !
    bool mLastInsertEqual;
    OOF_ULPSorterElement *mEqualInserted;
};



/**
	Element for partitionable, sortable list of longs.
	\ingroup oofUtils
*/
class OOF_LPSorterElement {
   public:
     OOF_LPSorterElement(long theItem, unsigned long theData):
     		mNext(0),
     		mItem(theItem),
     		mData(theData),
			mPart(0) {};	// initialize a list element
// use default copy ctor
     	~OOF_LPSorterElement() {};

     OOF_LPSorterElement *mNext;		// next element on list, 
																	// NULL if this is the last
     long mItem;		 	   				 	// item on the list
     unsigned long mData;								// 4-byte list container
     unsigned short mPart;						// Partition Counter
};


/**
	Sortable list of longs with partitioning for nested sorts.
	\ingroup oofUtils
*/
class OOF_LPSorter : public OOF_ListBase {
  public:
    OOF_LPSorter():
    	OOF_ListBase(),
    	mFirst(0),
    	mLast(0) {};			// initialize the list
// use default copy ctor
	virtual ~OOF_LPSorter() { while (mCount) remove(); };

    virtual unsigned long remove(); // Take item off the front of the list
    																// NOTE: This now returns the container data and not the Key
    inline OOF_LPSorterElement *theFirst() { return mFirst; };

    void sortedInsert(long theItem, unsigned long theData);
    void sortedInsertReverse(long theItem, unsigned long theData);
    													  			// Put item into list sorted
	
  private:
    OOF_LPSorterElement *mFirst;  // Head of the list, NULL if list is empty // owned - special case as linked list of owned items
    OOF_LPSorterElement *mLast;		// Last element of list
};



/**
	Sortable list of doubles with partitioning for nested sorts.
	\ingroup oofUtils
*/


class OOF_DPSorterElement {
   public:
     OOF_DPSorterElement(double theItem, unsigned long theData):
     		mNext(0),
     		mItem(theItem),
     		mData(theData),
			mPart(0) {};	// initialize a list element
// use default copy ctor
     	~OOF_DPSorterElement() {};

     OOF_DPSorterElement *mNext;		// next element on list, 
																				// NULL if this is the last
     double mItem; 	   				 	// item on the list
     unsigned long mData;								// 4-byte list container
     unsigned short mPart;						// Partition Counter
};


/**
	Sortable list of doubles with partitioning for nested sorts.
	\ingroup oofUtils
*/
class OOF_DPSorter : public OOF_ListBase {
  public:
    OOF_DPSorter():
    	OOF_ListBase(),
    	mFirst(0),
    	mLast(0) {};			// initialize the list
// use default copy ctor
	virtual ~OOF_DPSorter() { while (mCount) remove(); };

    virtual unsigned long remove(); // Take item off the front of the list
    																// NOTE: This now returns the container data and not the Key
    inline OOF_DPSorterElement *theFirst() { return mFirst; };

    void sortedInsert(double theItem, unsigned long theData);
    void sortedInsertReverse(double theItem, unsigned long theData);
    													  			// Put item into list sorted
	
  private:
    OOF_DPSorterElement *mFirst;  // Head of the list, NULL if list is empty // owned - special case as linked list of owned items
    OOF_DPSorterElement *mLast;		// Last element of list
};

#endif


