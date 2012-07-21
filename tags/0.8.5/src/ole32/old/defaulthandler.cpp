/* $Id: defaulthandler.cpp,v 1.1 2001-04-26 19:26:09 sandervl Exp $ */
/* 
 *  OLE 2 default object handler
 * 
 * 21/9/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Direct port of Wine Implementation
 *   Copyright 1999  Francis Beaudet
 *
 * NOTES:
 *    The OLE2 default object handler supports a whole whack of
 *    interfaces including:
 *       IOleObject, IDataObject, IPersistStorage, IViewObject2,
 *       IRunnableObject, IOleCache2, IOleCacheControl and much more.
 *
 *    All the implementation details are taken from: Inside OLE
 *    second edition by Kraig Brockschmidt,
 *
 * TODO
 * - This implementation of the default handler does not launch the
 *   server in the DoVerb, Update, GetData, GetDataHere and Run
 *   methods. When it is fixed to do so, all the methods will have
 *   to be  revisited to allow delegating to the running object
 *
 * - All methods in the class that use the class ID should be
 *   aware that it is possible for a class to be treated as
 *   another one and go into emulation mode. Nothing has been
 *   done in This area.
 *
 * - Some functions still return E_NOTIMPL they have to be 
 *   implemented. Most of those are related to the running of the
 *   actual server.
 *
 * - All the methods related to notification and advise sinks are
 *   in place but no notifications are sent to the sinks yet.
 */

#include "ole32.h"
#include "heapstring.h"
#include "oleauto.h"
#include "debugtools.h"

#include <assert.h>

DEFAULT_DEBUG_CHANNEL(ole)

/****************************************************************************
 * DefaultHandler
 *
 */
struct DefaultHandler
{
  /*
   * List all interface VTables here
   */
  ICOM_VTABLE(IOleObject)*      lpvtbl1; 
  ICOM_VTABLE(IUnknown)*        lpvtbl2;
  ICOM_VTABLE(IDataObject)*     lpvtbl3;
  ICOM_VTABLE(IRunnableObject)* lpvtbl4;

  /*
   * Reference count of This object
   */
  ULONG ref;

  /*
   * IUnknown implementation of the outer object.
   */
  IUnknown* outerUnknown;

  /*
   * Class Id that This handler object represents.
   */
  CLSID clsid;

  /*
   * IUnknown implementation of the datacache.
   */
  IUnknown* dataCache;

  /*
   * Client site for the embedded object.
   */
  IOleClientSite* clientSite;

  /*
   * The IOleAdviseHolder maintains the connections
   * on behalf of the default handler.
   */
  IOleAdviseHolder* oleAdviseHolder;

  /*
   * The IDataAdviseHolder maintains the data 
   * connections on behalf of the default handler.
   */
  IDataAdviseHolder* dataAdviseHolder;

  /*
   * Name of the container and object contained
   */
  LPWSTR containerApp; 
  LPWSTR containerObj;

};

typedef struct DefaultHandler DefaultHandler;

/*
 * Here, I define utility macros to help with the casting of the 
 * "This" parameter.
 * There is a version to accomodate all of the VTables implemented
 * by This object.
 */
#define _ICOM_THIS_From_IOleObject(class,name)       class* This = (class*)name;
#define _ICOM_THIS_From_NDIUnknown(class, name)      class* This = (class*)(((char*)name)-sizeof(void*)); 
#define _ICOM_THIS_From_IDataObject(class, name)     class* This = (class*)(((char*)name)-2*sizeof(void*)); 
#define _ICOM_THIS_From_IRunnableObject(class, name) class* This = (class*)(((char*)name)-3*sizeof(void*)); 

/*
 * Prototypes for the methods of the DefaultHandler class.
 */
static DefaultHandler* DefaultHandler_Construct(REFCLSID  clsid,
						LPUNKNOWN pUnkOuter);
static void            DefaultHandler_Destroy(DefaultHandler* ptrToDestroy);

/*
 * Prototypes for the methods of the DefaultHandler class
 * that implement non delegating IUnknown methods.
 */
static HRESULT WINAPI DefaultHandler_NDIUnknown_QueryInterface(
            IUnknown*      iface,
            REFIID         riid,
            void**         ppvObject);
static ULONG WINAPI DefaultHandler_NDIUnknown_AddRef( 
            IUnknown*      iface);
static ULONG WINAPI DefaultHandler_NDIUnknown_Release( 
            IUnknown*      iface);

/*
 * Prototypes for the methods of the DefaultHandler class
 * that implement IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_QueryInterface(
            IOleObject*      iface,
            REFIID           riid,
            void**           ppvObject);
static ULONG WINAPI DefaultHandler_AddRef( 
            IOleObject*        iface);
static ULONG WINAPI DefaultHandler_Release( 
            IOleObject*        iface);
static HRESULT WINAPI DefaultHandler_SetClientSite(
	    IOleObject*        iface,
	    IOleClientSite*    pClientSite);
static HRESULT WINAPI DefaultHandler_GetClientSite(
	    IOleObject*        iface,
	    IOleClientSite**   ppClientSite);
static HRESULT WINAPI DefaultHandler_SetHostNames(
	    IOleObject*        iface,
	    LPCOLESTR          szContainerApp, 
	    LPCOLESTR          szContainerObj);
static HRESULT WINAPI DefaultHandler_Close(
	    IOleObject*        iface, 
	    DWORD              dwSaveOption);
static HRESULT WINAPI DefaultHandler_SetMoniker(
	    IOleObject*        iface, 
	    DWORD              dwWhichMoniker,
	    IMoniker*          pmk);
static HRESULT WINAPI DefaultHandler_GetMoniker(
	    IOleObject*        iface,
	    DWORD              dwAssign,
	    DWORD              dwWhichMoniker,
	    IMoniker**         ppmk);
static HRESULT WINAPI DefaultHandler_InitFromData(
	    IOleObject*        iface, 
	    IDataObject*       pDataObject, 
	    BOOL               fCreation,
	    DWORD              dwReserved);
static HRESULT WINAPI DefaultHandler_GetClipboardData(
	    IOleObject*        iface, 
	    DWORD              dwReserved, 
	    IDataObject**      ppDataObject);
static HRESULT WINAPI DefaultHandler_DoVerb(
	    IOleObject*        iface, 
	    LONG               iVerb, 
	    LPMSG              lpmsg, 
	    IOleClientSite*    pActiveSite, 
	    LONG               lindex, 
	    HWND               hwndParent, 
	    LPCRECT            lprcPosRect);
static HRESULT WINAPI DefaultHandler_EnumVerbs(
	    IOleObject*        iface, 
	    IEnumOLEVERB**     ppEnumOleVerb);
static HRESULT WINAPI DefaultHandler_Update(
	    IOleObject*        iface);
static HRESULT WINAPI DefaultHandler_IsUpToDate(
	    IOleObject*        iface);
static HRESULT WINAPI DefaultHandler_GetUserClassID(
	    IOleObject*        iface, 
	    CLSID*             pClsid);
static HRESULT WINAPI DefaultHandler_GetUserType(
	    IOleObject*        iface, 
	    DWORD              dwFormOfType, 
	    LPOLESTR*          pszUserType);
static HRESULT WINAPI DefaultHandler_SetExtent(
	    IOleObject*        iface, 
	    DWORD              dwDrawAspect, 
	    SIZEL*             psizel);
static HRESULT WINAPI DefaultHandler_GetExtent(
	    IOleObject*        iface, 
	    DWORD              dwDrawAspect, 
	    SIZEL*             psizel);
static HRESULT WINAPI DefaultHandler_Advise(
	    IOleObject*        iface, 
	    IAdviseSink*       pAdvSink, 
	    DWORD*             pdwConnection);
static HRESULT WINAPI DefaultHandler_Unadvise(
	    IOleObject*        iface, 
	    DWORD              dwConnection);
static HRESULT WINAPI DefaultHandler_EnumAdvise(
	    IOleObject*        iface, 
	    IEnumSTATDATA**    ppenumAdvise);
static HRESULT WINAPI DefaultHandler_GetMiscStatus(
	    IOleObject*        iface, 
	    DWORD              dwAspect, 
	    DWORD*             pdwStatus);
static HRESULT WINAPI DefaultHandler_SetColorScheme(
	    IOleObject*           iface,
	    LOGPALETTE*	       pLogpal);

/*
 * Prototypes for the methods of the DefaultHandler class
 * that implement IDataObject methods.
 */
static HRESULT WINAPI DefaultHandler_IDataObject_QueryInterface(
            IDataObject*     iface,
            REFIID           riid,
            void**           ppvObject);
static ULONG WINAPI DefaultHandler_IDataObject_AddRef( 
            IDataObject*     iface);
static ULONG WINAPI DefaultHandler_IDataObject_Release( 
            IDataObject*     iface);
static HRESULT WINAPI DefaultHandler_GetData(
	    IDataObject*     iface,
	    LPFORMATETC      pformatetcIn, 
	    STGMEDIUM*       pmedium);
static HRESULT WINAPI DefaultHandler_GetDataHere(
	    IDataObject*     iface, 
	    LPFORMATETC      pformatetc,
	    STGMEDIUM*       pmedium);
static HRESULT WINAPI DefaultHandler_QueryGetData(
	    IDataObject*     iface,
	    LPFORMATETC      pformatetc);
static HRESULT WINAPI DefaultHandler_GetCanonicalFormatEtc(
	    IDataObject*     iface, 
	    LPFORMATETC      pformatectIn, 
	    LPFORMATETC      pformatetcOut);
static HRESULT WINAPI DefaultHandler_SetData(
	    IDataObject*     iface,
	    LPFORMATETC      pformatetc, 
	    STGMEDIUM*       pmedium, 
	    BOOL             fRelease);
static HRESULT WINAPI DefaultHandler_EnumFormatEtc(
	    IDataObject*     iface,       
	    DWORD            dwDirection,
	    IEnumFORMATETC** ppenumFormatEtc);
static HRESULT WINAPI DefaultHandler_DAdvise(
	    IDataObject*     iface, 
	    FORMATETC*       pformatetc, 
	    DWORD            advf, 
	    IAdviseSink*     pAdvSink, 
	    DWORD*           pdwConnection);
static HRESULT WINAPI DefaultHandler_DUnadvise(
	    IDataObject*     iface,
	    DWORD            dwConnection);
static HRESULT WINAPI DefaultHandler_EnumDAdvise(
	    IDataObject*     iface,
	    IEnumSTATDATA**  ppenumAdvise);

/*
 * Prototypes for the methods of the DefaultHandler class
 * that implement IRunnableObject methods.
 */
static HRESULT WINAPI DefaultHandler_IRunnableObject_QueryInterface(
            IRunnableObject*     iface,
            REFIID               riid,
            void**               ppvObject);
static ULONG WINAPI DefaultHandler_IRunnableObject_AddRef( 
            IRunnableObject*     iface);
static ULONG WINAPI DefaultHandler_IRunnableObject_Release( 
            IRunnableObject*     iface);
static HRESULT WINAPI DefaultHandler_GetRunningClass( 
            IRunnableObject*     iface,   
	    LPCLSID              lpClsid);
static HRESULT WINAPI DefaultHandler_Run( 
            IRunnableObject*     iface,
	    IBindCtx*            pbc);
static BOOL    WINAPI DefaultHandler_IsRunning( 
            IRunnableObject*     iface);
static HRESULT WINAPI DefaultHandler_LockRunning( 
            IRunnableObject*     iface, 
	    BOOL                 fLock, 
	    BOOL                 fLastUnlockCloses);
static HRESULT WINAPI DefaultHandler_SetContainedObject( 
            IRunnableObject*     iface, 
	    BOOL                 fContained);


/*
 * Virtual function tables for the DefaultHandler class.
 */
static ICOM_VTABLE(IOleObject) DefaultHandler_IOleObject_VTable =
{
  ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
  DefaultHandler_QueryInterface,
  DefaultHandler_AddRef,
  DefaultHandler_Release,
  DefaultHandler_SetClientSite,
  DefaultHandler_GetClientSite,
  DefaultHandler_SetHostNames,
  DefaultHandler_Close,
  DefaultHandler_SetMoniker,
  DefaultHandler_GetMoniker,
  DefaultHandler_InitFromData,
  DefaultHandler_GetClipboardData,
  DefaultHandler_DoVerb,
  DefaultHandler_EnumVerbs,
  DefaultHandler_Update,
  DefaultHandler_IsUpToDate,
  DefaultHandler_GetUserClassID,
  DefaultHandler_GetUserType,
  DefaultHandler_SetExtent,
  DefaultHandler_GetExtent,
  DefaultHandler_Advise,
  DefaultHandler_Unadvise,
  DefaultHandler_EnumAdvise,
  DefaultHandler_GetMiscStatus,
  DefaultHandler_SetColorScheme
};

static ICOM_VTABLE(IUnknown) DefaultHandler_NDIUnknown_VTable =
{
  ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
  DefaultHandler_NDIUnknown_QueryInterface,
  DefaultHandler_NDIUnknown_AddRef,
  DefaultHandler_NDIUnknown_Release,
};

static ICOM_VTABLE(IDataObject) DefaultHandler_IDataObject_VTable =
{
  ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
  DefaultHandler_IDataObject_QueryInterface,
  DefaultHandler_IDataObject_AddRef,
  DefaultHandler_IDataObject_Release,
  DefaultHandler_GetData,
  DefaultHandler_GetDataHere,
  DefaultHandler_QueryGetData,
  DefaultHandler_GetCanonicalFormatEtc,
  DefaultHandler_SetData,
  DefaultHandler_EnumFormatEtc,
  DefaultHandler_DAdvise,
  DefaultHandler_DUnadvise,
  DefaultHandler_EnumDAdvise
};

static ICOM_VTABLE(IRunnableObject) DefaultHandler_IRunnableObject_VTable =
{
  ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
  DefaultHandler_IRunnableObject_QueryInterface,
  DefaultHandler_IRunnableObject_AddRef,
  DefaultHandler_IRunnableObject_Release,
  DefaultHandler_GetRunningClass,
  DefaultHandler_Run,
  DefaultHandler_IsRunning,
  DefaultHandler_LockRunning,
  DefaultHandler_SetContainedObject
};

/******************************************************************************
 * OleCreateDefaultHandler [OLE32.90]
 */
HRESULT WINAPI OleCreateDefaultHandler(
  REFCLSID  clsid,
  LPUNKNOWN pUnkOuter,
  REFIID    riid,
  LPVOID*   ppvObj)
{
  DefaultHandler* newHandler = NULL;
  HRESULT         hr         = S_OK;
  char            xclsid[50];
  char            xriid[50];

  WINE_StringFromCLSID((LPCLSID)clsid,xclsid);
  WINE_StringFromCLSID((LPCLSID)riid,xriid);

  TRACE("(%s, %p, %s, %p)\n", xclsid, pUnkOuter, xriid, ppvObj);

  /*
   * Sanity check
   */
  if (ppvObj==0)
    return E_POINTER;

  *ppvObj = 0;

  /*
   * If This handler is constructed for aggregation, make sure
   * the caller is requesting the IUnknown interface.
   * This is necessary because it's the only time the non-delegating
   * IUnknown pointer can be returned to the outside.
   */
  if ( (pUnkOuter!=NULL) && 
       (memcmp(&IID_IUnknown, riid, sizeof(IID_IUnknown)) != 0) )
    return CLASS_E_NOAGGREGATION;

  /*
   * Try to construct a new instance of the class.
   */
  newHandler = DefaultHandler_Construct(clsid, 
					pUnkOuter);

  if (newHandler == 0)
    return E_OUTOFMEMORY;

  /*
   * Make sure it supports the interface required by the caller.
   */
  hr = IUnknown_QueryInterface((IUnknown*)&(newHandler->lpvtbl2), riid, ppvObj);

  /*
   * Release the reference obtained in the constructor. If
   * the QueryInterface was unsuccessful, it will free the class.
   */
  IUnknown_Release((IUnknown*)&(newHandler->lpvtbl2));

  return hr;
}

/*********************************************************
 * Methods implementation for the DefaultHandler class.
 */
static DefaultHandler* DefaultHandler_Construct(
  REFCLSID  clsid,
  LPUNKNOWN pUnkOuter)
{
  DefaultHandler* newObject = 0;

  /*
   * Allocate space for the object.
   */
  newObject = (DefaultHandler*)HeapAlloc(GetProcessHeap(), 0, sizeof(DefaultHandler));

  if (newObject==0)
    return newObject;
  
  /*
   * Initialize the virtual function table.
   */
  newObject->lpvtbl1 = &DefaultHandler_IOleObject_VTable;
  newObject->lpvtbl2 = &DefaultHandler_NDIUnknown_VTable;
  newObject->lpvtbl3 = &DefaultHandler_IDataObject_VTable;
  newObject->lpvtbl4 = &DefaultHandler_IRunnableObject_VTable;

  /*
   * Start with one reference count. The caller of This function 
   * must release the interface pointer when it is done.
   */
  newObject->ref = 1;

  /*
   * Initialize the outer unknown
   * We don't keep a reference on the outer unknown since, the way 
   * aggregation works, our lifetime is at least as large as it's
   * lifetime.
   */
  if (pUnkOuter==NULL)
    pUnkOuter = (IUnknown*)&(newObject->lpvtbl2);

  newObject->outerUnknown = pUnkOuter;

  /*
   * Create a datacache object.
   * We aggregate with the datacache. Make sure we pass our outer
   * unknown as the datacache's outer unknown. 
   */
  CreateDataCache(newObject->outerUnknown,
		  clsid,
		  &IID_IUnknown,
		  (void**)&newObject->dataCache);

  /*
   * Initialize the other data members of the class.
   */
  memcpy(&(newObject->clsid), clsid, sizeof(CLSID));
  newObject->clientSite = NULL;
  newObject->oleAdviseHolder = NULL;
  newObject->dataAdviseHolder = NULL;
  newObject->containerApp = NULL;
  newObject->containerObj = NULL;

  return newObject;
}

static void DefaultHandler_Destroy(
  DefaultHandler* ptrToDestroy)
{
  /*
   * Free the strings idenfitying the object
   */
  if (ptrToDestroy->containerApp!=NULL)
  {
    SysFreeString(ptrToDestroy->containerApp);
    ptrToDestroy->containerApp = NULL;
  }

  if (ptrToDestroy->containerObj!=NULL)
  {
    SysFreeString(ptrToDestroy->containerObj);
    ptrToDestroy->containerObj = NULL;
  }
  
  /*
   * Release our reference to the data cache.
   */
  if (ptrToDestroy->dataCache!=NULL)
  {
    IUnknown_Release(ptrToDestroy->dataCache);
    ptrToDestroy->dataCache = NULL;
  }

  /*
   * Same thing for the client site.
   */
  if (ptrToDestroy->clientSite!=NULL)
  {
    IOleClientSite_Release(ptrToDestroy->clientSite);
    ptrToDestroy->clientSite = NULL;
  }

  /*
   * And the advise holder.
   */
  if (ptrToDestroy->oleAdviseHolder!=NULL)
  {
    IOleClientSite_Release(ptrToDestroy->oleAdviseHolder);
    ptrToDestroy->oleAdviseHolder = NULL;
  }

  /*
   * And the data advise holder.
   */
  if (ptrToDestroy->dataAdviseHolder!=NULL)
  {
    IOleClientSite_Release(ptrToDestroy->dataAdviseHolder);
    ptrToDestroy->dataAdviseHolder = NULL;
  }


  /*
   * Free the actual default handler structure.
   */
  HeapFree(GetProcessHeap(), 0, ptrToDestroy);
}

/*********************************************************
 * Method implementation for the  non delegating IUnknown
 * part of the DefaultHandler class.
 */

/************************************************************************
 * DefaultHandler_NDIUnknown_QueryInterface (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 *
 * This version of QueryInterface will not delegate it's implementation
 * to the outer unknown.
 */
static HRESULT WINAPI DefaultHandler_NDIUnknown_QueryInterface(
            IUnknown*      iface,
            REFIID         riid,
            void**         ppvObject)
{
  _ICOM_THIS_From_NDIUnknown(DefaultHandler, iface);

  /*
   * Perform a sanity check on the parameters.
   */
  if ( (This==0) || (ppvObject==0) )
    return E_INVALIDARG;
  
  /*
   * Initialize the return parameter.
   */
  *ppvObject = 0;

  /*
   * Compare the riid with the interface IDs implemented by This object.
   */
  if (memcmp(&IID_IUnknown, riid, sizeof(IID_IUnknown)) == 0) 
  {
    *ppvObject = iface;
  }
  else if (memcmp(&IID_IOleObject, riid, sizeof(IID_IOleObject)) == 0) 
  {
    *ppvObject = (IOleObject*)&(This->lpvtbl1);
  }
  else if (memcmp(&IID_IDataObject, riid, sizeof(IID_IDataObject)) == 0) 
  {
    *ppvObject = (IDataObject*)&(This->lpvtbl3);
  }
  else if (memcmp(&IID_IRunnableObject, riid, sizeof(IID_IRunnableObject)) == 0) 
  {
    *ppvObject = (IRunnableObject*)&(This->lpvtbl4);
  }
  else
  {
    /*
     * Blind aggregate the data cache to "inherit" it's interfaces.
     */
    IUnknown_QueryInterface(This->dataCache, riid, ppvObject);
  }
  
  /*
   * Check that we obtained an interface.
   */
  if ((*ppvObject)==0)
  {
    char clsid[50];

    WINE_StringFromCLSID((LPCLSID)riid,clsid);
    
    WARN(
	 "() : asking for un supported interface %s\n", 
	 clsid);

    return E_NOINTERFACE;
  }
  
  /*
   * Query Interface always increases the reference count by one when it is
   * successful. 
   */
  IUnknown_AddRef((IUnknown*)*ppvObject);

  return S_OK;;  
}

/************************************************************************
 * DefaultHandler_NDIUnknown_AddRef (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 *
 * This version of QueryInterface will not delegate it's implementation
 * to the outer unknown.
 */
static ULONG WINAPI DefaultHandler_NDIUnknown_AddRef( 
            IUnknown*      iface)
{
  _ICOM_THIS_From_NDIUnknown(DefaultHandler, iface);

  This->ref++;

  return This->ref;
}

/************************************************************************
 * DefaultHandler_NDIUnknown_Release (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 *
 * This version of QueryInterface will not delegate it's implementation
 * to the outer unknown.
 */
static ULONG WINAPI DefaultHandler_NDIUnknown_Release( 
            IUnknown*      iface)
{
  _ICOM_THIS_From_NDIUnknown(DefaultHandler, iface);

  /*
   * Decrease the reference count on This object.
   */
  This->ref--;

  /*
   * If the reference count goes down to 0, perform suicide.
   */
  if (This->ref==0)
  {
    DefaultHandler_Destroy(This);

    return 0;
  }
  
  return This->ref;
}

/*********************************************************
 * Methods implementation for the IOleObject part of
 * the DefaultHandler class.
 */

/************************************************************************
 * DefaultHandler_QueryInterface (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static HRESULT WINAPI DefaultHandler_QueryInterface(
            IOleObject*      iface,
            REFIID           riid,
            void**           ppvObject)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  return IUnknown_QueryInterface(This->outerUnknown, riid, ppvObject);  
}

/************************************************************************
 * DefaultHandler_AddRef (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static ULONG WINAPI DefaultHandler_AddRef( 
            IOleObject*        iface)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  return IUnknown_AddRef(This->outerUnknown);
}

/************************************************************************
 * DefaultHandler_Release (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static ULONG WINAPI DefaultHandler_Release( 
            IOleObject*        iface)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  return IUnknown_Release(This->outerUnknown);
}

/************************************************************************
 * DefaultHandler_SetClientSite (IOleObject)
 *
 * The default handler's implementation of This method only keeps the
 * client site pointer for future reference.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_SetClientSite(
	    IOleObject*        iface,
	    IOleClientSite*    pClientSite)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  TRACE("(%p, %p)\n", iface, pClientSite);

  /*
   * Make sure we release the previous client site if there
   * was one.
   */
  if (This->clientSite!=NULL)
  {
    IOleClientSite_Release(This->clientSite);
  }

  This->clientSite = pClientSite;

  if (This->clientSite!=NULL)
  {
    IOleClientSite_AddRef(This->clientSite);
  }

  return S_OK;
}

/************************************************************************
 * DefaultHandler_GetClientSite (IOleObject)
 *
 * The default handler's implementation of This method returns the
 * last pointer set in IOleObject_SetClientSite.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetClientSite(
	    IOleObject*        iface,
	    IOleClientSite**   ppClientSite)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  /*
   * Sanity check.
   */
  if (ppClientSite == NULL)
    return E_POINTER;

  *ppClientSite = This->clientSite;

  if (*ppClientSite!=NULL)
  {
    IOleClientSite_Release(*ppClientSite);
  }

  return S_OK;
}

/************************************************************************
 * DefaultHandler_SetHostNames (IOleObject)
 *
 * The default handler's implementation of This method just stores
 * the strings and returns S_OK.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_SetHostNames(
	    IOleObject*        iface,
	    LPCOLESTR          szContainerApp, 
	    LPCOLESTR          szContainerObj)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  TRACE("(%p, %s, %s)\n",
	iface,
	debugstr_w(szContainerApp), 
	debugstr_w(szContainerObj));

  /*
   * Be sure to cleanup before re-assinging the strings.
   */ 
  if (This->containerApp != NULL)
  {
    HeapFree( GetProcessHeap(), 0, This->containerApp );
    This->containerApp = NULL;
  }

  if (This->containerObj != NULL)
  {
    HeapFree( GetProcessHeap(), 0, This->containerObj );
    This->containerObj = NULL;
  }

  /*
   * Copy the string supplied.
   */
  if (szContainerApp != NULL)
  {
      if ((This->containerApp = (LPWSTR)HeapAlloc( GetProcessHeap(), 0,
				       (lstrlenW(szContainerApp) + 1) * sizeof(WCHAR) )) != NULL)
          lstrcpyW( This->containerApp, szContainerApp );
  }

  if (szContainerObj != NULL)
  {
      if ((This->containerObj = (LPWSTR)HeapAlloc( GetProcessHeap(), 0,
				       (lstrlenW(szContainerObj) + 1) * sizeof(WCHAR) )) != NULL)
          lstrcpyW( This->containerObj, szContainerObj );
  }

  return S_OK;
}

/************************************************************************
 * DefaultHandler_Close (IOleObject)
 *
 * The default handler's implementation of This method is meaningless
 * without a running server so it does nothing.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_Close(
	    IOleObject*        iface, 
	    DWORD              dwSaveOption)
{
  TRACE("()\n");
  return S_OK;
}

/************************************************************************
 * DefaultHandler_SetMoniker (IOleObject)
 *
 * The default handler's implementation of This method does nothing.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_SetMoniker(
	    IOleObject*        iface, 
	    DWORD              dwWhichMoniker,
	    IMoniker*          pmk)
{
  TRACE("(%p, %ld, %p)\n",
	iface, 
	dwWhichMoniker, 
	pmk);

  return S_OK;
}

/************************************************************************
 * DefaultHandler_GetMoniker (IOleObject)
 *
 * Delegate This request to the client site if we have one.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetMoniker(
	    IOleObject*        iface,
	    DWORD              dwAssign,
	    DWORD              dwWhichMoniker,
	    IMoniker**         ppmk)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  TRACE("(%p, %ld, %ld, %p)\n",
	iface, dwAssign, dwWhichMoniker, ppmk);

  if (This->clientSite)
  {
    return IOleClientSite_GetMoniker(This->clientSite,
				     dwAssign,
				     dwWhichMoniker,
				     ppmk);
			      
  }

  return E_UNSPEC;
}

/************************************************************************
 * DefaultHandler_InitFromData (IOleObject)
 *
 * This method is meaningless if the server is not running
 * 
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_InitFromData(
	    IOleObject*        iface, 
	    IDataObject*       pDataObject, 
	    BOOL               fCreation,
	    DWORD              dwReserved)
{
  TRACE("(%p, %p, %d, %ld)\n",
	iface, pDataObject, fCreation, dwReserved);

  return OLE_E_NOTRUNNING;
}

/************************************************************************
 * DefaultHandler_GetClipboardData (IOleObject)
 *
 * This method is meaningless if the server is not running
 * 
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetClipboardData(
	    IOleObject*        iface, 
	    DWORD              dwReserved, 
	    IDataObject**      ppDataObject)
{
  TRACE("(%p, %ld, %p)\n",
	iface, dwReserved, ppDataObject);

  return OLE_E_NOTRUNNING;
}

static HRESULT WINAPI DefaultHandler_DoVerb(
	    IOleObject*        iface, 
	    LONG               iVerb, 
	    LPMSG              lpmsg, 
	    IOleClientSite*    pActiveSite, 
	    LONG               lindex, 
	    HWND               hwndParent, 
	    LPCRECT            lprcPosRect)
{
  FIXME(": Stub\n");
  return E_NOTIMPL;
}

/************************************************************************
 * DefaultHandler_EnumVerbs (IOleObject)
 *
 * The default handler implementation of This method simply delegates
 * to OleRegEnumVerbs
 * 
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_EnumVerbs(
	    IOleObject*        iface, 
	    IEnumOLEVERB**     ppEnumOleVerb)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  TRACE("(%p, %p)\n", iface, ppEnumOleVerb);

  return OleRegEnumVerbs(&This->clsid, ppEnumOleVerb);
}

static HRESULT WINAPI DefaultHandler_Update(
	    IOleObject*        iface)
{
  FIXME(": Stub\n");
  return E_NOTIMPL;
}

/************************************************************************
 * DefaultHandler_IsUpToDate (IOleObject)
 *
 * This method is meaningless if the server is not running
 * 
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_IsUpToDate(
	    IOleObject*        iface)
{
  TRACE("(%p)\n", iface);

  return OLE_E_NOTRUNNING;
}

/************************************************************************
 * DefaultHandler_GetUserClassID (IOleObject)
 *
 * TODO: Map to a new class ID if emulation is active.
 * 
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetUserClassID(
	    IOleObject*        iface, 
	    CLSID*             pClsid)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  TRACE("(%p, %p)\n", iface, pClsid);

  /*
   * Sanity check.
   */
  if (pClsid==NULL)
    return E_POINTER;

  memcpy(pClsid, &This->clsid, sizeof(CLSID));

  return S_OK;
}

/************************************************************************
 * DefaultHandler_GetUserType (IOleObject)
 *
 * The default handler implementation of This method simply delegates
 * to OleRegGetUserType
 * 
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetUserType(
	    IOleObject*        iface, 
	    DWORD              dwFormOfType, 
	    LPOLESTR*          pszUserType)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  TRACE("(%p, %ld, %p)\n", iface, dwFormOfType, pszUserType);

  return OleRegGetUserType(&This->clsid, dwFormOfType, pszUserType);
}

/************************************************************************
 * DefaultHandler_SetExtent (IOleObject)
 *
 * This method is meaningless if the server is not running
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_SetExtent(
	    IOleObject*        iface, 
	    DWORD              dwDrawAspect, 
	    SIZEL*             psizel)
{
  TRACE("(%p, %lx, (%d,%d))\n", iface, dwDrawAspect, psizel->cx, psizel->cy);
  return OLE_E_NOTRUNNING;
}

/************************************************************************
 * DefaultHandler_GetExtent (IOleObject)
 *
 * The default handler's implementation of This method returns uses
 * the cache to locate the aspect and extract the extent from it.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetExtent(
	    IOleObject*        iface, 
	    DWORD              dwDrawAspect, 
	    SIZEL*             psizel)
{
  DVTARGETDEVICE* targetDevice;
  IViewObject2*   cacheView = NULL;
  HRESULT         hres;

  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);  

  TRACE("(%p, %lx, %p)\n", iface, dwDrawAspect, psizel);

  hres = IUnknown_QueryInterface(This->dataCache, &IID_IViewObject2, (void**)&cacheView);

  if (FAILED(hres))
    return E_UNEXPECTED;

  /*
   * Prepare the call to the cache's GetExtent method.
   *
   * Here we would build a valid DVTARGETDEVICE structure
   * but, since we are calling into the data cache, we 
   * know it's implementation and we'll skip This 
   * extra work until later.
   */
  targetDevice = NULL;

  hres = IViewObject2_GetExtent(cacheView,
				dwDrawAspect,
				-1,
				targetDevice,
				psizel);

  /*
   * Cleanup
   */
  IViewObject2_Release(cacheView);

  return hres;
}

/************************************************************************
 * DefaultHandler_Advise (IOleObject)
 *
 * The default handler's implementation of This method simply
 * delegates to the OleAdviseHolder.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_Advise(
	    IOleObject*        iface, 
	    IAdviseSink*       pAdvSink, 
	    DWORD*             pdwConnection)
{
  HRESULT hres = S_OK;
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);  

  TRACE("(%p, %p, %p)\n", iface, pAdvSink, pdwConnection);

  /*
   * Make sure we have an advise holder before we start.
   */
  if (This->oleAdviseHolder==NULL)
  {
    hres = CreateOleAdviseHolder(&This->oleAdviseHolder);
  }

  if (SUCCEEDED(hres))
  {
    hres = IOleAdviseHolder_Advise(This->oleAdviseHolder, 
				   pAdvSink, 
				   pdwConnection);
  }

  return hres;
}

/************************************************************************
 * DefaultHandler_Unadvise (IOleObject)
 *
 * The default handler's implementation of This method simply
 * delegates to the OleAdviseHolder.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_Unadvise(
	    IOleObject*        iface, 
	    DWORD              dwConnection)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);  

  TRACE("(%p, %ld)\n", iface, dwConnection);

  /*
   * If we don't have an advise holder yet, it means we don't have
   * a connection.
   */
  if (This->oleAdviseHolder==NULL)
    return OLE_E_NOCONNECTION;

  return IOleAdviseHolder_Unadvise(This->oleAdviseHolder,
				   dwConnection);
}

/************************************************************************
 * DefaultHandler_EnumAdvise (IOleObject)
 *
 * The default handler's implementation of This method simply
 * delegates to the OleAdviseHolder.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_EnumAdvise(
	    IOleObject*        iface, 
	    IEnumSTATDATA**    ppenumAdvise)
{
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);  

  TRACE("(%p, %p)\n", iface, ppenumAdvise);

  /*
   * Sanity check
   */
  if (ppenumAdvise==NULL)
    return E_POINTER;

  /*
   * Initialize the out parameter.
   */
  *ppenumAdvise = NULL;

  if (This->oleAdviseHolder==NULL)
    return IOleAdviseHolder_EnumAdvise(This->oleAdviseHolder,
				       ppenumAdvise);

  return S_OK;
}

/************************************************************************
 * DefaultHandler_GetMiscStatus (IOleObject)
 *
 * The default handler's implementation of This method simply delegates
 * to OleRegGetMiscStatus.
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetMiscStatus(
	    IOleObject*        iface, 
	    DWORD              dwAspect, 
	    DWORD*             pdwStatus)
{
  HRESULT hres;
  _ICOM_THIS_From_IOleObject(DefaultHandler, iface);

  TRACE("(%p, %lx, %p)\n", iface, dwAspect, pdwStatus);

  hres = OleRegGetMiscStatus(&(This->clsid), dwAspect, pdwStatus);

  if (FAILED(hres))
    *pdwStatus = 0;

  return S_OK;
}

/************************************************************************
 * DefaultHandler_SetExtent (IOleObject)
 *
 * This method is meaningless if the server is not running
 *
 * See Windows documentation for more details on IOleObject methods.
 */
static HRESULT WINAPI DefaultHandler_SetColorScheme(
	    IOleObject*           iface,
	    LOGPALETTE* pLogpal)
{
  TRACE("(%p, %p))\n", iface, pLogpal);
  return OLE_E_NOTRUNNING;
}

/*********************************************************
 * Methods implementation for the IDataObject part of
 * the DefaultHandler class.
 */

/************************************************************************
 * DefaultHandler_IDataObject_QueryInterface (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static HRESULT WINAPI DefaultHandler_IDataObject_QueryInterface(
            IDataObject*     iface, 
           REFIID           riid,
            void**           ppvObject)
{
  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);

  return IUnknown_QueryInterface(This->outerUnknown, riid, ppvObject);  
}

/************************************************************************
 * DefaultHandler_IDataObject_AddRef (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static ULONG WINAPI DefaultHandler_IDataObject_AddRef( 
            IDataObject*     iface)
{
  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);

  return IUnknown_AddRef(This->outerUnknown);  
}

/************************************************************************
 * DefaultHandler_IDataObject_Release (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static ULONG WINAPI DefaultHandler_IDataObject_Release( 
            IDataObject*     iface)
{
  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);

  return IUnknown_Release(This->outerUnknown);  
}

/************************************************************************
 * DefaultHandler_GetData
 *
 * Get Data from a source dataobject using format pformatetcIn->cfFormat
 * See Windows documentation for more details on GetData.
 * Default handler's implementation of this method delegates to the cache.
 */
static HRESULT WINAPI DefaultHandler_GetData(
            IDataObject*     iface,
            LPFORMATETC      pformatetcIn,
            STGMEDIUM*       pmedium)
{
  IDataObject* cacheDataObject = NULL;
  HRESULT      hres;

  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);

  TRACE("(%p, %p, %p)\n", iface, pformatetcIn, pmedium);

  hres = IUnknown_QueryInterface(This->dataCache,
                                 &IID_IDataObject,
                                 (void**)&cacheDataObject);

  if (FAILED(hres))
    return E_UNEXPECTED;

  hres = IDataObject_GetData(cacheDataObject,
                             pformatetcIn,
                             pmedium);

  IDataObject_Release(cacheDataObject);

  return hres;
}

static HRESULT WINAPI DefaultHandler_GetDataHere(
	    IDataObject*     iface, 
	    LPFORMATETC      pformatetc,
	    STGMEDIUM*       pmedium)
{
  FIXME(": Stub\n");
  return E_NOTIMPL;
}

/************************************************************************
 * DefaultHandler_QueryGetData (IDataObject)
 *
 * The default handler's implementation of This method delegates to 
 * the cache.
 *
 * See Windows documentation for more details on IDataObject methods.
 */
static HRESULT WINAPI DefaultHandler_QueryGetData(
	    IDataObject*     iface,
	    LPFORMATETC      pformatetc)
{
  IDataObject* cacheDataObject = NULL;
  HRESULT      hres;

  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);

  TRACE("(%p, %p)\n", iface, pformatetc);

  hres = IUnknown_QueryInterface(This->dataCache, 
				 &IID_IDataObject,
				 (void**)&cacheDataObject);

  if (FAILED(hres))
    return E_UNEXPECTED;

  hres = IDataObject_QueryGetData(cacheDataObject,
				  pformatetc);

  IDataObject_Release(cacheDataObject);
  
  return hres;
}

/************************************************************************
 * DefaultHandler_GetCanonicalFormatEtc (IDataObject)
 *
 * This method is meaningless if the server is not running
 *
 * See Windows documentation for more details on IDataObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetCanonicalFormatEtc(
	    IDataObject*     iface, 
	    LPFORMATETC      pformatectIn, 
	    LPFORMATETC      pformatetcOut)
{
  FIXME("(%p, %p, %p)\n", iface, pformatectIn, pformatetcOut);

  return OLE_E_NOTRUNNING;
}

/************************************************************************
 * DefaultHandler_SetData (IDataObject)
 *
 * The default handler's implementation of This method delegates to 
 * the cache.
 *
 * See Windows documentation for more details on IDataObject methods.
 */
static HRESULT WINAPI DefaultHandler_SetData(
	    IDataObject*     iface,
	    LPFORMATETC      pformatetc, 
	    STGMEDIUM*       pmedium, 
	    BOOL             fRelease)
{
  IDataObject* cacheDataObject = NULL;
  HRESULT      hres;

  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);

  TRACE("(%p, %p, %p, %d)\n", iface, pformatetc, pmedium, fRelease);

  hres = IUnknown_QueryInterface(This->dataCache, 
				 &IID_IDataObject,
				 (void**)&cacheDataObject);

  if (FAILED(hres))
    return E_UNEXPECTED;

  hres = IDataObject_SetData(cacheDataObject,
			     pformatetc,
			     pmedium,
			     fRelease);
  
  IDataObject_Release(cacheDataObject);
  
  return hres;
}

/************************************************************************
 * DefaultHandler_EnumFormatEtc (IDataObject)
 *
 * The default handler's implementation of This method simply delegates
 * to OleRegEnumFormatEtc.
 *
 * See Windows documentation for more details on IDataObject methods.
 */
static HRESULT WINAPI DefaultHandler_EnumFormatEtc(
	    IDataObject*     iface,       
	    DWORD            dwDirection,
	    IEnumFORMATETC** ppenumFormatEtc)
{
  HRESULT hres;
  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);

  TRACE("(%p, %lx, %p)\n", iface, dwDirection, ppenumFormatEtc);

  hres = OleRegEnumFormatEtc(&(This->clsid), dwDirection, ppenumFormatEtc);

  return hres;
}

/************************************************************************
 * DefaultHandler_DAdvise (IDataObject)
 *
 * The default handler's implementation of This method simply
 * delegates to the DataAdviseHolder.
 *
 * See Windows documentation for more details on IDataObject methods.
 */
static HRESULT WINAPI DefaultHandler_DAdvise(
	    IDataObject*     iface, 
	    FORMATETC*       pformatetc, 
	    DWORD            advf, 
	    IAdviseSink*     pAdvSink, 
	    DWORD*           pdwConnection)
{
  HRESULT hres = S_OK;
  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);  

  TRACE("(%p, %p, %ld, %p, %p)\n", 
	iface, pformatetc, advf, pAdvSink, pdwConnection);

  /*
   * Make sure we have a data advise holder before we start.
   */
  if (This->dataAdviseHolder==NULL)
  {
    hres = CreateDataAdviseHolder(&This->dataAdviseHolder);
  }

  if (SUCCEEDED(hres))
  {
    hres = IDataAdviseHolder_Advise(This->dataAdviseHolder, 
				    iface,
				    pformatetc, 
				    advf, 
				    pAdvSink, 
				    pdwConnection);
  }

  return hres;
}

/************************************************************************
 * DefaultHandler_DUnadvise (IDataObject)
 *
 * The default handler's implementation of This method simply
 * delegates to the DataAdviseHolder.
 *
 * See Windows documentation for more details on IDataObject methods.
 */
static HRESULT WINAPI DefaultHandler_DUnadvise(
	    IDataObject*     iface,
	    DWORD            dwConnection)
{
  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);  

  TRACE("(%p, %ld)\n", iface, dwConnection);

  /*
   * If we don't have a data advise holder yet, it means that
   * we don't have any connections..
   */
  if (This->dataAdviseHolder==NULL)
  {
    return OLE_E_NOCONNECTION;
  }

  return IDataAdviseHolder_Unadvise(This->dataAdviseHolder, 
				    dwConnection);
}

/************************************************************************
 * DefaultHandler_EnumDAdvise (IDataObject)
 *
 * The default handler's implementation of This method simply
 * delegates to the DataAdviseHolder.
 *
 * See Windows documentation for more details on IDataObject methods.
 */
static HRESULT WINAPI DefaultHandler_EnumDAdvise(
	    IDataObject*     iface,
	    IEnumSTATDATA**  ppenumAdvise)
{
  _ICOM_THIS_From_IDataObject(DefaultHandler, iface);  

  TRACE("(%p, %p)\n", iface, ppenumAdvise);

  /*
   * Sanity check
   */
  if (ppenumAdvise == NULL)
    return E_POINTER;

  /*
   * Initialize the out parameter.
   */
  *ppenumAdvise = NULL;

  /*
   * If we have a data advise holder object, delegate.
   */
  if (This->dataAdviseHolder!=NULL)
  {
    return IDataAdviseHolder_EnumAdvise(This->dataAdviseHolder, 
					ppenumAdvise);
  }

  return S_OK;
}

/*********************************************************
 * Methods implementation for the IRunnableObject part 
 * of the DefaultHandler class.
 */

/************************************************************************
 * DefaultHandler_IRunnableObject_QueryInterface (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static HRESULT WINAPI DefaultHandler_IRunnableObject_QueryInterface(
            IRunnableObject*     iface,
            REFIID               riid,
            void**               ppvObject)
{
  _ICOM_THIS_From_IRunnableObject(DefaultHandler, iface);

  return IUnknown_QueryInterface(This->outerUnknown, riid, ppvObject);  
}

/************************************************************************
 * DefaultHandler_IRunnableObject_QueryInterface (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static ULONG WINAPI DefaultHandler_IRunnableObject_AddRef( 
            IRunnableObject*     iface)
{
  _ICOM_THIS_From_IRunnableObject(DefaultHandler, iface);

  return IUnknown_AddRef(This->outerUnknown);
}

/************************************************************************
 * DefaultHandler_IRunnableObject_QueryInterface (IUnknown)
 *
 * See Windows documentation for more details on IUnknown methods.
 */
static ULONG WINAPI DefaultHandler_IRunnableObject_Release( 
            IRunnableObject*     iface)
{
  _ICOM_THIS_From_IRunnableObject(DefaultHandler, iface);

  return IUnknown_Release(This->outerUnknown);
}

/************************************************************************
 * DefaultHandler_GetRunningClass (IRunnableObject)
 *
 * According to Brockscmidt, Chapter 19, the default handler's 
 * implementation of IRunnableobject does nothing until the object
 * is actually running.
 *
 * See Windows documentation for more details on IRunnableObject methods.
 */
static HRESULT WINAPI DefaultHandler_GetRunningClass( 
            IRunnableObject*     iface,   
	    LPCLSID              lpClsid)
{
  TRACE("()\n");
  return S_OK;
}

static HRESULT WINAPI DefaultHandler_Run( 
            IRunnableObject*     iface,
	    IBindCtx*            pbc)
{
  FIXME(": Stub\n");
  return E_NOTIMPL;
}

/************************************************************************
 * DefaultHandler_IsRunning (IRunnableObject)
 *
 * According to Brockscmidt, Chapter 19, the default handler's 
 * implementation of IRunnableobject does nothing until the object
 * is actually running.
 *
 * See Windows documentation for more details on IRunnableObject methods.
 */
static BOOL    WINAPI DefaultHandler_IsRunning( 
            IRunnableObject*     iface)
{
  TRACE("()\n");
  return S_FALSE;
}

/************************************************************************
 * DefaultHandler_LockRunning (IRunnableObject)
 *
 * According to Brockscmidt, Chapter 19, the default handler's 
 * implementation of IRunnableobject does nothing until the object
 * is actually running.
 *
 * See Windows documentation for more details on IRunnableObject methods.
 */
static HRESULT WINAPI DefaultHandler_LockRunning( 
            IRunnableObject*     iface, 
	    BOOL                 fLock, 
	    BOOL                 fLastUnlockCloses)
{
  TRACE("()\n");
  return S_OK;
}

/************************************************************************
 * DefaultHandler_SetContainedObject (IRunnableObject)
 *
 * According to Brockscmidt, Chapter 19, the default handler's 
 * implementation of IRunnableobject does nothing until the object
 * is actually running.
 *
 * See Windows documentation for more details on IRunnableObject methods.
 */
static HRESULT WINAPI DefaultHandler_SetContainedObject( 
            IRunnableObject*     iface, 
	    BOOL                 fContained)
{
  TRACE("()\n");
  return S_OK;
}

