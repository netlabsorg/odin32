/* $Id: stubs.cpp,v 1.21 2002-02-19 12:34:50 sandervl Exp $ */
/* 
 * Win32 COM/OLE stubs for OS/2
 * 
 * 1/7/99
 * 
 * Copyright 1999 David J. Raison
 * Copyright 1998 Sander van Leeuwen
 * 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#include "ole32.h"

//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API DllDebugObjectRPCHook()
{
    dprintf(("OLE32: DllDebugObjectRPCHook, UNKNOWN API - stub"));
    return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API DllGetClassObjectWOW()
{
    dprintf(("OLE32: DllGetClassObjectWOW, UNKNOWN API - stub"));
    return 0;
}
//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API CoGetCallerTID()
{
    dprintf(("OLE32: CoGetCallerTID, UNKNOWN API - stub"));
    return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetCurrentLogicalThreadId()
{
    dprintf(("OLE32: CoGetCurrentLogicalThreadId, UNKNOWN API - stub"));
    return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetState()
{
    dprintf(("OLE32: CoGetState, UNKNOWN API - stub"));
    return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetTreatAsClass(REFCLSID clsidOld, LPCLSID pclsidNew)
{
    dprintf(("OLE32: CoGetTreatAsClass - stub"));
    return S_FALSE;
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API CoIsHandlerConnected(LPUNKNOWN pUnk)
{
    dprintf(("OLE32: CoIsHandlerConnected - stub"));
    return FALSE;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoMarshalHresult(IStream *pStm, HRESULT hresult)
{
    dprintf(("OLE32: CoMarshalHresult - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoQueryReleaseObject()
{
    dprintf(("OLE32: CoQueryReleaseObject, UNKNOWN API - stub"));
    return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRegisterMallocSpy(LPMALLOCSPY pMallocSpy)
{
    dprintf(("OLE32: CoRegisterMallocSpy - stub"));
    return CO_E_OBJISREG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoReleaseMarshalData(IStream *pStm)
{
    dprintf(("OLE32: CoReleaseMarshalData - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRevokeMallocSpy()
{
    dprintf(("OLE32: CoRevokeMallocSpy - stub"));
    return E_ACCESSDENIED;
}
//*******************************************************************************
//*******************************************************************************
void WIN32API CoUnloadingWOW()
{
    dprintf(("OLE32: CoUnloadingWOW, UNKNOWN API - stub"));
    return ;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoUnmarshalHresult(LPSTREAM pStm, HRESULT *phresult)
{
    dprintf(("OLE32: CoUnmarshalHresult - stub"));
    return S_OK;
}

//******************************************************************************
//******************************************************************************
HRESULT WIN32API BindMoniker(LPMONIKER pmk, DWORD grfOpt, REFIID iidResult,
                                LPVOID *ppvResult)
{
    dprintf(("OLE32: BindMoniker- stub"));
    return MK_E_NOOBJECT;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreatePointerMoniker(LPUNKNOWN punk, LPMONIKER *ppmk)
{
    dprintf(("OLE32: CreatePointerMoniker - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API EnableHookObject()
{
    dprintf(("OLE32: EnableHookObject, UNKNOWN API - stub"));
    return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetDocumentBitStg()
{
    dprintf(("OLE32: GetDocumentBitStg, UNKNOWN API - stub"));
    return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetHookInterface()
{
    dprintf(("OLE32: GetHookInterface, UNKNOWN API - stub"));
    return 0;
}
//*******************************************************************************
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API IsValidIid(DWORD Iid)
{
    dprintf(("OLE32: IsValidIid, obsolete - stub"));
    return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API IsValidPtrIn(DWORD ptrIn)
{
    dprintf(("OLE32: IsValidPtrIn, obsolete - stub"));
    return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API IsValidPtrOut(DWORD ptrOut)
{
    dprintf(("OLE32: IsValidPtrOut, obsolete - stub"));
    return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API MonikerRelativePathTo(LPMONIKER pmkSrc, LPMONIKER pmkDest,
                                          LPMONIKER *ppmkRelPath, BOOL dwReserved )
{
    dprintf(("OLE32: MonikerRelativePathTo - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleConvertIStorageToOLESTREAMEx(LPSTORAGE	pStg,
						 CLIPFORMAT	ctFormat,
						 LONG		lWidth,
						 LONG		lHeight,
						 DWORD		dwSize,
						 LPSTGMEDIUM	pmedium,
						 LPOLESTREAM	lpolestream)
{
    dprintf(("OLE32: OleConvertIStorageToOLESTREAMEx - stub"));
    return(E_INVALIDARG);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleConvertOLESTREAMToIStorageEx(LPOLESTREAM	lpolestream,
                                                 LPSTORAGE	pstg,
                                                 CLIPFORMAT *	ctFormat,
                                                 LONG *		plWidth,
                                                 LONG *		plHeight,
                                                 DWORD *	pdwSize,
                                                 LPSTGMEDIUM	pmedium)
{
    dprintf(("OLE32: OleConvertOLESTREAMToIStorageEx - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateEmbeddingHelper(REFCLSID clsid, LPUNKNOWN pUnkOuter,
                                             DWORD flags, LPCLASSFACTORY pCF,
                                             REFIID riid, LPVOID *ppvObj)
{
    dprintf(("OLE32: OleCreateEmbeddingHelper - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleDoAutoConvert(IStorage *pStg, LPCLSID pClsidNew)
{
    dprintf(("OLE32: OleDoAutoConvert - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleDraw(IUnknown *pUnk, DWORD dwAspect, HDC hdcDraw,
                            LPCRECT lprcBounds)
{
    dprintf(("OLE32: OleDraw - stub"));
    return(E_INVALIDARG);
}
//*******************************************************************************
//*******************************************************************************
HGLOBAL WIN32API OleGetIconOfFile(LPOLESTR lpszPath, BOOL fUseTypeAsLabel)
{
    dprintf(("OLE32: OleGetIconOfFile - stub"));
    return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleInitializeWOW()
{
    dprintf(("OLE32: OleInitializeWOW, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HGLOBAL WIN32API OleMetafilePictFromIconAndLabel(HICON hIcon, LPOLESTR lpszLabel,
                                                    LPOLESTR lpszSourceFile,
                                                    UINT iIconIndex)
{
    dprintf(("OLE32: OleMetafilePictFromIconAndLabel - stub"));
    return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleNoteObjectVisible(LPUNKNOWN pUnknown, BOOL fVisible)
{
    dprintf(("OLE32: OleNoteObjectVisible - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OpenOrCreateStream()
{
    dprintf(("OLE32: OpenOrCreateStream, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadOleStg()
{
    dprintf(("OLE32: ReadOleStg, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadStringStream()
{
    dprintf(("OLE32: ReadStringStream, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API SetDocumentBitStg()
{
    dprintf(("OLE32: SetDocumentBitStg, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API UtConvertDvtd16toDvtd32()
{
    dprintf(("OLE32: UtConvertDvtd16toDvtd32, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API UtConvertDvtd32toDvtd16()
{
    dprintf(("OLE32: UtConvertDvtd32toDvtd16, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API UtGetDvtd16Info()
{
    dprintf(("OLE32: UtGetDvtd16Info, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API UtGetDvtd32Info()
{
    dprintf(("OLE32: UtGetDvtd32Info, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}

//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteOleStg()
{
    dprintf(("OLE32: WriteOleStg, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteStringStream()
{
    dprintf(("OLE32: WriteStringStream, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************

HRESULT WIN32API CoImpersonateClient()		// DCOM - obj_clientserver.h
{
    dprintf(("OLE32: CoImpersonateClient - Stub"));
    return(S_OK);
}

//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoInitializeSecurity(		// DCOM - obj_clientserver.h
    PSECURITY_DESCRIPTOR         pSecDesc,
    LONG                         cAuthSvc,
    SOLE_AUTHENTICATION_SERVICE *asAuthSvc,
    void                        *pReserved1,
    DWORD                        dwAuthnLevel,
    DWORD                        dwImpLevel,
    void                        *pReserved2,
    DWORD                        dwCapabilities,
    void                        *pReserved3 )
{
    dprintf(("OLE32: CoInitializeSecurity - Stub"));
    return(S_OK);
}

//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRegisterSurrogate(LPSURROGATE pSurrogate)	// NT4 SP2, Win95b+
{
    dprintf(("OLE32: CoRegisterSurrogate - Stub"));
    return(S_OK);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetInstanceFromFile(
    COSERVERINFO *              pServerInfo,
    CLSID       *               pClsid,
    IUnknown    *               punkOuter, // only relevant locally
    DWORD                       dwClsCtx,
    DWORD                       grfMode,
    OLECHAR *                   pwszName,
    DWORD                       dwCount,
    MULTI_QI        *           pResults )
{
    dprintf(("ERROR: OLE32: CoGetInstanceFromFile, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetInstanceFromIStorage(
    COSERVERINFO *              pServerInfo,
    CLSID       *               pClsid,
    IUnknown    *               punkOuter, // only relevant locally
    DWORD                       dwClsCtx,
    struct IStorage *           pstg,
    DWORD                       dwCount,
    MULTI_QI        *           pResults )
{
    dprintf(("ERROR: OLE32: CoGetInstanceFromIStorage, UNKNOWN API - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************