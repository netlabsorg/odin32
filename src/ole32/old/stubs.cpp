/* $Id: stubs.cpp,v 1.1 2001-04-26 19:26:15 sandervl Exp $ */
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
HRESULT WIN32API DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
    dprintf(("OLE32: DllGetClassObject - stub"));
    return E_OUTOFMEMORY;
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
HRESULT WIN32API CoCreateFreeThreadedMarshaler(LPUNKNOWN punkOuter,
                                                  LPUNKNOWN *ppunkMarshaler)
{
    dprintf(("OLE32: CoCreateFreeThreadedMarshaler- stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoDisconnectObject(IUnknown *pUnk, DWORD dwReserved)
{
    dprintf(("OLE32: CoDisconnectObject - stub"));
    return S_OK;
}
//*******************************************************************************
//*******************************************************************************
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
HRESULT WIN32API CoGetInterfaceAndReleaseStream(LPSTREAM pStm, REFIID riid,
                                                   LPVOID *ppv)
{
    dprintf(("OLE32: CoGetInterfaceAndReleaseStream - stub"));
    return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetMarshalSizeMax(ULONG *pulSize, REFIID riid, IUnknown *pUnk,
                                        DWORD dwDestContext, LPVOID pvDestContext,
                                        DWORD mshlflags)
{
    dprintf(("OLE32: CoGetMarshalSizeMax - stub"));
    return CO_E_NOTINITIALIZED;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetPSClsid(REFIID riid, CLSID *pclsid)
{
    dprintf(("OLE32: CoGetPSClsid - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetStandardMarshal(REFIID riid, IUnknown *pUnk, DWORD dwDestContext,
                                         LPVOID pvDestContext, DWORD mshlflags,
                                         LPMARSHAL *ppMarshal)
{
    dprintf(("OLE32: CoGetStandardMarshal - stub"));
    return E_OUTOFMEMORY;
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
HRESULT WIN32API CoInitializeWOW()
{
    dprintf(("OLE32: CoInitializeWOW, UNKNOWN API - stub"));
    return 0;
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
BOOL WIN32API CoIsOle1Class(REFCLSID rclsid)
{
    dprintf(("OLE32: CoIsOle1Class - stub"));
    return S_FALSE;
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
HRESULT WIN32API CoMarshalInterThreadInterfaceInStream(REFIID riid,
                                                          LPUNKNOWN pUnk,
                                                          LPSTREAM *ppStm)
{
    dprintf(("OLE32: CoMarshalInterThreadInterfaceInStream - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoMarshalInterface(IStream *pStm, REFIID riid, IUnknown *pUnk,
                                       DWORD dwDestContext, void *pvDestContext,
                                       DWORD mshlflags)
{
    dprintf(("OLE32: CoMarshalInterface - stub"));
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
HRESULT WIN32API CoSetState(LPDWORD state)
{
    dprintf(("OLE32: CoSetState (%p),stub!\n", state));
    if (state) *state = 0;
    return S_OK;
}

//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoTreatAsClass(REFCLSID clsidOld, REFCLSID clsidNew)
{
    dprintf(("OLE32: CoTreatAsClass - stub"));
    return E_INVALIDARG;
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
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoUnmarshalInterface(IStream *pSTm, REFIID riid, void **ppv)
{
    dprintf(("OLE32: CoUnmarshalInterface - stub"));
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
HRESULT WIN32API GetConvertStg(IStorage *pStg)
{
    dprintf(("OLE32: GetConvertStg - stub"));
    return STG_E_ACCESSDENIED;
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
BOOL WIN32API IsAccelerator(HACCEL hAccel, int cAccelEntries, struct tagMSG* lpMsg, WORD* lpwCmd)
{
    dprintf(("OLE32: IsAccelerator - stub"));
    return FALSE;
}
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
HRESULT WIN32API MkParseDisplayName(LPBC pbc, LPSTR szUserName, ULONG *lpchEaten,
                                        LPMONIKER *ppmk)
{
    dprintf(("OLE32: MkParseDisplayName - stub"));
    return E_OUTOFMEMORY;
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
HRESULT WIN32API OleCreate(REFCLSID rclsid, REFIID riid, DWORD renderopt,
                              FORMATETC *pFormatEtc, IOleClientSite *pClientSite,
                              IStorage *pStg, void **ppvObject)
{
    dprintf(("OLE32: OleCreate - stub"));
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
HRESULT WIN32API OleCreateFromFile(REFCLSID rclsid, LPCOLESTR lpszFileName,
                                      REFIID riid,
                                      DWORD renderopt, LPFORMATETC pFormatEtc,
                                      LPOLECLIENTSITE pClientSite, LPSTORAGE pStg,
                                      LPVOID *ppvObj)
{
    dprintf(("OLE32: OleCreateFromFile - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateLink(LPMONIKER lpmkLinkSrc, REFIID riid,
                                  DWORD renderopt, LPFORMATETC pFormatEtc,
                                  LPOLECLIENTSITE lpClientSite, LPSTORAGE pStg,
                                  LPVOID *ppvObj)
{
    dprintf(("OLE32: OleCreateLink - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateLinkFromData(LPDATAOBJECT pSrcDataObj, REFIID riid,
                                          DWORD renderopt, LPFORMATETC pFormatEtc,
                                          LPOLECLIENTSITE pClientSite, LPSTORAGE pStg,
                                          LPVOID *ppvObj)
{
    dprintf(("OLE32: OleCreateLinkFromData - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateLinkToFile(LPCOLESTR lpszFileName, REFIID riid, DWORD renderopt,
                                        LPFORMATETC pFormatEtc, IOleClientSite *pClientSite,
                                        IStorage *pStg, void **ppvObj)
{
    dprintf(("OLE32: OleCreateLinkToFile - stub"));
    return(STG_E_FILENOTFOUND);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateStaticFromData(LPDATAOBJECT pSrcDataObj, REFIID riid,
                                            DWORD renderopt, LPFORMATETC pFormatEtc,
                                            LPOLECLIENTSITE pClientSite, LPSTORAGE pStg,
                                            LPVOID *ppvObj)
{
    dprintf(("OLE32: OleCreateStaticFromData - stub"));
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
HRESULT WIN32API OleDuplicateData(HANDLE hSrc, CLIPFORMAT cfFormat, UINT uiFlags)
{
    dprintf(("OLE32: OleDuplicateData - stub"));
    return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleGetAutoConvert(REFCLSID clsidOld, LPCLSID pClsidNew)
{
    dprintf(("OLE32: OleGetAutoConvert - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HGLOBAL WIN32API OleGetIconOfClass(REFCLSID rclsid, LPOLESTR lpszLabel, BOOL fUseTypeAsLabel)
{
    dprintf(("OLE32: OleGetIconOfClass - stub"));
    return(NULL);
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
BOOL WIN32API OleIsRunning(LPOLEOBJECT pObject)
{
    dprintf(("OLE32: OleIsRunning - stub"));
    return(FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleLockRunning(LPUNKNOWN pUnknown, BOOL fLock, BOOL fLastUnlockCloses)
{
    dprintf(("OLE32: OleLockRunning - stub"));
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
HRESULT WIN32API OleQueryLinkFromData(IDataObject *pSrcDataObject)
{
    dprintf(("OLE32: OleQueryLinkFromData - stub"));
    return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRegEnumFormatEtc(REFCLSID clsid, DWORD dwDirection,
                                        LPENUMFORMATETC *ppenumFormatetc)
{
    dprintf(("OLE32: OleRegEnumFormatEtc - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRegEnumVerbs(REFCLSID clsid, LPENUMOLEVERB *ppenumOleVerb)
{
    dprintf(("OLE32: OleRegEnumVerbs - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRun(LPUNKNOWN pUnknown)
{
    dprintf(("OLE32: OleRun - stub"));
    return(E_UNEXPECTED);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSetAutoConvert(REFCLSID clsidOld, REFCLSID clsidNew)
{
    dprintf(("OLE32: OleSetAutoConvert - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleTranslateAccelerator
   (LPOLEINPLACEFRAME		lpFrame,
    LPOLEINPLACEFRAMEINFO	lpFrameInfo,
    struct tagMSG *		lpmsg)
{
    dprintf(("OLE32: OleTranslateAccelerator - stub"));
    return(S_FALSE);
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
HRESULT WIN32API ReadFmtUserTypeStg(IStorage *pStg, CLIPFORMAT *pcf,
                                       LPWSTR *lplpszUserType)
{
    dprintf(("OLE32: ReadFmtUserTypeStg - stub"));
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
HRESULT WIN32API SetConvertStg(IStorage *pStg, BOOL fConvert)
{
    dprintf(("OLE32: SetConvertStg - stub"));
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
HRESULT WIN32API StgSetTimes(WCHAR const *lpszName, FILETIME const *pctime,
                                FILETIME const *patime, FILETIME const *pmtime)
{
    dprintf(("OLE32: StgSetTimes - stub"));
    return(STG_E_FILENOTFOUND);
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
HRESULT WIN32API WriteFmtUserTypeStg(LPSTORAGE pstg, CLIPFORMAT cf, LPOLESTR lpszUserType)
{
    dprintf(("OLE32: WriteFmtUserTypeStg - stub"));
    return(STG_E_MEDIUMFULL);
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
