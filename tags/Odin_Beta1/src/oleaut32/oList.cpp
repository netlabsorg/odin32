/* $Id: oList.cpp,v 1.2 2000-08-02 16:28:19 bird Exp $ */
/*
 * oList.cpp
 * ---------
 * Simple double linked list class
 */

#include "oleaut32.h"
#include "olectl.h"
#include "oList.h"	// linked list template

// ----------------------------------------------------------------------
// _oListElem::InsertBefore
// ----------------------------------------------------------------------
void _oListElem::InsertBefore(_oList * pList, _oListElem * pSibling)
{
    m_pList = pList;

    if (pSibling)	// Add before nominated...
    {
	m_pPrev = pSibling->m_pPrev;
	m_pNext = pSibling;
    }
    else		// add to start...
    {
	m_pPrev = 0;
	m_pNext = m_pList->m_pStart;
    }

    if ( m_pPrev )
	m_pPrev->m_pNext = this;
    else
	m_pList->m_pStart = this;

    if ( m_pNext )
	m_pNext->m_pPrev = this;
    else
	m_pList->m_pEnd = this;

    m_pList->m_ulCount += 1;
}

// ----------------------------------------------------------------------
// _oListElem::InsertAfter
// ----------------------------------------------------------------------
void _oListElem::InsertAfter(_oList * pList, _oListElem * pSibling)
{
    m_pList = pList;

    if (pSibling)	// Add after nominated...
    {
	m_pPrev = pSibling;
	m_pNext = pSibling->m_pNext;
    }
    else		// add to end...
    {
	m_pPrev = m_pList->m_pEnd;
	m_pNext = 0;
    }

    if ( m_pPrev )
	m_pPrev->m_pNext = this;
    else
	m_pList->m_pStart = this;

    if ( m_pNext )
	m_pNext->m_pPrev = this;
    else
	m_pList->m_pEnd = this;

    m_pList->m_ulCount += 1;
}

// ----------------------------------------------------------------------
// _oListElem::Remove
// ----------------------------------------------------------------------
void _oListElem::Remove()
{
    if ( m_pPrev )
	m_pPrev->m_pNext = m_pNext;
    else
	m_pList->m_pStart = m_pNext;

    if ( m_pNext )
	m_pNext->m_pPrev = m_pPrev;
    else
	m_pList->m_pEnd = m_pPrev;

    m_pList->m_ulCount--;

    m_pList = 0;
    m_pPrev = 0;
    m_pNext = 0;
}

// ----------------------------------------------------------------------
// _oList::locate
// ----------------------------------------------------------------------
_oListElem * _oList::locate(ULONG index) const
{
    _oListElem *	pElem;
    ULONG		ii;

    // Wow - This so efficient ;-) [NOT!]
    for(ii = 0, pElem = m_pStart; pElem != 0; ii += 1, pElem = pElem->m_pNext)
    {
	if (ii == index)
	    return pElem;
    }

    return 0;
}

