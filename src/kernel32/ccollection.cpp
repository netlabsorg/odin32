/* $Id: ccollection.cpp,v 1.4 2001-05-30 13:02:55 phaller Exp $ */

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

#include <ccollection.h>



static inline unsigned long _Optlink hashcode(int iRing, char *pszName)
{
    unsigned long h;
    unsigned char *t = (unsigned char*)pszName;

    for (h = 0;
         *t;
         t++)
    {
        h = (h << 6);
        h += *t;
        h %= iRing;
    }

    return h;
}



CCollection::CCollection(int iInitialSize)
{
    iSize = iInitialSize;
}

CCollection::~CCollection()
{
}


CIndexLookup::CIndexLookup(int iInitialSize)
{
    // iSize is expected to be set by parent constructor

    // allocate array with given size
    if (iInitialSize > 0)
    {
        pEntries = (PINDEXLOOKUPENTRY)malloc(iInitialSize * sizeof(INDEXLOOKUPENTRY));
        memset(pEntries,
               0,
               iInitialSize * sizeof(INDEXLOOKUPENTRY));
    }
    else
        // no initial array provided
        pEntries = NULL;

    iOffset = INT_MAX;
    iHigh = INT_MIN;
    iUsedOffset = INT_MAX;
    iUsedHigh = INT_MIN;
}


CIndexLookup::~CIndexLookup()
{
    free(pEntries);
}


int CIndexLookup::isValidIndex(int iIndex)
{
    /* determine if the given index is within bounds */
    return ( (iIndex >= iOffset) &&
             (iIndex <= iHigh) );
}


int CIndexLookup::shrink()
{
    // shrink the index array to the absolutely necessary size only

    // verify if shrink can do anything good
    if ( (iOffset < iUsedOffset) ||
         (iHigh   > iUsedHigh) )
    {
        // OK, reallocate the beast
        int iRequiredSize = iUsedHigh - iUsedOffset + 1;

        PINDEXLOOKUPENTRY pNewData = (PINDEXLOOKUPENTRY)malloc(iRequiredSize * sizeof(INDEXLOOKUPENTRY));
        if (NULL == pNewData)
        {
            /* signal failure */
            return 0;
        }

        // copy old data and write new boundary info
        if (NULL != pEntries)
        {
            int iRelative = iUsedOffset - iOffset;

            memcpy(pNewData,
                   pEntries + (iRelative * sizeof(INDEXLOOKUPENTRY)),
                   iRequiredSize * sizeof(INDEXLOOKUPENTRY));
    
            // deferred delete ensures we've always got a valid array
            PINDEXLOOKUPENTRY pTemp = pEntries;
            pEntries = pNewData;
            free(pTemp);
        }
        else
        {
            // set new pointer and we're done
            memset(pNewData,
                   0,
                   iRequiredSize * sizeof(INDEXLOOKUPENTRY));

            pEntries = pNewData;
        }

        iSize = iRequiredSize;
        iOffset = iUsedOffset;
        iHigh = iUsedHigh;

        return 1; // shrunk
    }
    else
        return 0; // didn't do anything
}


PINDEXLOOKUPENTRY CIndexLookup::reallocateData(int iShift, int iRequiredSize)
{
    // Note: assumption is iRequiredSize is > iSize!
    // if (iRequiredSize < iSize)
    //   return NULL;

    PINDEXLOOKUPENTRY pNewData = (PINDEXLOOKUPENTRY)malloc(iRequiredSize * sizeof(INDEXLOOKUPENTRY));
    if (NULL == pNewData)
    {
        /* signal failure */
        return NULL;
    }

    // copy current data
    if (pEntries != NULL)
    {
        // copy the data, array cannot overlap
        memmove(pNewData,
                pEntries,
                iSize);

        // zero out at the beginning or at the end
        if (iShift == 0)
        {
            // clear trailing
            memset(pNewData + sizeof(INDEXLOOKUPENTRY) * iSize,
                   0,
                   sizeof(INDEXLOOKUPENTRY) * (iRequiredSize - iSize));
        }
        else
        {
            // clear leading
            memset(pNewData,
                   0,
                   sizeof(INDEXLOOKUPENTRY) * (iShift));
        }

    }
    else
        // clear brand-new array
        memset(pNewData,
               0,
               iRequiredSize * sizeof(INDEXLOOKUPENTRY));


    return pNewData;
}


int CIndexLookup::ensureCapacity(int iIndex)
{
    // we need to resize if the index is not valid
    if (!isValidIndex(iIndex))
    {
        // calculate new array bounds,
        // copy original table,
        // swap table pointers
        // and free original pointer

        // check if new base offset is lower
        if (iIndex < iOffset)
        {
            // insert space at the front

            // calculate new size
            int iRequiredSize = iHigh - iIndex + 1;

            // reallocate the array
            int iShift = iOffset - iIndex; // for how many elements to shift left

            PINDEXLOOKUPENTRY pNewData = reallocateData(iShift, iRequiredSize);
            if (NULL == pNewData)
                // signal failure
                return 0;

            // deferred delete ensures we've always got a valid array
            PINDEXLOOKUPENTRY pTemp = pEntries;
            pEntries = pNewData;
            free(pTemp);

            iSize = iRequiredSize;
            iOffset = iIndex;
        }
        else
            if (iIndex > iHigh)
            {
                // lengthen the array

                // calculate new size
                int iRequiredSize = iIndex - iOffset + 1;

                // reallocate the array
                PINDEXLOOKUPENTRY pNewData = reallocateData(0, iRequiredSize);
                if (NULL == pNewData)
                    // signal failure
                    return 0;

                // deferred delete ensures we've always got a valid array
                PINDEXLOOKUPENTRY pTemp = pEntries;
                pEntries = pNewData;
                free(pTemp);

                iSize = iRequiredSize;
                iHigh = iIndex;
            }
            else
                // internal logic flaw!
                return 0;
    }

    return 1; /* OK */
}


void* CIndexLookup::addElement(int iIndex, void* pObject)
{
    // ensure the array can hold the entry
    if (ensureCapacity(iIndex))
    {
        // update the used low and high marks
        if (iIndex < iUsedOffset)
            iUsedOffset = iIndex;

        if (iIndex > iUsedHigh)
            iUsedHigh = iIndex;

        // insert entry and quit
        pEntries[iIndex - iOffset].pObject = pObject;
        return pObject;
    }
    else
    {
        // signal failure
        return NULL;
    }
}


void* CIndexLookup::removeElement(int iIndex)
{
    // check if index is within table
    if (isValidIndex(iIndex))
    {
        // remove specified element from the array
        void *pOld = pEntries[iIndex - iOffset].pObject;
        pEntries[iIndex - iOffset].pObject = NULL;

        return pOld;
    }
    else
    {
        // signal failure
        return NULL;
    }
}


void CIndexLookup::clear(void)
{
    free(pEntries);
    pEntries = NULL;
    iOffset = INT_MAX;
    iHigh = INT_MIN;
    iSize = 0;
    iUsedOffset = INT_MAX;
    iUsedHigh = INT_MIN;
}


void* CIndexLookup::getElement(int iIndex)
{
    if (isValidIndex(iIndex))
    {
        // OK, return entry
        return pEntries[iIndex - iOffset].pObject;
    }
    else
    {
        // signal: not found
        return NULL;
    }
}



CIndexLookupLimit::CIndexLookupLimit(int iHardLimitLow,
                                     int iHardLimitHigh)
  : CIndexLookup(0)
{
    // iSize is expected to be set by parent constructor

    // enable hard limits
    iLimitLow = min(iHardLimitLow, iHardLimitHigh);
    iLimitHigh = max (iHardLimitLow, iHardLimitHigh);

    // size the array
    iOffset = iLimitLow;
    iHigh = iLimitHigh;
    iSize = iHigh - iOffset + 1;
    pEntries = reallocateData(0, iSize);
}


CIndexLookupLimit::~CIndexLookupLimit()
{
}


int CIndexLookupLimit::ensureCapacity(int iIndex)
{
    // verify against hard limits if enabled
    // prevent growing under the low limit
    if (iIndex < iLimitLow)
        return 0;

    // prevent growing above the high limit
    if (iIndex > iLimitHigh)
        return 0;


    return CIndexLookup::ensureCapacity(iIndex);
}


CLinearList::CLinearList()
  : CCollection(0)
{
    pFirst = NULL;
    pLast  = NULL;
}


CLinearList::~CLinearList()
{
    clear();
}


PLINEARLISTENTRY CLinearList::addFirst(void* pObject)
{
    if (pFirst)
        return addBefore(pFirst, pObject);
    else
        return add0(pObject);
}


PLINEARLISTENTRY CLinearList::addLast(void* pObject)
{
    if (pLast)
        return addAfter(pLast, pObject);
    else
        return add0(pObject);
}


// special internal entry to add the 1st element
PLINEARLISTENTRY CLinearList::add0 (void *pObject)
{
    // allocate new entry
    PLINEARLISTENTRY pLLENew = (PLINEARLISTENTRY)malloc(sizeof(LINEARLISTENTRY));
    if (NULL == pLLENew)
        // signal failure
        return NULL;

    // setup object
    pLLENew->pObject = pObject;
    pLLENew->pPrev = NULL;
    pLLENew->pNext = NULL;

    pFirst = pLLENew;
    pLast  = pLLENew;

    // count items
    iSize = 1;

    return pLLENew;
}



PLINEARLISTENTRY CLinearList::addBefore (PLINEARLISTENTRY pLLE, void *pObject)
{
    // allocate new entry
    PLINEARLISTENTRY pLLENew = (PLINEARLISTENTRY)malloc(sizeof(LINEARLISTENTRY));
    if (NULL == pLLENew)
        // signal failure
        return NULL;

    // setup object
    pLLENew->pObject = pObject;
    pLLENew->pPrev = pLLE->pPrev;
    pLLENew->pNext = pLLE;
    pLLE->pPrev = pLLENew;

    // establish linking, append to end of list
    if (pFirst == pLLE)
        pFirst = pLLENew;

    // count items
    iSize++;

    return pLLENew;
}


PLINEARLISTENTRY CLinearList::addAfter(PLINEARLISTENTRY pLLE, void *pObject)
{
    // allocate new entry
    PLINEARLISTENTRY pLLENew = (PLINEARLISTENTRY)malloc(sizeof(LINEARLISTENTRY));
    if (NULL == pLLENew)
        // signal failure
        return NULL;

    // setup object
    pLLENew->pObject = pObject;
    pLLENew->pNext = pLLE->pNext;
    pLLENew->pPrev = pLLE;
    pLLE->pNext = pLLENew;

    // establish linking, append to end of list
    if (pLast  == pLLE)
        pLast  = pLLENew;

    // count items
    iSize++;

    return pLLENew;
}



PLINEARLISTENTRY CLinearList::findForward(void *pObject)
{
    return findForward(pFirst, pObject);
}


PLINEARLISTENTRY CLinearList::findBackward(void *pObject)
{
    return findBackward(pLast, pObject);
}


PLINEARLISTENTRY CLinearList::findForward(PLINEARLISTENTRY pLLECurrent,
                                          void *pObject)
{
    while(pLLECurrent)
    {
        // check current item
        if (pLLECurrent->pObject == pObject)
            return pLLECurrent;

        pLLECurrent = pLLECurrent->pNext;
    }

    // signal not found
    return NULL;
}


PLINEARLISTENTRY CLinearList::findBackward(PLINEARLISTENTRY pLLECurrent,
                                           void *pObject)
{
    while(pLLECurrent)
    {
        // check current item
        if (pLLECurrent->pObject == pObject)
            return pLLECurrent;

        pLLECurrent = pLLECurrent->pPrev;
    }

    // signal not found
    return NULL;
}


void CLinearList::removeElement(PLINEARLISTENTRY pLLE)
{
    // check if we're removing from the head
    if (pLLE == pFirst)
        pFirst = pLLE->pNext;

    // check if we're removing from the tail
    if (pLLE == pLast)
        pLast = pLLE->pPrev;

    // remove from chain
    if (pLLE->pPrev != NULL)
        pLLE->pPrev->pNext = pLLE->pNext;

    if (pLLE->pNext != NULL)
        pLLE->pNext->pPrev = pLLE->pPrev;

    // free memory of the pLLE
    free(pLLE);

    // count elements
    iSize--;
}


int CLinearList::removeElement(void* pObject)
{
    PLINEARLISTENTRY pLLE = findForward(pObject);
    if (pLLE == NULL)
        // signal not found
        return 0;
    else
    {
        removeElement(pLLE);
        return 1;
    }
}


void CLinearList::clear()
{
    PLINEARLISTENTRY pLLE = pFirst;
    PLINEARLISTENTRY pLLENext;

    // Establish new head and tail pointers very quickly,
    // since new items can be added already while the old ones
    // are still being deleted without causing interference.
    pFirst = NULL;
    pLast = NULL;

    while (pLLE)
    {
        pLLENext = pLLE->pNext;
        free (pLLE);
        pLLE = pLLENext;
    }
}


CHashtableLookup::CHashtableLookup(int iInitialSize)
  : CCollection(iInitialSize)
{
    // the parent constructor is expected to set iSize
    // to iInitialSize

    parrLists = new CLinearList* [iSize];
    memset(parrLists,
           0,
           iSize * sizeof(CLinearList*) );

    iElements = 0;
}

CHashtableLookup::~CHashtableLookup()
{
    // kill all the slot lists
    for (int i = 0;
         i < iSize;
         i++)
    {
        // check if slot was occupied
        if (parrLists[i] != NULL)
        {
            delete parrLists[i];
            parrLists[i] = NULL;
        }
    }

    iSize = 0;
    iElements = 0;

    delete [] parrLists;
}


PHASHTABLEENTRY CHashtableLookup::addElement(char *pszName, void *pObject)
{
    // get slot number
    unsigned long ulHash = hashcode(iSize, pszName);

    // create entry
    PHASHTABLEENTRY pHTE = (PHASHTABLEENTRY)malloc(sizeof(HASHTABLEENTRY));
    pHTE->pszName = pszName;
    pHTE->pObject = pObject;

    // check if slot has a list object already
    if (parrLists[ulHash] == NULL)
        parrLists[ulHash] = new CLinearList();

    parrLists[ulHash]->addLast(pHTE);

    // count allocated elements
    iElements++;

    return pHTE;
}


void* CHashtableLookup::removeElement(char *pszName)
{
    // get slot number
    unsigned long ulHash = hashcode(iSize, pszName);

    // check if slot is occupied
    if (parrLists[ulHash] == NULL)
        // signal not found
        return NULL;

    // search the list
    PLINEARLISTENTRY pLLE = parrLists[ulHash]->getFirst();
    if (pLLE == NULL)
        // signal not found
        return NULL;

    // iterate over the list
    while(pLLE)
    {
        PHASHTABLEENTRY pHTE = (PHASHTABLEENTRY)pLLE->pObject;

        // quickly compare 1st character for equality
        // before doing the strcmp call
        if (*pHTE->pszName == *pszName)
            if (strcmp(pHTE->pszName, pszName) == 0)
            {
                // save old object pointer
                void* pTemp = pHTE->pObject;
                free(pHTE);

                // found the correct entry
                parrLists[ulHash]->removeElement(pLLE);

                // count allocated elements
                iElements--;

                // return old object pointer to signal success
                return pTemp;
            }

        pLLE = pLLE->pNext;
    }

    // failed
    return NULL;
}


void* CHashtableLookup::getElement(char *pszName)
{
    // get slot number
    unsigned long ulHash = hashcode(iSize, pszName);

    CLinearList *pLL = parrLists[ulHash];

    // check if slot is occupied
    if (pLL == NULL)
        // signal not found
        return NULL;

    // search the list
    PLINEARLISTENTRY pLLE = pLL->getFirst();
    if (pLLE == NULL)
        // signal not found
        return NULL;

    // even if this is the only element on the list,
    // we cannot save the strcmp, since the pszName-key
    // might still be different! (mismatch)

    // iterate over the list
    while(pLLE)
    {
        PHASHTABLEENTRY pHTE = (PHASHTABLEENTRY)pLLE->pObject;

        // quickly compare 1st character for equality
        // before doing the strcmp call
        if (*pHTE->pszName == *pszName)
            if (strcmp(pHTE->pszName, pszName) == 0)
            {
                // return result
                return pHTE->pObject;
            }

        pLLE = pLLE->pNext;
    }

    // failed
    return NULL;
}


void  CHashtableLookup::clear()
{
    // clear all the slot lists
    for (int i = 0;
         i < iSize;
         i++)
    {
        // check if slot was occupied
        if (parrLists[i] != NULL)
            parrLists[i]->clear();
    }

    iSize = 0;
    iElements = 0;
}


void  CHashtableLookup::rehash()
{
    // if there are less slots than elements,
    // the blocking factor is expected to be high
    setSize(iElements);
}


void  CHashtableLookup::setSize(int iNewSize)
{
    // determine number of allocated elements
    // actually, we need the prime next to
    // the given number.
    if (iSize < iNewSize)
        setSize0(nextPrime(iNewSize));
}


void  CHashtableLookup::setSize0(int iNewSize)
{
    // check if rehashing is necessary at all
    if (iSize == iNewSize)
        return;

    // save old array, allocate new array
    CLinearList** parrNew = new CLinearList* [iNewSize];
    memset(parrNew,
           0,
           sizeof(CLinearList*) * iNewSize);

    // convert all the slot lists
    for (int i = 0;
         i < iSize;
         i++)
    {
        // check if slot was occupied
        if (parrLists[i] != NULL)
        {
            // iterate over the slot
            PLINEARLISTENTRY pLLE = parrLists[i]->getFirst();
            PHASHTABLEENTRY pHTE;
            unsigned long ulHash;

            while(pLLE)
            {
                // calculate new hashcode for the entry
                pHTE = (PHASHTABLEENTRY)pLLE->pObject;
                ulHash = hashcode(iNewSize, pHTE->pszName);

                // reinsert the pHTE into new slot
                if (parrNew[ulHash] == NULL)
                    parrNew[ulHash] = new CLinearList();

                parrNew[ulHash]->addLast( (void*)pHTE );

                pLLE=pLLE->pNext;
            }

            // kill the slot
            delete parrLists[i];
            parrLists[i] = NULL;
        }
    }

    // swap the tables
    iSize = iNewSize;
    delete [] parrLists;

    parrLists = parrNew;
}


unsigned long CHashtableLookup::nextPrime(unsigned long x)
{
   // Return next prime number after x, unless x is a prime in which case
   // x is returned.

   if (x < 2)
       return 2;

   if (x == 3)
      return 3;

   if (x % 2 == 0)
      x++;

   unsigned long sqr = (unsigned long) sqrt((double)x) + 1;

   for (;;)
   {
       unsigned long n;

       for (n = 3;
            (n <= sqr) && ((x % n) != 0);
            n += 2)
           ;

      if (n > sqr)
          return x;

      x += 2;
   }
}

