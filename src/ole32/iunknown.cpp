/* $Id: iunknown.cpp,v 1.3 1999-09-08 11:29:27 davidr Exp $ */
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
    /* IUnknown fields */
    ICOM_VTABLE(IUnknown)* lpvtbl;
    DWORD                  ref;
} IUnknownImpl;

static ULONG WIN32API IUnknown_fnAddRef(LPUNKNOWN iface);
static ULONG WIN32API IUnknown_fnRelease(LPUNKNOWN iface);
static HRESULT WIN32API IUnknown_fnQueryInterface(LPUNKNOWN iface, REFIID refiid, LPVOID * obj);

static ICOM_VTABLE(IUnknown) uvt = 
{
    IUnknown_fnQueryInterface,
    IUnknown_fnAddRef,
    IUnknown_fnRelease
};

// ======================================================================
// Local Methods
// ======================================================================

// ----------------------------------------------------------------------
// IUnknown_fnAddRef
// ----------------------------------------------------------------------
static ULONG WIN32API IUnknown_fnAddRef(LPUNKNOWN iface)
{ 
    ICOM_THIS(IUnknownImpl, iface);

    dprintf(("OLE32: (%p)->AddRef()", This));

    return ++(This->ref);
}

// ----------------------------------------------------------------------
// IUnknown_fnRelease
// ----------------------------------------------------------------------
static ULONG WIN32API IUnknown_fnRelease(LPUNKNOWN iface)
{
    ICOM_THIS(IUnknownImpl, iface);

    dprintf(("OLE32: (%p)->Release()\n", This));

    if (--(This->ref) == 0)
    {
	HeapFree(GetProcessHeap(), 0, This);
	return 0;
    }
    return This->ref;
}

// ----------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------
static HRESULT WIN32API IUnknown_fnQueryInterface(LPUNKNOWN iface, REFIID refiid, LPVOID *obj)
{
    ICOM_THIS(IUnknownImpl, iface);

#ifdef DEBUG
    oStringA	tRefiid(refiid);
    dprintf(("OLE32: (%p)->QueryInterface(%s, %p)\n", This, (char *)tRefiid, obj));
#endif

    if (!memcmp(&IID_IUnknown, refiid, sizeof(IID_IUnknown)))
    {
	*obj = This;
	return 0; 
    }
    return OLE_E_ENUM_NOMORE; 
}

// ----------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------
LPUNKNOWN IUnknown_Constructor()
{
    IUnknownImpl *	unk;

    unk = (IUnknownImpl*)HeapAlloc(GetProcessHeap(), 0, sizeof(IUnknownImpl));
    unk->lpvtbl	= &uvt;
    unk->ref	= 1;
    return (LPUNKNOWN)unk;
}


