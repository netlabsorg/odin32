/* $Id: kLIFO.cpp,v 1.2 2002-02-24 02:47:27 bird Exp $ */
/*
 * Simple LIFO template class implementation.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

#ifndef _kLIFO_cpp_
#define _kLIFO_cpp_

/**
 * Creates an empty LIFO.
 */
template <class kEntry>
kLIFO<kEntry>::kLIFO(void) : pTop(NULL)
{
}


/**
 * Deletes entire LIFO.
 * @remark    calls destroy().
 */
template <class kEntry>
kLIFO<kEntry>::~kLIFO(void)
{
    destroy();
}


/**
 * Deletes all nodes in the LIFO.
 */
template <class kEntry>
void kLIFO<kEntry>::destroy(void)
{
    if (pTop)
    {
        #if 0
            /* recursive */
            delete pTop;
        #else
            /* iterative */
            kEntry *p1;
            p1 = pTop;
            while (p1 != NULL)
            {
                kEntry *p2;
                p2 = p1;
                p1 = (kEntry*)p1->getNext();
                p2->setNext(NULL);
                delete p2;
            }
        #endif
        pTop = NULL;
    }
}


/**
 * Push a node on top of the LIFO.
 * @param     pNewEntry  Entry to push onto the stack.
 */
template <class kEntry>
void kLIFO<kEntry>::push(kEntry *pNewEntry)
{
    pNewEntry->setNext(pTop);
    pTop = pNewEntry;
}


/**
 * Pop the topnode from the LIFO (if any)
 * @returns   Topentry of the LIFO. If empty LIFO, NULL.
 */
template <class kEntry>
kEntry *kLIFO<kEntry>::pop(void)
{
    if (pTop)
    {
        kEntry *pRetEntry = pTop;
        pTop = (kEntry*)pTop->getNext();
        pRetEntry->setNext(NULL);
        return pRetEntry;
    }
    else
        return NULL;
}


/**
 * Gets a given node from the lifo. The node does not have to be the top item.
 * @returns   Pointer to node. NULL on error.
 * @param     pGetEntry  Pointer to node to get.
 */
template <class kEntry>
kEntry *kLIFO<kEntry>::get(const kEntry *pGetEntry)
{
    kEntry *pPrev = NULL;
    kEntry *pRet = pTop;

    /* find */
    while (pRet != pGetEntry)
    {
        pPrev = pRet;
        pRet = (kEntry *)pRet->getNext();
    }

    /* unlink */
    if (pRet != NULL)
    {
        if (pPrev == NULL)
            pTop = (kEntry *)pRet->getNext();
        else
            pPrev->setNext(pRet->getNext());
    }

    return pRet;
}


/**
 * Unwinds the LIFO until a given node.
 * @param     pToEntry  Pointer to the new top node.
 */
template <class kEntry>
void  kLIFO<kEntry>::unwind(kEntry *pToEntry)
{
    if (pToEntry == NULL)
        return;

    while (pTop != pToEntry && pTop != NULL)
    {
        kEntry *pEntry = pTop;
        pTop = (kEntry*)pTop->getNext();
        delete pEntry;
    }
}


/**
 * Pops from this lifo and pushes the items onto lifoTo.
 * @param     pToEntry  Pointer to node to stop at. Will become the top node in this lifo.
 * @param     lifoTo    Receiving lifo.
 * @precond   pToEntry must exist in this lifo.
 */
template <class kEntry>
void kLIFO<kEntry>::popPush(const kEntry *pToEntry, kLIFO<kEntry> &lifoTo)
{
    if (pToEntry != NULL && exists(pToEntry))
        while (pTop != NULL && pTop != pToEntry)
            lifoTo.push(pop());
}


/**
 * Is the LIFO empty?
 * @returns   TRUE - empty, FALSE - not empty.
 */
template <class kEntry>
KBOOL kLIFO<kEntry>::isEmpty(void) const
{
    return pTop == NULL;
}


/**
 * Finds a node matching the given key.
 * @returns   Pointer to node if a match exists.
 * @param     pszKey  Pointer to key string.
 * @remark    uses the == operator of the nodes.
 */
template <class kEntry>
kEntry *kLIFO<kEntry>::find(const char *pszKey) const
{
    kEntry *pCurrentEntry = pTop;

    while (pCurrentEntry != NULL && !(*pCurrentEntry == pszKey))
        pCurrentEntry = (kEntry*)pCurrentEntry->getNext();

    return pCurrentEntry;
}


/**
 * Checks if a node exists in the LIFO.
 * @returns   TRUE if the node exists, FALSE if it don't.
 * @param     pEntry  Pointer to node.
 */
template <class kEntry>
KBOOL kLIFO<kEntry>::exists(const kEntry *pEntry) const
{
    kEntry *pCurrentEntry = pTop;

    while (pCurrentEntry != NULL && pCurrentEntry != pEntry)
        pCurrentEntry = (kEntry*)pCurrentEntry->getNext();

    return pCurrentEntry != NULL;
}

#endif
