/* $Id: kList.cpp,v 1.1 1999-09-05 02:09:17 bird Exp $ */
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
        if (cEntries != 0)
            fprintf(stderr,
                    "%s(%d, %s)internal processing warning - cEntires was incorrect upon list destruction.",
                    __FILE__, __LINE__, __FUNCTION__);
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
#endif
