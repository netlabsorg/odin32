/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 OLE stubs for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 */

// ><DJR 17.05.99 Force to use C-interfaces for now to prevent CALLBACK definition compiler error
#define CINTERFACE

// ><DJR 17.05.99 Move standard includes to before os2win.h [memcmp]
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

// ><DJR 17.05.99 Prevent CALLCONV becoming _System
#include <winbase.h>
#include <uconv.h>
#undef CALLCONV

#include <os2win.h>
#include <ole2ver.h>
#include <ole.h>

#include "wine/obj_base.h"
#include "wine/obj_misc.h"
#include "wine/obj_inplace.h"
#include "wine/obj_dataobject.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_marshal.h"
#include "wine/obj_moniker.h"
#include "wine/obj_clientserver.h"
#include "wine/obj_dragdrop.h"

//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CLSIDFromProgID(LPCOLESTR lpszProgID, LPCLSID pclsid)
{
   dprintf(("CLSIDFromProgID- stub\n"));
   return OLE_ERROR_GENERIC;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CLSIDFromString(LPCOLESTR lpsz, LPCLSID pclisid)
{
   dprintf(("CLSIDFromString- stub\n"));
   return OLE_ERROR_GENERIC;
}
//*******************************************************************************
//*******************************************************************************
DWORD WIN32API CoBuildVersion()
{
   dprintf(("CoBuildVersion, obsolete\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoCreateFreeThreadedMarshaler(LPUNKNOWN punkOuter,
                                                  LPUNKNOWN *ppunkMarshaler)
{
   dprintf(("CoCreateFreeThreadedMarshaler- stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoCreateGuid(GUID *pguid)
{
   dprintf(("CoCreateGuid - partial\n"));
   memset(pguid, 0, sizeof(GUID));      //TODO: should be random GUID
   return S_OK;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter,
                                     DWORD dwClsContext, REFIID riid,
                                     LPVOID *ppv)
{
   dprintf(("CoCreateInstance - stub\n"));
   return REGDB_E_CLASSNOTREG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoDisconnectObject(IUnknown *pUnk, DWORD dwReserved)
{
   dprintf(("CoDisconnectObject - stub\n"));
   return S_OK;
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API CoDosDateTimeToFileTime(WORD nDosDate, WORD nDosTime,
                                         FILETIME *lpFileTime)
{
   dprintf(("CoDosDateTimeToFileTime, redundant\n"));
   return DosDateTimeToFileTime(nDosDate, nDosTime, lpFileTime);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoFileTimeNow(FILETIME *lpFileTime)
{
 SYSTEMTIME systime;

   dprintf(("CoFileTimeNow\n"));
   GetSystemTime(&systime);
   return SystemTimeToFileTime(&systime, lpFileTime);
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API CoFileTimeToDosDateTime(FILETIME *lpFileTime, LPWORD lpDosDate,
                                         LPWORD lpDosTime)
{
   dprintf(("CoFileTimeToDosDateTime\n"));
   return FileTimeToDosDateTime(lpFileTime, lpDosDate, lpDosTime);
}
//*******************************************************************************
//Frees all libs loaded with CoLoadLibrary
//*******************************************************************************
void WIN32API CoFreeAllLibraries()
{
   dprintf(("CoFreeAllLibraries - stub\n"));
   return ;
}
//*******************************************************************************
//*******************************************************************************
void WIN32API CoFreeLibrary(HINSTANCE hInst)
{
   dprintf(("CoFreeLibrary\n"));
   FreeLibrary(hInst);
}
//*******************************************************************************
//*******************************************************************************
void WIN32API CoFreeUnusedLibraries()
{
   dprintf(("CoFreeUnusedLibraries - stub\n"));
   return ;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetCallerTID()
{
   dprintf(("CoGetCallerTID, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
//HRESULT WIN32API CoGetClassObject(REFCLSID rclsid, DWORD dwClsContext, COSERVERINFO *pServerInfo, REFIID riid, LPVOID *ppv)
HRESULT WIN32API CoGetClassObject(REFCLSID rclsid, DWORD dwClsContext, LPVOID pServerInfo, REFIID riid, LPVOID *ppv)
{
   dprintf(("CoGetClassObject - stub\n"));
   return REGDB_E_CLASSNOTREG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetCurrentLogicalThreadId()
{
   dprintf(("CoGetCurrentLogicalThreadId, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
DWORD WIN32API CoGetCurrentProcess()
{
   dprintf(("CoGetCurrentProcess - stub\n"));
   return 666;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetInterfaceAndReleaseStream(LPSTREAM pStm, REFIID riid,
                                                   LPVOID *ppv)
{
   dprintf(("CoGetInterfaceAndReleaseStream - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetMalloc(DWORD dwMemContext, LPMALLOC *ppMalloc)
{
   dprintf(("CoGetMalloc - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetMarshalSizeMax(ULONG *pulSize, REFIID riid, IUnknown *pUnk,
                                        DWORD dwDestContext, LPVOID pvDestContext,
                                        DWORD mshlflags)
{
   dprintf(("CoGetMarshalSizeMax - stub\n"));
   return CO_E_NOTINITIALIZED;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetPSClsid(REFIID riid, CLSID *pclsid)
{
   dprintf(("CoGetPSClsid - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetStandardMarshal(REFIID riid, IUnknown *pUnk, DWORD dwDestContext,
                                         LPVOID pvDestContext, DWORD mshlflags,
                                         LPMARSHAL *ppMarshal)
{
   dprintf(("CoGetStandardMarshal - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetState()
{
   dprintf(("CoGetState, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoGetTreatAsClass(REFCLSID clsidOld, LPCLSID pclsidNew)
{
   dprintf(("CoGetTreatAsClass - stub\n"));
   return S_FALSE;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoInitialize(LPVOID lpReserved)
{
   dprintf(("CoInitialize, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoInitializeWOW()
{
   dprintf(("CoInitializeWOW, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API CoIsHandlerConnected(LPUNKNOWN pUnk)
{
   dprintf(("CoIsHandlerConnected - stub\n"));
   return FALSE;
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API CoIsOle1Class(REFCLSID rclsid)
{
   dprintf(("CoIsOle1Class - stub\n"));
   return S_FALSE;
}
//*******************************************************************************
//*******************************************************************************
HINSTANCE WIN32API CoLoadLibrary(LPSTR lpszLibName, BOOL bAutoFree)
{
   dprintf(("CoLoadLibrary\n"));
   return LoadLibraryA(lpszLibName);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoLockObjectExternal(IUnknown *pUnk, BOOL fLock, BOOL fLastUnlockReleases)
{
   dprintf(("CoLockObjectExternal - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoMarshalHresult(IStream *pStm, HRESULT hresult)
{
   dprintf(("CoMarshalHresult - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoMarshalInterThreadInterfaceInStream(REFIID riid,
                                                          LPUNKNOWN pUnk,
                                                          LPSTREAM *ppStm)
{
   dprintf(("CoMarshalInterThreadInterfaceInStream - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoMarshalInterface(IStream *pStm, REFIID riid, IUnknown *pUnk,
                                       DWORD dwDestContext, void *pvDestContext,
                                       DWORD mshlflags)
{
   dprintf(("CoMarshalInterface - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoQueryReleaseObject()
{
   dprintf(("CoQueryReleaseObject, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRegisterClassObject(REFCLSID rclsid, IUnknown *pUnk,
                                          DWORD dwClsContext, DWORD flags,
                                          LPDWORD lpdwRegister)
{
   dprintf(("CoRegisterClassObject - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRegisterMallocSpy(LPMALLOCSPY pMallocSpy)
{
   dprintf(("CoRegisterMallocSpy - stub\n"));
   return CO_E_OBJISREG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRegisterMessageFilter(LPMESSAGEFILTER lpMessageFilter,
                                            LPMESSAGEFILTER *lplpMessageFilter)
{
   dprintf(("CoRegisterMessageFilter - stub\n"));
   return S_FALSE;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoReleaseMarshalData(IStream *pStm)
{
   dprintf(("CoReleaseMarshalData - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRevokeClassObject(DWORD dwRegister)
{
   dprintf(("CoRevokeClassObject - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoRevokeMallocSpy()
{
   dprintf(("CoRevokeMallocSpy - stub\n"));
   return E_ACCESSDENIED;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoSetState()
{
   dprintf(("CoSetState, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
LPVOID WIN32API CoTaskMemAlloc(ULONG cb)
{
   dprintf(("CoTaskMemAlloc\n"));
   return malloc(cb);
}
//*******************************************************************************
//*******************************************************************************
void WIN32API CoTaskMemFree(void *pv)
{
   dprintf(("CoTaskMemFree\n"));
   free(pv);
   return;
}
//*******************************************************************************
//*******************************************************************************
LPVOID WIN32API CoTaskMemRealloc(LPVOID pv, ULONG cb)
{
   dprintf(("CoTaskMemRealloc - stub\n"));
   return NULL;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoTreatAsClass(REFCLSID clsidOld, REFCLSID clsidNew)
{
   dprintf(("CoTreatAsClass - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
void WIN32API CoUninitialize()
{
   dprintf(("CoUninitialize - stub\n"));
   return ;
}
//*******************************************************************************
//*******************************************************************************
void WIN32API CoUnloadingWOW()
{
   dprintf(("CoUnloadingWOW, UNKNOWN API - stub\n"));
   return ;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoUnmarshalHresult(LPSTREAM pStm, HRESULT *phresult)
{
   dprintf(("CoUnmarshalHresult - stub\n"));
   return S_OK;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CoUnmarshalInterface(IStream *pSTm, REFIID riid, void **ppv)
{
   dprintf(("CoUnmarshalInterface - stub\n"));
   return S_OK;
}

//******************************************************************************
//******************************************************************************
void WIN32API OleUninitialize(void)
{
  dprintf(("OleUninitialize - stub\n"));
  return;
}
//******************************************************************************
//******************************************************************************
HRESULT WIN32API BindMoniker(LPMONIKER pmk, DWORD grfOpt, REFIID iidResult,
                                LPVOID *ppvResult)
{
   dprintf(("BindMoniker- stub\n"));
   return MK_E_NOOBJECT;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateAntiMoniker(LPMONIKER *ppmk)
{
   dprintf(("CreateAntiMoniker - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateBindCtx(DWORD reserved, LPBC *ppbc)
{
   dprintf(("CreateBindCtx - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateDataAdviseHolder(IDataAdviseHolder **ppDAHolder)
{
   dprintf(("CreateDataAdviseHolder - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateDataCache(LPUNKNOWN pUnkOuter, REFCLSID rclsid,
                                    REFIID riid, LPVOID *ppvObj)
{
   dprintf(("CreateDataCache - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateFileMoniker(LPCOLESTR lpszPathName, LPMONIKER *ppmk)
{
   dprintf(("CreateFileMoniker - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateGenericComposite(LPMONIKER pmkFirst, LPMONIKER pmkRest,
                                           LPMONIKER *ppmkComposite)
{
   dprintf(("CreateGenericComposite - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateILockBytesOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease,
                                              ILockBytes **ppkLkbyt)
{
   dprintf(("CreateILockBytesOnHGlobal - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateItemMoniker(LPCOLESTR lpszDelim, LPCOLESTR lpszItem, LPMONIKER *ppmk)
{
   dprintf(("CreateItemMoniker - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateOleAdviseHolder(IOleAdviseHolder ppOAHolder)
{
   dprintf(("CreateOleAdviseHolder - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreatePointerMoniker(LPUNKNOWN punk, LPMONIKER *ppmk)
{
   dprintf(("CreatePointerMoniker - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API CreateStreamOnHGlobal(HGLOBAL hGlobal, BOOL fDeleteOnRelease,
                                          LPSTREAM *ppstm)
{
   dprintf(("CreateStreamOnHGlobal - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API DllDebugObjectRPCHook()
{
   dprintf(("DllDebugObjectRPCHook, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
   dprintf(("DllGetClassObject - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API DllGetClassObjectWOW()
{
   dprintf(("DllGetClassObjectWOW, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API DoDragDrop(IDataObject *pDataObject, IDropSource *pDropSource,
                               DWORD dwOKEffect, DWORD *pdwEffect)
{
   dprintf(("DoDragDrop - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API EnableHookObject()
{
   dprintf(("EnableHookObject, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetClassFile(LPCWSTR szFileName, CLSID *pclsid)
{
   dprintf(("GetClassFile - stub\n"));
   return MK_E_CANTOPENFILE;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetConvertStg(IStorage *pStg)
{
   dprintf(("GetConvertStg - stub\n"));
   return STG_E_ACCESSDENIED;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetDocumentBitStg()
{
   dprintf(("GetDocumentBitStg, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetHGlobalFromILockBytes(ILockBytes *pLkbyt, HGLOBAL *phglobal)
{
   dprintf(("GetHGlobalFromILockBytes - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetHGlobalFromStream(IStream *pstm, HGLOBAL *phglobal)
{
   dprintf(("GetHGlobalFromStream - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API GetHookInterface()
{
   dprintf(("GetHookInterface, UNKNOWN API - stub\n"));
   return 0;
}
//*******************************************************************************
//*******************************************************************************
//HRESULT WIN32API GetRunningObjectTable(DWORD reserved, LPRUNNINGOBJECTTABLE *pprot)
HRESULT WIN32API GetRunningObjectTable(DWORD reserved, LPVOID *pprot)
{
   dprintf(("GetRunningObjectTable - stub\n"));
   return E_UNEXPECTED;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API IIDFromString(LPSTR lpsz, LPIID lpiid)
{
   dprintf(("IIDFromString - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API IsAccelerator(HACCEL hAccel, INT cAccelEntries, LPMSG lpMsg,
                               WORD *lpwCmd)
{
   dprintf(("IsAccelerator - stub\n"));
   return FALSE;
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API CONCRETE_IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
   dprintf(("IsEqualGUID - stub\n"));
   return FALSE;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API IsValidIid(DWORD Iid)
{
   dprintf(("IsValidIid, obsolete - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API IsValidInterface(DWORD interface_)
{
   dprintf(("IsValidInterface, obsolete - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API IsValidPtrIn(DWORD ptrIn)
{
   dprintf(("IsValidPtrIn, obsolete - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API IsValidPtrOut(DWORD ptrOut)
{
   dprintf(("IsValidPtrOut, obsolete - stub\n"));
   return E_INVALIDARG;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API MkParseDisplayName(LPBC pbc, LPSTR szUserName, ULONG *lpchEaten,
                                        LPMONIKER *ppmk)
{
   dprintf(("MkParseDisplayName - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API MonikerCommonPrefixWith(LPMONIKER pmkThis, LPMONIKER pmkOther,
                                            LPMONIKER *ppmkCommon)
{
   dprintf(("MonikerCommonPrefixWith - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API MonikerRelativePathTo(LPMONIKER pmkSrc, LPMONIKER pmkDest,
                                          LPMONIKER *ppmkRelPath, BOOL dwReserved )
{
   dprintf(("MonikerRelativePathTo - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
DWORD WIN32API OleBuildVersion()
{
   dprintf(("OleBuildVersion, obsolete - stub\n"));
   return 666;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleConvertIStorageToOLESTREAM(IStorage *pStg, LPOLESTREAM lpolestream)
{
   dprintf(("OleConvertIStorageToOLESTREAM - stub\n"));
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
   dprintf(("OleConvertIStorageToOLESTREAMEx - stub\n"));
   return(E_INVALIDARG);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleConvertOLESTREAMToIStorage(LPOLESTREAM lpolestream,
                                                  IStorage *pstg,
                                                  const DVTARGETDEVICE *ptd)
{
   dprintf(("OleConvertOLESTREAMToIStorage - stub\n"));
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
   dprintf(("OleConvertOLESTREAMToIStorageEx - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreate(REFCLSID rclsid, REFIID riid, DWORD renderopt,
                              FORMATETC *pFormatEtc, IOleClientSite *pClientSite,
                              IStorage *pStg, void **ppvObject)
{
   dprintf(("OleCreate - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateDefaultHandler(REFCLSID clsid, LPUNKNOWN pUnkOuter,
                                            REFIID riid, LPVOID *ppvObj)
{
   dprintf(("OleCreateDefaultHandler - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateEmbeddingHelper(REFCLSID clsid, LPUNKNOWN pUnkOuter,
                                             DWORD flags, LPCLASSFACTORY pCF,
                                             REFIID riid, LPVOID *ppvObj)
{
   dprintf(("OleCreateEmbeddingHelper - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateFromData(LPDATAOBJECT pSrcDataObj, REFIID riid,
                                      DWORD renderopt, LPFORMATETC pFormatEtc,
                                      LPOLECLIENTSITE pClientSite, LPSTORAGE pStg,
                                      LPVOID *ppvObj)
{
   dprintf(("OleCreateFromData - stub\n"));
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
   dprintf(("OleCreateFromFile - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateLink(LPMONIKER lpmkLinkSrc, REFIID riid,
                                  DWORD renderopt, LPFORMATETC pFormatEtc,
                                  LPOLECLIENTSITE lpClientSite, LPSTORAGE pStg,
                                  LPVOID *ppvObj)
{
   dprintf(("OleCreateLink - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateLinkFromData(LPDATAOBJECT pSrcDataObj, REFIID riid,
                                          DWORD renderopt, LPFORMATETC pFormatEtc,
                                          LPOLECLIENTSITE pClientSite, LPSTORAGE pStg,
                                          LPVOID *ppvObj)
{
   dprintf(("OleCreateLinkFromData - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateLinkToFile(LPCOLESTR lpszFileName, REFIID riid, DWORD renderopt,
                                        LPFORMATETC pFormatEtc, IOleClientSite *pClientSite,
                                        IStorage *pStg, void **ppvObj)
{
   dprintf(("OleCreateLinkToFile - stub\n"));
   return(STG_E_FILENOTFOUND);
}
//*******************************************************************************
//*******************************************************************************
HOLEMENU WIN32API OleCreateMenuDescriptor(HMENU hmenuCombined, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
   dprintf(("OleCreateMenuDescriptor - stub\n"));
   return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleCreateStaticFromData(LPDATAOBJECT pSrcDataObj, REFIID riid,
                                            DWORD renderopt, LPFORMATETC pFormatEtc,
                                            LPOLECLIENTSITE pClientSite, LPSTORAGE pStg,
                                            LPVOID *ppvObj)
{
   dprintf(("OleCreateStaticFromData - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleDestroyMenuDescriptor(HOLEMENU holemenu)
{
   dprintf(("OleDestroyMenuDescriptor - stub\n"));
   return E_OUTOFMEMORY;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleDoAutoConvert(IStorage *pStg, LPCLSID pClsidNew)
{
   dprintf(("OleDoAutoConvert - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleDraw(IUnknown *pUnk, DWORD dwAspect, HDC hdcDraw,
                            LPCRECT lprcBounds)
{
   dprintf(("OleDraw - stub\n"));
   return(E_INVALIDARG);
}
//*******************************************************************************
//*******************************************************************************
HANDLE WIN32API OleDuplicateData(HANDLE hSrc, CLIPFORMAT cfFormat, UINT uiFlags)
{
   dprintf(("OleDuplicateData - stub\n"));
   return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleFlushClipboard()
{
   dprintf(("OleFlushClipboard - stub\n"));
   return(S_OK);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleGetAutoConvert(REFCLSID clsidOld, LPCLSID pClsidNew)
{
   dprintf(("OleGetAutoConvert - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleGetClipboard(IDataObject **ppDataObj)
{
   dprintf(("OleGetClipboard - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HGLOBAL WIN32API OleGetIconOfClass(REFCLSID rclsid, LPOLESTR lpszLabel, BOOL fUseTypeAsLabel)
{
   dprintf(("OleGetIconOfClass - stub\n"));
   return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HGLOBAL WIN32API OleGetIconOfFile(LPOLESTR lpszPath, BOOL fUseTypeAsLabel)
{
   dprintf(("OleGetIconOfFile - stub\n"));
   return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleInitialize(LPVOID pvReserved)
{
   dprintf(("OleInitialize - stub\n"));
//   return(E_OUTOFMEMORY);
   return(0);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleInitializeWOW()
{
   dprintf(("OleInitializeWOW, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleIsCurrentClipboard(IDataObject *pDataObject)
{
   dprintf(("OleIsCurrentClipboard - stub\n"));
   return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
BOOL WIN32API OleIsRunning(LPOLEOBJECT pObject)
{
   dprintf(("OleIsRunning - stub\n"));
   return(FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleLoad(IStorage *pStg, REFIID riid, IOleClientSite *pClientSite,
                            LPVOID *ppvObj)
{
   dprintf(("OleLoad - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleLoadFromStream(IStream *pStm, REFIID riid, void *ppvObj)
{
   dprintf(("OleLoadFromStream - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleLockRunning(LPUNKNOWN pUnknown, BOOL fLock, BOOL fLastUnlockCloses)
{
   dprintf(("OleLockRunning - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HGLOBAL WIN32API OleMetafilePictFromIconAndLabel(HICON hIcon, LPOLESTR lpszLabel,
                                                    LPOLESTR lpszSourceFile,
                                                    UINT iIconIndex)
{
   dprintf(("OleMetafilePictFromIconAndLabel - stub\n"));
   return(NULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleNoteObjectVisible(LPUNKNOWN pUnknown, BOOL fVisible)
{
   dprintf(("OleNoteObjectVisible - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleQueryCreateFromData(IDataObject *pSrcDataObject)
{
   dprintf(("OleQueryCreateFromData - stub\n"));
   return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleQueryLinkFromData(IDataObject *pSrcDataObject)
{
   dprintf(("OleQueryLinkFromData - stub\n"));
   return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRegEnumFormatEtc(REFCLSID clsid, DWORD dwDirection,
                                        LPENUMFORMATETC *ppenumFormatetc)
{
   dprintf(("OleRegEnumFormatEtc - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRegEnumVerbs(REFCLSID clsid, LPENUMOLEVERB *ppenumOleVerb)
{
   dprintf(("OleRegEnumVerbs - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRegGetMiscStatus(REFCLSID clsid, DWORD dwAspect, DWORD *pdwStatus)
{
   dprintf(("OleRegGetMiscStatus - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRegGetUserType(REFCLSID clsid, DWORD dwFormOfType,
                                      LPOLESTR *pszUserType)
{
   dprintf(("OleRegGetUserType - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleRun(LPUNKNOWN pUnknown)
{
   dprintf(("OleRun - stub\n"));
   return(E_UNEXPECTED);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSave(IPersistStorage *pPS, IStorage *pStg, BOOL fSameAsLoad)
{
   dprintf(("OleSave - stub\n"));
   return(STG_E_MEDIUMFULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSaveToStream(IPersistStorage *pPS, IStream *pStm)
{
   dprintf(("OleSaveToStream - stub\n"));
   return(STG_E_MEDIUMFULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSetAutoConvert(REFCLSID clsidOld, REFCLSID clsidNew)
{
   dprintf(("OleSetAutoConvert - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSetClipboard(IDataObject *pDataObj)
{
   dprintf(("OleSetClipboard - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSetContainedObject(LPUNKNOWN pUnknown, BOOL fContained)
{
   dprintf(("OleSetContainedObject - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleSetMenuDescriptor(HOLEMENU holemenu, HWND hwndFrame,
                                         HWND hwndActiveObject,
                                         LPOLEINPLACEFRAME lpFrame,
                                         LPOLEINPLACEACTIVEOBJECT lpActiveObj)
{
   dprintf(("OleSetMenuDescriptor - stub\n"));
   return(E_FAIL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OleTranslateAccelerator(LPOLEINPLACEFRAME lpFrame,
                                            LPOLEINPLACEFRAMEINFO lpFrameInfo,
                                            LPMSG lpmsg)
{
   dprintf(("OleTranslateAccelerator - stub\n"));
   return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API OpenOrCreateStream()
{
   dprintf(("OpenOrCreateStream, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ProgIDFromCLSID(REFCLSID clsid, LPOLESTR *lplpszProgID)
{
   dprintf(("ProgIDFromCLSID - stub\n"));
   return(REGDB_E_CLASSNOTREG);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadClassStg(IStorage *pStg, CLSID *pclsid)
{
   dprintf(("ReadClassStg - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadClassStm(IStream *pStm, CLSID *pclsid)
{
   dprintf(("ReadClassStm - stub\n"));
   return(STG_E_READFAULT);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadFmtUserTypeStg(IStorage *pStg, CLIPFORMAT *pcf,
                                       LPWSTR *lplpszUserType)
{
   dprintf(("ReadFmtUserTypeStg - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadOleStg()
{
   dprintf(("ReadOleStg, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API ReadStringStream()
{
   dprintf(("ReadStringStream, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API RegisterDragDrop(HWND hwnd, IDropTarget *pDropTarget)
{
   dprintf(("RegisterDragDrop - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
void WIN32API ReleaseStgMedium(STGMEDIUM *pmedium)
{
   dprintf(("ReleaseStgMedium - stub\n"));
   return;
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API RevokeDragDrop(HWND hwnd)
{
   dprintf(("RevokeDragDrop - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API SetConvertStg(IStorage *pStg, BOOL fConvert)
{
   dprintf(("SetConvertStg - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgCreateDocfile(const WCHAR *pwcsName, DWORD grfMode,
                                     DWORD reserved, IStorage **ppstgOpen)
{
   dprintf(("StgCreateDocfile - stub\n"));
   return(STG_E_INSUFFICIENTMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API SetDocumentBitStg()
{
   dprintf(("SetDocumentBitStg, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgCreateDocfileOnILockBytes(ILockBytes *plkbyt, DWORD grfMode,
                                                 DWORD reserved, IStorage **ppstgOpen)
{
   dprintf(("StgCreateDocfileOnILockBytes - stub\n"));
   return(STG_E_INSUFFICIENTMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgIsStorageFile(const WCHAR *pwcsName)
{
   dprintf(("StgIsStorageFile - stub\n"));
   return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgIsStorageILockBytes(ILockBytes *plkbyt)
{
   dprintf(("StgIsStorageILockBytes - stub\n"));
   return(S_FALSE);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgOpenStorage(const WCHAR *pwcsName, IStorage *pstgPriority,
                                   DWORD grfMode, SNB snbExclude, DWORD reserved,
                                   IStorage **ppstgOpen)
{
   dprintf(("StgOpenStorage - stub\n"));
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
   dprintf(("StgOpenStorageOnILockBytes - stub\n"));
   return(STG_E_INSUFFICIENTMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API StgSetTimes(WCHAR const *lpszName, FILETIME const *pctime,
                                FILETIME const *patime, FILETIME const *pmtime)
{
   dprintf(("StgSetTimes - stub\n"));
   return(STG_E_FILENOTFOUND);
}
//*******************************************************************************
//Freed in CoTaskMemFree
//*******************************************************************************
HRESULT WIN32API StringFromCLSID(REFCLSID rclsid, LPOLESTR *ppsz)
{
 char   szclass[64];
 WCHAR *szclsid = (WCHAR *)malloc(64*sizeof(WCHAR));

   dprintf(("StringFromCLSID\n"));

   sprintf(szclass, "[%8X-%4X-%4X-%4X-%4X%8X]", rclsid.Data1, rclsid.Data2, rclsid.Data3,
           *(WORD *)rclsid.Data4, *(WORD *)&rclsid.Data4[2], *(DWORD *)&rclsid.Data4[4]);
   AsciiToUnicode(szclass, szclsid);
   *ppsz = (LPOLESTR)szclsid;
   return(S_OK);
}
//*******************************************************************************
//*******************************************************************************
int WIN32API StringFromGUID2(REFGUID rguid, LPOLESTR lpsz, int cbMax)
{
 char szclsid[64];

   dprintf(("StringFromGUID2\n"));
   if(cbMax < 55)
        return(0);

   sprintf(szclsid, "[%8X-%4X-%4X-%4X-%4X%8X]", rguid.Data1, rguid.Data2, rguid.Data3,
           *(WORD *)rguid.Data4, *(WORD *)&rguid.Data4[2], *(DWORD *)&rguid.Data4[4]);
   AsciiToUnicode(szclsid, lpsz);
   return(55);  //including 0 terminator
}
//*******************************************************************************
//Freed in CoTaskMemFree
//*******************************************************************************
HRESULT WIN32API StringFromIID(REFIID riid, LPOLESTR *lplpsz)
{
 char   szclass[64];
 WCHAR *szclsid = (WCHAR *)malloc(64*sizeof(WCHAR));

   dprintf(("StringFromIID\n"));

   sprintf(szclass, "[%8X-%4X-%4X-%4X-%4X%8X]", riid.Data1, riid.Data2, riid.Data3,
           *(WORD *)riid.Data4, *(WORD *)&riid.Data4[2], *(DWORD *)&riid.Data4[4]);

   AsciiToUnicode(szclass, szclsid);
   *lplpsz = (LPOLESTR)szclsid;
   return(S_OK);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API UtConvertDvtd16toDvtd32()
{
   dprintf(("UtConvertDvtd16toDvtd32, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API UtConvertDvtd32toDvtd16()
{
   dprintf(("UtConvertDvtd32toDvtd16, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API UtGetDvtd16Info()
{
   dprintf(("UtGetDvtd16Info, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API UtGetDvtd32Info()
{
   dprintf(("UtGetDvtd32Info, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteClassStg(IStorage *pStg, REFCLSID rclsid)
{
   dprintf(("WriteClassStg - stub\n"));
   return(STG_E_MEDIUMFULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteClassStm(IStream *pStm, REFCLSID rclsid)
{
   dprintf(("WriteClassStm - stub\n"));
   return(STG_E_MEDIUMFULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteFmtUserTypeStg(IStorage *pStg, CLIPFORMAT cf, LPWSTR *lpszUserType)
{
   dprintf(("WriteFmtUserTypeStg - stub\n"));
   return(STG_E_MEDIUMFULL);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteOleStg()
{
   dprintf(("WriteOleStg, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
HRESULT WIN32API WriteStringStream()
{
   dprintf(("WriteStringStream, UNKNOWN API - stub\n"));
   return(E_OUTOFMEMORY);
}
//*******************************************************************************
//*******************************************************************************
