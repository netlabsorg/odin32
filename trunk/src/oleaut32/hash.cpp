/* $Id: hash.cpp,v 1.2 2001-08-10 19:36:04 sandervl Exp $ */
/* 
 * LHash functions.
 * 
 * 2000/01/03
 * 
 * Copyright 2000 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Based on Knuth - Sorting & Searching 2nd Ed. 1997 Section 6.4
 */

#include "oleaut32.h"
#include "olectl.h"
#include "oList.h"	// linked list template
//#include "itypelib.h"

// ======================================================================
// Local Data
// ======================================================================
class HashElem
{
public:
    HashElem(LCID lcid, const WCHAR * szName) : m_lcid(lcid)
	{
	    m_szName = new WCHAR[lstrlenW(szName) + 1];
	    lstrcpyW(m_szName, szName);
	}
    virtual ~HashElem()
	{ delete[] m_szName; }

    LCID		lcid() const
	{ return m_lcid; }
    WCHAR *		szName() const
	{ return m_szName; }

    int	compare (HashElem & rhs)
	{
	    if (rhs.lcid() > m_lcid)
		return 1;
	    else if (rhs.lcid() < m_lcid)
		return -1;

	    return lstrcmpW(rhs.szName(), m_szName);
	}

protected:

private:
    LCID		m_lcid;
    WCHAR *		m_szName;

};

class Hash
{
public:
    Hash(ULONG size = 1009);
    virtual ~Hash();

    ULONG	Compute(LCID lcid, const OLECHAR * szName);

protected:

private:
    USHORT	GetBaseVal(HashElem * pElem);
    USHORT	Lookup(HashElem * pElem);

    ULONG		m_lSize;
    oList<HashElem *> *	m_pTable;

};

static	Hash *	m_pHash;

// ======================================================================
// Local functions.
// ======================================================================

// ----------------------------------------------------------------------
// Hash::Hash
//
// Constructor
// ----------------------------------------------------------------------
Hash::Hash(ULONG size)
    : m_lSize(size)
{
    m_pTable = new oList<HashElem *>[m_lSize];
}

// ----------------------------------------------------------------------
// Hash::~Hash
//
// Destructor
// ----------------------------------------------------------------------
Hash::~Hash()
{
    ULONG	ii;

    for (ii = 0; ii < m_lSize; ii++)
    {
	oListIter<HashElem *>	pIter(m_pTable[ii]);

	for (pIter.MoveStart(); pIter.IsValid(); pIter.MoveNext())
	    delete pIter.Element();

    }

    delete[] m_pTable;
    m_pTable = 0;
}

// ----------------------------------------------------------------------
// Hash::GetBaseVal
//
// Return a 16bit hash value for the string.
// ----------------------------------------------------------------------
USHORT	Hash::GetBaseVal(HashElem * pElem)
{
    USHORT		usVal = 0;
    const WCHAR *	pName = pElem->szName();

    while(*pName)
    {
    	usVal ^= (int)towupper(*pName);
    	usVal <<= 1;
	pName++;
    }

    return usVal % m_lSize;
}

// ----------------------------------------------------------------------
// Hash::Lookup
// ----------------------------------------------------------------------
USHORT	Hash::Lookup(HashElem * pElem)
{
    USHORT			usVal = GetBaseVal(pElem);
    oListIter<HashElem *>	pIter(m_pTable[usVal]);

    for (pIter.MoveStart(); pIter.IsValid(); pIter.MoveNext())
	if (pElem->compare(*pIter.Element()) == 0)
	    return usVal;

    m_pTable[usVal].AddAtEnd(pElem);
    return usVal;
}

// ----------------------------------------------------------------------
// Hash::Compute
// ----------------------------------------------------------------------
ULONG    Hash::Compute(LCID lcid, const OLECHAR * szName)
{
    HashElem *		pElem = new HashElem(lcid, szName);
    return Lookup(pElem) | (lcid << 16);
}

// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
// Hash_Initialise
// ----------------------------------------------------------------------
void Hash_Initialise()
{
    m_pHash = new Hash;
}

// ----------------------------------------------------------------------
// Hash_Terminate
// ----------------------------------------------------------------------
void Hash_Terminate()
{
    delete m_pHash;
}

// ----------------------------------------------------------------------
// LHashValOfNameSys()    				[OLEAUT32.165]
//
// Return a hash to identify an OLE typelib element name.
// ----------------------------------------------------------------------
ULONG WIN32API LHashValOfNameSys(SYSKIND syskind, LCID lcid, const OLECHAR * szName)
{
    ULONG	rc;

    dprintf(("OLEAUT32: LHashValOfNameSys"));

    rc = m_pHash->Compute(lcid, szName);

    return rc;
}

// ----------------------------------------------------------------------
// LHashValOfNameSysA()    				[OLEAUT32.166]
//
// Return a hash to identify an OLE typelib element name.
// ----------------------------------------------------------------------
ULONG WIN32API LHashValOfNameSysA (SYSKIND syskind, LCID lcid, LPCSTR szName)
{
    LPWSTR      szWName;
    HANDLE	hHeap = GetProcessHeap();
    ULONG	rc;

    szWName = (LPWSTR)HeapAlloc(hHeap, 0, (strlen(szName) + 1) * 2);
    AsciiToUnicode(szName, szWName);
    rc = LHashValOfNameSys(syskind, lcid, szWName);
    HeapFree(hHeap, 0, szWName);

    return rc;
}

