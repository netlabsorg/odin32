/* $Id: iunknown.cpp,v 1.1 2001-04-26 19:26:11 sandervl Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * ClassID Manipulation.
 * 
 * 18/7/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Some portions from Wine Implementation
 *   Copyright 1997 Marcus Meissner
 */

#include "ole32.h"

#include "oString.h"

// ======================================================================
// Local Data
// ======================================================================

typedef struct
{
    ICOM_VTABLE(IUnknown)* lpvtbl;
    DWORD                  ref;
} IUnknownImpl;

static ULONG	WIN32API IUnknownImpl_AddRef(LPUNKNOWN iface);
static ULONG	WIN32API IUnknownImpl_Release(LPUNKNOWN iface);
static HRESULT	WIN32API IUnknownImpl_QueryInterface(LPUNKNOWN iface,
				REFIID riid, LPVOID * ppvObject);

static ICOM_VTABLE(IUnknown) IUnknownVt = 
{
    IUnknownImpl_QueryInterface,
    IUnknownImpl_AddRef,
    IUnknownImpl_Release
};

// ======================================================================
// Local Methods
// ======================================================================

// ----------------------------------------------------------------------
// IUnknownImpl_AddRef
// ----------------------------------------------------------------------
static ULONG WIN32API IUnknownImpl_AddRef(LPUNKNOWN iface)
{ 
    ICOM_THIS(IUnknownImpl, iface);

    dprintf(("OLE32: IUnknown(%p)->AddRef()", This));

    return ++(This->ref);
}

// ----------------------------------------------------------------------
// IUnknownImpl_Release
// ----------------------------------------------------------------------
static ULONG WIN32API IUnknownImpl_Release(LPUNKNOWN iface)
{
    ICOM_THIS(IUnknownImpl, iface);

    dprintf(("OLE32: IUnknown(%p)->Release()\n", This));

    if (--(This->ref) == 0)
    {
	HeapFree(GetProcessHeap(), 0, This);
	return 0;
    }
    return This->ref;
}

// ----------------------------------------------------------------------
// IUnknownImpl_QueryInterface
// ----------------------------------------------------------------------
static HRESULT WIN32API IUnknownImpl_QueryInterface(LPUNKNOWN iface, REFIID riid, LPVOID *ppvObject)
{
    ICOM_THIS(IUnknownImpl, iface);

#ifdef DEBUG
    oStringA	tRiid(riid);
    dprintf(("OLE32: IUnknown(%p)->QueryInterface(%s)\n", This, (char *)tRiid));
#endif

    if (IsEqualIID(&IID_IUnknown, riid))
    {
        dprintf(("       ->IUnknown"));
	*ppvObject = &(This->lpvtbl);
    }
    else
    {
        dprintf(("       ->E_NOINTERFACE"));
	return E_NOINTERFACE; 
    }

    // Query Interface always increases the reference count by one...
    IUnknownImpl_AddRef(iface);

    return S_OK;
}

// ----------------------------------------------------------------------
// IUnknownImpl_Constructor
// ----------------------------------------------------------------------
LPUNKNOWN IUnknownImpl_Constructor()
{
    IUnknownImpl *	unk;

    unk = (IUnknownImpl*)HeapAlloc(GetProcessHeap(), 0, sizeof(IUnknownImpl));
    unk->lpvtbl	= &IUnknownVt;
    unk->ref	= 1;
    return (LPUNKNOWN)unk;
}


