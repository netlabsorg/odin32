/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * Win32 COM/OLE stubs for OS/2
 * 
 * 1/7/99
 * 
 * Copyright 1999 David J. Raison
 * Copyright 1998 Sander van Leeuwen
 * 
 */

#include "ole32.h"

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
//Frees all libs loaded with CoLoadLibrary
//*******************************************************************************
void WIN32API CoFreeAllLibraries()
{
    dprintf(("OLE32: CoFreeAllLibraries - stub"));
    return ;
}
//*******************************************************************************
//*******************************************************************************
void WIN32API CoFreeLibrary(HINSTANCE hInst)
{
    dprintf(("OLE32: CoFreeLibrary"));
   FreeLibrary(hInst);
}
//*******************************************************************************
//*******************************************************************************
void WIN32API CoFreeUnusedLibraries()
{
    dprintf(("OLE32: CoFreeUnusedLibraries - stub"));
    return ;
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
DWORD WIN32API CoGetCurrentProcess()
{
    dprintf(("OLE32: CoGetCurrentProcess - stub"));
    return 666;
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
HINSTANCE WIN32API CoLoadLibrary(LPSTR lpszLibName, BOOL bAutoFree)
{
    dprintf(("OLE32: CoLoadLibrary"));
    return LoadLibraryA(lpszLibName);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoLockObjectExternal(IUnknown *pUnk, BOOL fLock, BOOL fLastUnlockReleases)
{
    dprintf(("OLE32: CoLockObjectExternal - stub"));
    return E_OUTOFMEMORY;
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
HRESULT WIN32API CoRegisterClassObject(REFCLSID rclsid, IUnknown *pUnk,
                                          DWORD dwClsContext, DWORD flags,
                                          LPDWORD lpdwRegister)
{
    dprintf(("OLE32: CoRegisterClassObject - stub"));
    return E_OUTOFMEMORY;
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
HRESULT WIN32API CoRegisterMessageFilter(LPMESSAGEFILTER lpMessageFilter,
                                            LPMESSAGEFILTER *lplpMessageFilter)
{
    dprintf(("OLE32: CoRegisterMessageFilter - stub"));
    return S_FALSE;
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
HRESULT WIN32API CoRevokeClassObject(DWORD dwRegister)
{
    dprintf(("OLE32: CoRevokeClassObject - stub"));
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
HRESULT WIN32API CoSetState()
{
    dprintf(("OLE32: CoSetState, UNKNOWN API - stub"));
    return 0;
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
void WIN32API OleUninitialize(void)
{
  dprintf(("OLE32: OleUninitialize - stub"));
  return;
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
HRESULT WIN32API CreateAntiMoniker(LPMONIKER *ppmk)
{
    dprintf(("OLE32: CreateAntiMoniker - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateBindCtx(DWORD reserved, LPBC *ppbc)
{
    dprintf(("OLE32: CreateBindCtx - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateDataAdviseHolder(IDataAdviseHolder **ppDAHolder)
{
    dprintf(("OLE32: CreateDataAdviseHolder - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateDataCache(LPUNKNOWN pUnkOuter, REFCLSID rclsid,
                                    REFIID riid, LPVOID *ppvObj)
{
    dprintf(("OLE32: CreateDataCache - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateFileMoniker(LPCOLESTR lpszPathName, LPMONIKER *ppmk)
{
    dprintf(("OLE32: CreateFileMoniker - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateGenericComposite(LPMONIKER pmkFirst, LPMONIKER pmkRest,
                                           LPMONIKER *ppmkComposite)
{
    dprintf(("OLE32: CreateGenericComposite - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateILockBytesOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease,
                                              ILockBytes **ppkLkbyt)
{
    dprintf(("OLE32: CreateILockBytesOnHGlobal - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateItemMoniker(LPCOLESTR lpszDelim, LPCOLESTR lpszItem, LPMONIKER *ppmk)
{
    dprintf(("OLE32: CreateItemMoniker - stub"));
    return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateOleAdviseHolder(IOleAdviseHolder ppOAHolder)
{
    dprintf(("OLE32: CreateOleAdviseHolder - stub"));
    return E_OUTOFMEMORY;
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
HRESULT WIN32API CreateStreamOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease,
                                          LPSTREAM *ppstm)
{
    dprintf(("OLE32: CreateStreamOnHGlobal - stub"));
    return E_OUTOFMEMORY;
}
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
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API DoDragDrop(IDataObject *pDataObject, IDropSource *pDropSource,
                               DWORD dwOKEffect, DWORD *pdwEffect)
{
    dprintf(("OLE32: DoDragDrop - stub"));
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
HRESULT WIN32API GetClassFile(LPCWSTR szFileName, CLSID *pclsid)
{
    dprintf(("OLE32: GetClassFile - stub"));
    return MK_E_CANTOPENFILE;
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
HRESULT WIN32API GetHGlobalFromILockBytes(ILockBytes *pLkbyt, HGLOBAL *phglobal)
{
    dprintf(("OLE32: GetHGlobalFromILockBytes - stub"));
    return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetHGlobalFromStream(IStream *pstm, HGLOBAL *phglobal)
{
    dprintf(("OLE32: GetHGlobalFromStream - stub"));
    return E_INVALIDARG;
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
BOOL WIN32API IsAccelerator(HACCEL hAccel, INT cAccelEntries, LPMSG lpMsg,
                               WORD *lpwCmd)
{
    dprintf(("OLE32: IsAccelerator - stub"));
    return FALSE;
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API CONCRETE_IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
    dprintf(("OLE32: IsEqualGUID - stub"));
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
HRESULT WIN32API MonikerCommonPrefixWith(LPMONIKER pmkThis, LPMONIKER pmkOther,
                                            LPMONIKER *ppmkCommon)
{
    dprintf(("OLE32: MonikerCommonPrefixWith - stub"));
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
DWORD WIN32API OleBuildVersion()
{
    dprintf(("OLE32: OleBuildVersion, obsolete - stub"));
    return 666;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleConvertIStorageToOLESTREAM(IStorage *pStg, LPOLESTREAM lpolestream)
{
    dprintf(("OLE32: OleConvertIStorageToOLESTREAM - stub"));
    return E_INVALIDARG;
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
HRESULT WIN32API OleConvertOLESTREAMToIStorage(LPOLESTREAM lpolestream,
                                                  IStorage *pstg,
                                                  const DVTARGETDEVICE *ptd)
{
    dprintf(("OLE32: OleConvertOLESTREAMToIStorage - stub"));
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
HRESULT WIN32API OleCreateDefaultHandler(REFCLSID clsid, LPUNKNOWN pUnkOuter,
                                            REFIID riid, LPVOID *ppvObj)
{
    dprintf(("OLE32: OleCreateDefaultHandler - stub"));
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
HRESULT WIN32API OleCreateFromData(LPDATAOBJECT pSrcDataObj, REFIID riid,
                                      DWORD renderopt, LPFORMATETC pFormatEtc,
                                      LPOLECLIENTSITE pClientSite, LPSTORAGE pStg,
                                      LPVOID *ppvObj)
{
    dprintf(("OLE32: OleCreateFromData - stub"));
    return(DV_E_FORMATETC);
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
HOLEMENU WIN32API OleCreateMenuDescriptor(HMENU hmenuCombined, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
    dprintf(("OLE32: OleCreateMenuDescriptor - stub"));
    return(NULL);
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
HRESULT WIN32API OleDestroyMenuDescriptor(HOLEMENU holemenu)
{
    dprintf(("OLE32: OleDestroyMenuDescriptor - stub"));
    return E_OUTOFMEMORY;
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
HANDLE WIN32API OleDuplicateData(HANDLE hSrc, CLIPFORMAT cfFormat, UINT uiFlags)
{
    dprintf(("OLE32: OleDuplicateData - stub"));
    return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleFlushClipboard()
{
    dprintf(("OLE32: OleFlushClipboard - stub"));
    return(S_OK);
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
HRESULT WIN32API OleGetClipboard(IDataObject **ppDataObj)
{
    dprintf(("OLE32: OleGetClipboard - stub"));
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
HRESULT WIN32API OleInitialize(LPVOID pvReserved)
{
    dprintf(("OLE32: OleInitialize - stub"));
//   return(E_OUTOFMEMORY);
    return(0);
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
HRESULT WIN32API OleIsCurrentClipboard(IDataObject *pDataObject)
{
    dprintf(("OLE32: OleIsCurrentClipboard - stub"));
    return(S_FALSE);
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
HRESULT WIN32API OleLoad(IStorage *pStg, REFIID riid, IOleClientSite *pClientSite,
                            LPVOID *ppvObj)
{
    dprintf(("OLE32: OleLoad - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleLoadFromStream(IStream *pStm, REFIID riid, void *ppvObj)
{
    dprintf(("OLE32: OleLoadFromStream - stub"));
    return(E_OUTOFMEMORY);
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
HRESULT WIN32API OleQueryCreateFromData(IDataObject *pSrcDataObject)
{
    dprintf(("OLE32: OleQueryCreateFromData - stub"));
    return(S_FALSE);
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
HRESULT WIN32API OleRegGetMiscStatus(REFCLSID clsid, DWORD dwAspect, DWORD *pdwStatus)
{
    dprintf(("OLE32: OleRegGetMiscStatus - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRegGetUserType(REFCLSID clsid, DWORD dwFormOfType,
                                      LPOLESTR *pszUserType)
{
    dprintf(("OLE32: OleRegGetUserType - stub"));
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
HRESULT WIN32API OleSave(IPersistStorage *pPS, IStorage *pStg, BOOL fSameAsLoad)
{
    dprintf(("OLE32: OleSave - stub"));
    return(STG_E_MEDIUMFULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSaveToStream(IPersistStorage *pPS, IStream *pStm)
{
    dprintf(("OLE32: OleSaveToStream - stub"));
    return(STG_E_MEDIUMFULL);
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
HRESULT WIN32API OleSetClipboard(IDataObject *pDataObj)
{
    dprintf(("OLE32: OleSetClipboard - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSetContainedObject(LPUNKNOWN pUnknown, BOOL fContained)
{
    dprintf(("OLE32: OleSetContainedObject - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSetMenuDescriptor(HOLEMENU holemenu, HWND hwndFrame,
                                         HWND hwndActiveObject,
                                         LPOLEINPLACEFRAME lpFrame,
                                         LPOLEINPLACEACTIVEOBJECT lpActiveObj)
{
    dprintf(("OLE32: OleSetMenuDescriptor - stub"));
    return(E_FAIL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleTranslateAccelerator(LPOLEINPLACEFRAME lpFrame,
                                            LPOLEINPLACEFRAMEINFO lpFrameInfo,
                                            LPMSG lpmsg)
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
HRESULT WIN32API ProgIDFromCLSID(REFCLSID clsid, LPOLESTR *lplpszProgID)
{
    dprintf(("OLE32: ProgIDFromCLSID - stub"));
    return(REGDB_E_CLASSNOTREG);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadClassStg(IStorage *pStg, CLSID *pclsid)
{
    dprintf(("OLE32: ReadClassStg - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadClassStm(IStream *pStm, CLSID *pclsid)
{
    dprintf(("OLE32: ReadClassStm - stub"));
    return(STG_E_READFAULT);
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
HRESULT WIN32API RegisterDragDrop(HWND hwnd, IDropTarget *pDropTarget)
{
    dprintf(("OLE32: RegisterDragDrop - stub"));
    return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
void WIN32API ReleaseStgMedium(STGMEDIUM *pmedium)
{
    dprintf(("OLE32: ReleaseStgMedium - stub"));
    return;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API RevokeDragDrop(HWND hwnd)
{
    dprintf(("OLE32: RevokeDragDrop - stub"));
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
HRESULT WIN32API StgCreateDocfile(const WCHAR *pwcsName, DWORD grfMode,
                                     DWORD reserved, IStorage **ppstgOpen)
{
    dprintf(("OLE32: StgCreateDocfile - stub"));
    return(STG_E_INSUFFICIENTMEMORY);
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
HRESULT WIN32API StgCreateDocfileOnILockBytes(ILockBytes *plkbyt, DWORD grfMode,
                                                 DWORD reserved, IStorage **ppstgOpen)
{
    dprintf(("OLE32: StgCreateDocfileOnILockBytes - stub"));
    return(STG_E_INSUFFICIENTMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgIsStorageFile(const WCHAR *pwcsName)
{
    dprintf(("OLE32: StgIsStorageFile - stub"));
    return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgIsStorageILockBytes(ILockBytes *plkbyt)
{
    dprintf(("OLE32: StgIsStorageILockBytes - stub"));
    return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgOpenStorage(const WCHAR *pwcsName, IStorage *pstgPriority,
                                   DWORD grfMode, SNB snbExclude, DWORD reserved,
                                   IStorage **ppstgOpen)
{
    dprintf(("OLE32: StgOpenStorage - stub"));
    return(STG_E_INSUFFICIENTMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgOpenStorageOnILockBytes(ILockBytes *plkbyt,
                                               IStorage *pstgPriority,
                                               DWORD grfMode, SNB snbExclude,
                                               DWORD reserved,
                                               IStorage **ppstgOpen)
{
    dprintf(("OLE32: StgOpenStorageOnILockBytes - stub"));
    return(STG_E_INSUFFICIENTMEMORY);
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
HRESULT WIN32API WriteClassStg(IStorage *pStg, REFCLSID rclsid)
{
    dprintf(("OLE32: WriteClassStg - stub"));
    return(STG_E_MEDIUMFULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteClassStm(IStream *pStm, REFCLSID rclsid)
{
    dprintf(("OLE32: WriteClassStm - stub"));
    return(STG_E_MEDIUMFULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteFmtUserTypeStg(IStorage *pStg, CLIPFORMAT cf, LPWSTR *lpszUserType)
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

