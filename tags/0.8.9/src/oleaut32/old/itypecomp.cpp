/* $Id: itypecomp.cpp,v 1.1 2001-08-10 19:24:45 sandervl Exp $ */
/* 
 * ITypeComp interface
 * 
 * 15/04/2000
 * 
 * Copyright 2000 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#include "oleaut32.h"
#include "olectl.h"
#include "oList.h"	// linked list template
#include "itypelib.h"

// ======================================================================
// Local Data
// ======================================================================
static ICOM_VTABLE(ITypeComp) ITypeComp_VTable =
{
    ITypeCompImpl_QueryInterface,
    ITypeCompImpl_AddRef,
    ITypeCompImpl_Release,
    ITypeCompImpl_Bind,
    ITypeCompImpl_BindType
};

// ----------------------------------------------------------------------
// ITypeCompImpl_Constructor
// ----------------------------------------------------------------------
ITypeCompImpl * ITypeCompImpl_Constructor()
{
    ITypeCompImpl *	pNew;

//    dprintf(("OLEAUT32: ITypeComp()->Constructor()"));

    pNew = new ITypeCompImpl;

    // Initialise the vft
    pNew->lpvtbl = &ITypeComp_VTable;
    pNew->ref = 1;

    return pNew;
}

// ----------------------------------------------------------------------
// ITypeCompImpl_Destructor
// ----------------------------------------------------------------------
void ITypeCompImpl_Destructor(ITypeCompImpl * This)
{
    dprintf(("OLEAUT32: ITypeCompImpl(%p)->Destructor()", This));

    // TODO - need to relase all substructures etc.
//    delete This;
}

// ----------------------------------------------------------------------
// ITypeCompImpl_QueryInterface
//
// This object supports IUnknown, ITypeComp, ITypeComp2
// ----------------------------------------------------------------------
HRESULT WIN32API ITypeCompImpl_QueryInterface(LPTYPECOMP iface,
				REFIID riid, VOID **ppvObject)
{
    ICOM_THIS(ITypeCompImpl, iface);

    dprintf(("OLEAUT32: ITypeCompImpl(%p)->QueryInterface()", This));

    // Sanity check...
    if ((This == 0) || (ppvObject == 0))
	return E_INVALIDARG;

    // Initialise return jic...
    *ppvObject = 0;

    // Go find the correct interface...
    if (IsEqualIID(&IID_IUnknown, riid))
    {
	dprintf(("          ->IUnknown"));
	*ppvObject = &(This->lpvtbl);
    }
    else if (IsEqualIID(&IID_ITypeComp, riid))
    {
	dprintf(("          ->ITypeComp"));
	*ppvObject = &(This->lpvtbl);
    }
    else
    {
	char	tmp[50];

	WINE_StringFromCLSID(riid, tmp);

	dprintf(("          ->E_NOINTERFACE(%s)", tmp));
	return E_NOINTERFACE; 
    }

    // Query Interface always increases the reference count by one...
    ITypeCompImpl_AddRef(iface);

    return S_OK;
}


// ----------------------------------------------------------------------
// ITypeCompImpl_AddRef
// ----------------------------------------------------------------------
ULONG   WIN32API ITypeCompImpl_AddRef(LPTYPECOMP iface)
{
    ICOM_THIS(ITypeCompImpl, iface);

    dprintf(("OLEAUT32: ITypeCompImpl(%p)->AddRef()", This));

    return ++(This->ref);
}


// ----------------------------------------------------------------------
// ITypeCompImpl_Release
// ----------------------------------------------------------------------
ULONG   WIN32API ITypeCompImpl_Release(LPTYPECOMP iface)
{
    ICOM_THIS(ITypeCompImpl, iface);

    dprintf(("OLEAUT32: ITypeCompImpl(%p)->Release()\n", This));

    if (--(This->ref) == 0)
    {
	ITypeCompImpl_Destructor(This);
	return 0;
    }
    return This->ref;
}


// ----------------------------------------------------------------------
// ITypeCompImpl_Bind
// ----------------------------------------------------------------------
HRESULT	WIN32API ITypeCompImpl_Bind(LPTYPECOMP iface,
				LPOLESTR szName, ULONG lHashVal, WORD wFlags,
				ITypeInfo** ppTInfo, DESCKIND* pDescKind, BINDPTR* pBindPtr)
{
    ICOM_THIS(ITypeCompImpl, iface);

    dprintf(("OLEAUT32: ITypeCompImpl(%p)->Bind() - STUB\n", This));

    return E_OUTOFMEMORY;
}

// ----------------------------------------------------------------------
// ITypeCompImpl_BindType
// ----------------------------------------------------------------------
HRESULT	WIN32API ITypeCompImpl_BindType(LPTYPECOMP iface,
				LPOLESTR szName, ULONG lHashVal, ITypeInfo** ppTInfo,
				ITypeComp** ppTComp)
{
    ICOM_THIS(ITypeCompImpl, iface);

    dprintf(("OLEAUT32: ITypeCompImpl(%p)->BindType() - STUB\n", This));

    return E_OUTOFMEMORY;
}
