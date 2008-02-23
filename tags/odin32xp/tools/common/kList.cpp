/* $Id: kList.cpp,v 1.4 2002-02-24 02:47:28 bird Exp $ */
/*
 * Simple list and sorted list template class.
 * Note: simple list is not implemented yet, as it is not yet needed.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _kList_cpp_
#define _kList_cpp_

#ifndef DEBUG
    #define DEBUG
#endif

/**
 * Constructor.
 */
template <class kEntry>
kSortedList<kEntry>::kSortedList() : pFirst(NULL), pLast(NULL), cEntries(0)
{
}


/**
 * Destructor.
 */
template <class kEntry>
kSortedList<kEntry>::~kSortedList()
{
    destroy();
}


/**
 * Removes all entries in the list.
 */
template <class kEntry>
void kSortedList<kEntry>::destroy()
{
    while (pFirst != NULL)
    {
        kEntry *p = pFirst;
        pFirst = (kEntry*)pFirst->getNext();
        delete p;
        #ifdef DEBUG
            cEntries--;
        #endif
    }
    #ifdef DEBUG
        kASSERT(cEntries == 0);
    #endif
    cEntries = 0;
    pLast = NULL;
}


/**
 * Inserts entry into the list.
 * @param     pEntry  Pointer to entry to insert.
 */
template <class kEntry>
void kSortedList<kEntry>::insert(kEntry *pEntry)
{
    if (pEntry == NULL)
        return;

    if (pFirst == NULL)
    {
        pEntry->setNext(NULL);
        pLast = pFirst = pEntry;
    }
    else if (*pLast <= *pEntry)
    {
        pEntry->setNext(NULL);
        pLast->setNext(pEntry);
        pLast = pEntry;
    }
    else
    {
        kEntry *pPrev = NULL;
        kEntry *p = pFirst;
        while (p != NULL && *p < *pEntry)
        {
            pPrev = p;
            p = (kEntry*)p->getNext();
        }

        assert(p != NULL);

        pEntry->setNext(p);
        if (pPrev != NULL)
            pPrev->setNext(pEntry);
        else
            pFirst = pEntry;
    }
    cEntries++;
}


/**
 * Get an element from the list without removing it.
 * Also see function remove(...)
 * @returns    pointer to matching object. NULL if not found.
 * @param      entry  Reference to match object.
 * @remark
 */
template <class kEntry>
kEntry *kSortedList<kEntry>::get(const kEntry &entry) const
{
    kEntry *p = pFirst;
    while (p != NULL && *p < entry)
        p = (kEntry*)p->getNext();

    return p != NULL && *p == entry ? p : NULL;
}


/**
 * Get first element from the list without removing it.
 * @returns    pointer to matching object. NULL if empty list.
 * @remark
 */
template <class kEntry>
kEntry *kSortedList<kEntry>::getFirst(void) const
{

    return pFirst;
}


/**
 * Get first element from the list without removing it.
 * @returns    pointer to matching object. NULL if empty list.
 * @remark
 */
template <class kEntry>
kEntry *kSortedList<kEntry>::getLast(void) const
{
    return pLast;
}


/**
 * Gets count of entries in the list.
 * @returns   Entry count.
 */
template <class kEntry>
unsigned long kSortedList<kEntry>::getCount(void) const
{
    return cEntries;
}






/**
 * Constructor.
 */
template <class kEntry>
kList<kEntry>::kList() : pFirst(NULL), pLast(NULL), cEntries(0)
{
}


/**
 * Destructor.
 */
template <class kEntry>
kList<kEntry>::~kList()
{
    destroy();
}


/**
 * Removes all entries in the list.
 */
template <class kEntry>
void kList<kEntry>::destroy()
{
    while (pFirst != NULL)
    {
        kEntry *p = pFirst;
        pFirst = (kEntry*)pFirst->getNext();
        delete p;
        #ifdef DEBUG
            cEntries--;
        #endif
    }
    #ifdef DEBUG
        kASSERT(cEntries == 0);
    #endif
    cEntries = 0;
    pLast = NULL;
}


/**
 * Inserts an entry into the end of the list.
 * @param     pEntry  Pointer to entry to insert.
 */
template <class kEntry>
void kList<kEntry>::insert(kEntry *pEntry)
{
    if (pEntry == NULL)
        return;

    if (pFirst == NULL)
    {
        pEntry->setNext(NULL);
        pLast = pFirst = pEntry;
    }
    else
    {
        pEntry->setNext(NULL);
        pLast->setNext(pEntry);
        pLast = pEntry;
    }
    cEntries++;
}

#if 0
/**
 * Inserts an entry into the list in an ascending sorted fashion.
 * @param   pEntry  Pointer to entry to insert.
 */
template <class kEntry>
void kList<kEntry>::insertSorted(kEntry *pEntry)
{
    if (pEntry == NULL)
        return;
    if (pFirst == NULL)
    {
        pEntry->setNext(NULL);
        pLast = pFirst = pEntry;
    }
    else
    {   /* linear search */
        kEntry *pCur = pFirst;

        pCur->

        pEntry->setNext(NULL);
        pLast->setNext(pEntry);
        pLast = pEntry;
    }
    cEntries++;
}
#endif


/**
 * Get first element from the list without removing it.
 * @returns    pointer to matching object. NULL if empty list.
 * @remark
 */
template <class kEntry>
kEntry *kList<kEntry>::getFirst(void) const
{
    return pFirst;
}


/**
 * Get first element from the list without removing it.
 * @returns    pointer to matching object. NULL if empty list.
 * @remark
 */
template <class kEntry>
kEntry *kList<kEntry>::getLast(void) const
{
    return pLast;
}


/**
 * Gets count of entries in the list.
 * @returns   Entry count.
 */
template <class kEntry>
unsigned long kList<kEntry>::getCount(void) const
{
    return cEntries;
}


#endif
