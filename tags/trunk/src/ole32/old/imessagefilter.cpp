/* $Id: imessagefilter.cpp,v 1.1 2001-04-26 19:26:10 sandervl Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * Default Message Filter Implementation
 * 
 * 18/04/2000
 * 
 * Copyright 2000 David J. Raison
 * 
 */

#include "ole32.h"
#include "oString.h"

// ======================================================================
// Local Data
// ======================================================================

typedef struct
{
    /* IUnknown fields */
    ICOM_VTABLE(IMessageFilter)*	lpvtbl;
    DWORD                  		ref;
} IMessageFilterImpl;

static ULONG	WIN32API IMessageFilterImpl_AddRef(LPMESSAGEFILTER iface);
static ULONG	WIN32API IMessageFilterImpl_Release(LPMESSAGEFILTER iface);
static HRESULT	WIN32API IMessageFilterImpl_QueryInterface(LPMESSAGEFILTER iface,
			    REFIID riid, LPVOID * ppvObject);
static DWORD	WIN32API IMessageFilterImpl_HandleInComingCall(LPMESSAGEFILTER iface,
			    DWORD dwCallType, HTASK htaskCaller, DWORD dwTickCount,
			    LPINTERFACEINFO lpInterfaceInfo);
static DWORD	WIN32API IMessageFilterImpl_RetryRejectedCall(LPMESSAGEFILTER iface,
			    HTASK htaskCallee, DWORD dwTickCount, DWORD dwRejectType);
static DWORD	WIN32API IMessageFilterImpl_MessagePending(LPMESSAGEFILTER iface,
			    HTASK htaskCallee, DWORD dwTickCount, DWORD dwRejectType);

static ICOM_VTABLE(IMessageFilter) IMessageFilterVt = 
{
    IMessageFilterImpl_QueryInterface,
    IMessageFilterImpl_AddRef,
    IMessageFilterImpl_Release,
    IMessageFilterImpl_HandleInComingCall,
    IMessageFilterImpl_RetryRejectedCall,
    IMessageFilterImpl_MessagePending
};

// ======================================================================
// Local Methods
// ======================================================================

// ----------------------------------------------------------------------
// IMessageFilterImpl_Constructor
// ----------------------------------------------------------------------
LPMESSAGEFILTER IMessageFilterImpl_Constructor()
{
    IMessageFilterImpl *	pObject;

    pObject = (IMessageFilterImpl*)HeapAlloc(GetProcessHeap(), 0, sizeof(IMessageFilterImpl));
    pObject->lpvtbl	= &IMessageFilterVt;
    pObject->ref	= 1;

    return (LPMESSAGEFILTER)pObject;
}

// ----------------------------------------------------------------------
// IMessageFilterImpl_AddRef
// ----------------------------------------------------------------------
static ULONG WIN32API IMessageFilterImpl_AddRef(LPMESSAGEFILTER iface)
{ 
    ICOM_THIS(IMessageFilterImpl, iface);

    dprintf(("OLE32: IMessageFilter(%p)->AddRef()", This));

    return ++(This->ref);
}

// ----------------------------------------------------------------------
// IMessageFilterImpl_Release
// ----------------------------------------------------------------------
static ULONG WIN32API IMessageFilterImpl_Release(LPMESSAGEFILTER iface)
{
    ICOM_THIS(IMessageFilterImpl, iface);

    dprintf(("OLE32: IMessageFilter(%p)->Release()\n", This));

    if (--(This->ref) == 0)
    {
	HeapFree(GetProcessHeap(), 0, This);
	return 0;
    }
    return This->ref;
}

// ----------------------------------------------------------------------
// IMessageFilterImpl_QueryInterface
// ----------------------------------------------------------------------
static HRESULT WIN32API IMessageFilterImpl_QueryInterface(LPMESSAGEFILTER iface,
				REFIID riid, LPVOID *ppvObject)
{
    ICOM_THIS(IMessageFilterImpl, iface);

#ifdef DEBUG
    oStringA	tRiid(riid);
    dprintf(("OLE32: IMessageFilter(%p)->QueryInterface(%s)\n", This, (char *)tRiid));
#endif

    if (IsEqualIID(&IID_IUnknown, riid))
    {
        dprintf(("       ->IUnknown"));
	*ppvObject = &(This->lpvtbl);
    }
    else if (IsEqualIID(&IID_IMessageFilter, riid))
    {
        dprintf(("       ->IMessageFilter"));
	*ppvObject = &(This->lpvtbl);
    }
    else
    {
        dprintf(("       ->E_NOINTERFACE"));
	return E_NOINTERFACE; 
    }

    // Query Interface always increases the reference count by one...
    IMessageFilterImpl_AddRef(iface);

    return S_OK;
}

// ----------------------------------------------------------------------
// IMessageFilterImpl_HandleInComingCall
// ----------------------------------------------------------------------
static DWORD	WIN32API IMessageFilterImpl_HandleInComingCall(LPMESSAGEFILTER iface,
			    DWORD dwCallType, HTASK htaskCaller, DWORD dwTickCount,
			    LPINTERFACEINFO lpInterfaceInfo)
{
    ICOM_THIS(IMessageFilterImpl, iface);

    dprintf(("OLE32: IMessageFilter(%p)->HandleInComingCall()\n", This));
    return E_NOTIMPL;
}


// ----------------------------------------------------------------------
// IMessageFilterImpl_RetryRejectedCall
// ----------------------------------------------------------------------
static DWORD	WIN32API IMessageFilterImpl_RetryRejectedCall(LPMESSAGEFILTER iface,
			    HTASK htaskCallee, DWORD dwTickCount, DWORD dwRejectType)
{
    ICOM_THIS(IMessageFilterImpl, iface);

    dprintf(("OLE32: IMessageFilter(%p)->RetryRejectedCall()\n", This));
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------
// IMessageFilterImpl_MessagePending
// ----------------------------------------------------------------------
static DWORD	WIN32API IMessageFilterImpl_MessagePending(LPMESSAGEFILTER iface,
			    HTASK htaskCallee, DWORD dwTickCount, DWORD dwRejectType)
{
    ICOM_THIS(IMessageFilterImpl, iface);

    dprintf(("OLE32: IMessageFilter(%p)->MessagePending()\n", This));
    return E_NOTIMPL;
}

//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRegisterMessageFilter(LPMESSAGEFILTER lpMessageFilter,
                                            LPMESSAGEFILTER *lplpMessageFilter)
{
    dprintf(("OLE32: CoRegisterMessageFilter - stub"));
    if (lplpMessageFilter)
	*lplpMessageFilter = NULL;
    return S_FALSE;
}

