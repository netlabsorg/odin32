/* $Id: taskmem.cpp,v 1.1 2001-04-26 19:26:15 sandervl Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * COM/OLE memory management functions.
 *
 * 7/7/99
 *
 * Copyright 1999 David J. Raison
 *
 * Some portions from Wine Implementation
 *	Copyright 1997	Marcus Meissner
 */

#include "ole32.h"

#include "oString.h"

// ======================================================================
// Prototypes.
// ======================================================================
static HRESULT WIN32API IMalloc_fnQueryInterface(LPMALLOC iface, REFIID refiid, LPVOID * obj) ;
static ULONG WIN32API IMalloc_fnAddRef(LPMALLOC iface) ;
static ULONG WIN32API IMalloc_fnRelease(LPMALLOC iface) ;
static LPVOID WIN32API IMalloc_fnAlloc(LPMALLOC iface, DWORD cb);
static LPVOID WIN32API IMalloc_fnRealloc(LPMALLOC iface, LPVOID pv, DWORD cb);
static VOID WIN32API IMalloc_fnFree(LPMALLOC iface, LPVOID pv);
static DWORD WIN32API IMalloc_fnGetSize(const IMalloc* iface, LPVOID pv);
static INT WIN32API IMalloc_fnDidAlloc(const IMalloc* iface, LPVOID pv);
static LPVOID WIN32API IMalloc_fnHeapMinimize(LPMALLOC iface);
static LPMALLOC IMalloc_Constructor();

// ======================================================================
// Local Data
// ======================================================================
typedef struct
{
        /* IUnknown fields */
        ICOM_VTABLE(IMalloc)* lpvtbl;
        DWORD                   ref;
} IMallocImpl;

static ICOM_VTABLE(IMalloc) VT_IMalloc =
{
    IMalloc_fnQueryInterface,
    IMalloc_fnAddRef,
    IMalloc_fnRelease,
    IMalloc_fnAlloc,
    IMalloc_fnRealloc,
    IMalloc_fnFree,
    IMalloc_fnGetSize,
    IMalloc_fnDidAlloc,
    IMalloc_fnHeapMinimize
};

LPMALLOC currentMalloc32 = NULL;

// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
// CoTaskMemAlloc
// ----------------------------------------------------------------------
LPVOID WIN32API CoTaskMemAlloc(ULONG cb)
{
    dprintf(("OLE32: CoTaskMemAlloc(%lu)", cb));

    return HeapAlloc(GetProcessHeap(), 0, cb);
}

// ----------------------------------------------------------------------
// CoTaskMemFree
// ----------------------------------------------------------------------
void WIN32API CoTaskMemFree(void *pv)
{
    dprintf(("OLE32: CoTaskMemFree"));

    HeapFree(GetProcessHeap(), 0, pv);
}

// ----------------------------------------------------------------------
// CoTaskMemRealloc
// ----------------------------------------------------------------------
LPVOID WIN32API CoTaskMemRealloc(LPVOID pv, ULONG cb)
{
    dprintf(("OLE32: CoTaskMemRealloc(%lu)", cb));

    return HeapReAlloc(GetProcessHeap(), 0, pv, cb);
}

// ----------------------------------------------------------------------
// CoGetMalloc
// ----------------------------------------------------------------------
HRESULT WIN32API CoGetMalloc(DWORD dwMemContext, LPMALLOC *ppMalloc)
{
    dprintf(("OLE32: CoGetMalloc"));

    if(!currentMalloc32)
	currentMalloc32 = IMalloc_Constructor();

    *ppMalloc = currentMalloc32;
    return S_OK;
}

// ----------------------------------------------------------------------
// IsValidInterface32
// ----------------------------------------------------------------------
BOOL WIN32API IsValidInterface( LPUNKNOWN punk)	/* [in] interface to be tested */
{
    dprintf(("OLE32: IsValidInterface"));

    return !(IsBadReadPtr(punk, 4)
        || IsBadReadPtr(ICOM_VTBL(punk), 4)
        || IsBadReadPtr(ICOM_VTBL(punk)->fnQueryInterface, 9)
        || IsBadCodePtr((FARPROC)ICOM_VTBL(punk)->fnQueryInterface) );
}

// ======================================================================
// IMalloc implementation
// ======================================================================

// ----------------------------------------------------------------------
// IMalloc_Constructor
// ----------------------------------------------------------------------
static LPMALLOC IMalloc_Constructor()
{
    IMallocImpl* This;

    This = (IMallocImpl*)HeapAlloc(GetProcessHeap(), 0, sizeof(IMallocImpl));
    This->lpvtbl = &VT_IMalloc;
    This->ref = 1;
    return (LPMALLOC)This;
}

// ----------------------------------------------------------------------
// IMalloc_fnQueryInterface
// ----------------------------------------------------------------------
static HRESULT WIN32API IMalloc_fnQueryInterface(LPMALLOC iface, REFIID refiid, LPVOID * obj)
{
    ICOM_THIS(IMallocImpl, iface);

    oStringA	tRefiid(refiid);
    dprintf(("OLE32: IMalloc_fnQueryInterface(%s)", (char *)tRefiid));

    if (!memcmp(&IID_IUnknown, refiid, sizeof(IID_IUnknown))
    	|| !memcmp(&IID_IMalloc, refiid, sizeof(IID_IMalloc)))
    {
	*obj = This;
	return S_OK;
    }
    return E_NOINTERFACE;
}

// ----------------------------------------------------------------------
// IMalloc_fnAddRef
// ----------------------------------------------------------------------
static ULONG WIN32API IMalloc_fnAddRef(LPMALLOC iface)
{
    ICOM_THIS(IMallocImpl, iface);

    dprintf(("OLE32: IMalloc_fnAddRef"));

    return 1; /* cannot be freed */
}

// ----------------------------------------------------------------------
// IMalloc_fnRelease
// ----------------------------------------------------------------------
static ULONG WIN32API IMalloc_fnRelease(LPMALLOC iface)
{
    ICOM_THIS(IMallocImpl, iface);

    dprintf(("OLE32: IMalloc_fnRelease"));

    return 1; /* cannot be freed */
}

// ----------------------------------------------------------------------
// IMalloc_fnAlloc
// ----------------------------------------------------------------------
static LPVOID WIN32API IMalloc_fnAlloc(LPMALLOC iface, DWORD cb)
{
    ICOM_THIS(IMallocImpl, iface);

    dprintf(("OLE32: IMalloc_fnAlloc"));

    return HeapAlloc(GetProcessHeap(), 0, cb);
}

// ----------------------------------------------------------------------
// IMalloc_fnRealloc
// ----------------------------------------------------------------------
static LPVOID WIN32API IMalloc_fnRealloc(LPMALLOC iface, LPVOID pv, DWORD cb)
{
    ICOM_THIS(IMallocImpl, iface);

    dprintf(("OLE32: IMalloc_fnRealloc"));

    return HeapReAlloc(GetProcessHeap(), 0, pv, cb);
}

// ----------------------------------------------------------------------
// IMalloc_fnFree
// ----------------------------------------------------------------------
static VOID WIN32API IMalloc_fnFree(LPMALLOC iface, LPVOID pv)
{
    ICOM_THIS(IMallocImpl, iface);

    dprintf(("OLE32: IMalloc_fnFree"));

    HeapFree(GetProcessHeap(), 0, pv);
}

// ----------------------------------------------------------------------
// IMalloc_fnGetSize
// ----------------------------------------------------------------------
static DWORD WIN32API IMalloc_fnGetSize(const IMalloc* iface, LPVOID pv)
{
    ICOM_THIS(IMallocImpl, iface);

    dprintf(("OLE32: IMalloc_fnGetSize"));

    return HeapSize(GetProcessHeap(), 0, pv);
}

// ----------------------------------------------------------------------
// IMalloc_fnDidAlloc
// ----------------------------------------------------------------------
static INT WIN32API IMalloc_fnDidAlloc(const IMalloc* iface, LPVOID pv)
{
    ICOM_THIS(IMallocImpl, iface);

    dprintf(("OLE32: IMalloc_fnDidAlloc"));

    return -1;
}

// ----------------------------------------------------------------------
// IMalloc_fnHeapMinimize
// ----------------------------------------------------------------------
static LPVOID WIN32API IMalloc_fnHeapMinimize(LPMALLOC iface)
{
    ICOM_THIS(IMallocImpl, iface);

    dprintf(("OLE32: IMalloc_fnHeapMinimize"));

    return NULL;
}

