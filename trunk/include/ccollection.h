/* $Id: ccollection.h,v 1.5 2001-10-01 01:14:42 bird Exp $ */

/*
 * Collection class:
 *   provides very fast object lookup by index number
 *
 * Copyright 2001 Patrick Haller <patrick.haller@innotek.de>
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 *
 */


#ifndef _CCOLLECTION_H_
#define _CCOLLECTION_H_


#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <odin.h>                       /* Watcom needs max/min... */

class CCollection
{
    protected:
        int iSize; /* current size of the structure in elements */

    public:
        CCollection(int iInitialSize = 0);
        ~CCollection();

        virtual void clear() = 0; // remove all elements from the table
                int  getSize() { return iSize; } // query size of structure
};


typedef struct
{
    void* pObject;
} INDEXLOOKUPENTRY, *PINDEXLOOKUPENTRY;


class CIndexLookup : public CCollection
{
    public:
        CIndexLookup(int iInitialSize = 0);
        ~CIndexLookup();

        void* addElement(int iIndex, void *pObject);
        void* removeElement(int iIndex);
        void* getElement(int iIndex);
        int   isValidIndex(int iIndex);
        void  clear();
        int   shrink();

    protected:

        /* ensure the specified index can be held in the table */
        virtual int ensureCapacity(int iIndex);
        PINDEXLOOKUPENTRY reallocateData(int iShift, int iRequiredSize);

        /* pointer to array with data entries */
        PINDEXLOOKUPENTRY pEntries;

        /* offset is subtracted from any index into the table */
        /* this also is the lowest valid index in the table */
        int iOffset;
        int iUsedOffset;

        /* this is the highest valid index in the table */
        int iHigh;
        int iUsedHigh;

        /* are the iUsedOffset and iUserHigh initialized? */
        int iInitialized;
};


class CIndexLookupLimit : public CIndexLookup
{
    public:
        CIndexLookupLimit(int iHardLimitLow = 0,
                          int iHardLimitHigh = 0);
        ~CIndexLookupLimit();


    protected:

        /* ensure the specified index can be held in the table */
        int ensureCapacity(int iIndex);

        /* hard limits for the array */
        int iLimitLow;
        int iLimitHigh;
};


typedef struct tagLinearListEntry
{
    struct tagLinearListEntry *pNext;
    struct tagLinearListEntry *pPrev;

    void *pObject;
} LINEARLISTENTRY, *PLINEARLISTENTRY;


class CLinearList : public CCollection
{
    public:
        CLinearList();
        ~CLinearList();

        PLINEARLISTENTRY addFirst  (void *pObject);
        PLINEARLISTENTRY addLast   (void *pObject);
        PLINEARLISTENTRY addBefore (PLINEARLISTENTRY pLLE, void *pObject);
        PLINEARLISTENTRY addAfter  (PLINEARLISTENTRY pLLE, void *pObject);
        void             removeElement(PLINEARLISTENTRY pLLE);
        int              removeElement(void *pObject);
        PLINEARLISTENTRY findForward(void *pObject);
        PLINEARLISTENTRY findForward(PLINEARLISTENTRY pLLECurrent,
                                     void *pObject);
        PLINEARLISTENTRY findBackward(void *pObject);
        PLINEARLISTENTRY findBackward(PLINEARLISTENTRY pLLECurrent,
                                      void *pObject);
        PLINEARLISTENTRY getFirst(void) { return pFirst; }
        PLINEARLISTENTRY getLast(void)  { return pLast;  }
        PLINEARLISTENTRY getNext(PLINEARLISTENTRY pCurrent)
                                        { return pCurrent->pNext; }
        PLINEARLISTENTRY getPrevious(PLINEARLISTENTRY pCurrent)
                                        { return pCurrent->pPrev; }
        void             clear();

    protected:
        PLINEARLISTENTRY add0      (void *pObject);

        PLINEARLISTENTRY pFirst;
        PLINEARLISTENTRY pLast;
};


typedef struct
{
    char* pszName;
    void* pObject;
} HASHTABLEENTRY, *PHASHTABLEENTRY;


class CHashtableLookup : public CCollection
{
    public:
        CHashtableLookup(int iInitialSize = 23);
        ~CHashtableLookup();

        PHASHTABLEENTRY addElement(char *pszName, void *pObject);
        void* removeElement(char *pszName);
        void* getElement(char *pszName);
        void  clear();
        void  rehash();
        void  setSize(int iNewSize);

    protected:
        void          setSize0(int iNewSize);
        unsigned long nextPrime(unsigned long x);

        // the array of linear lists
        CLinearList** parrLists;

        // actual number of allocated elements
        int iElements;
};


#endif /* _CCOLLECTION_H_ */
