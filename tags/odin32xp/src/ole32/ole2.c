
/*
 *	OLE2 library
 *
 *	Copyright 1995	Martin von Loewis
 *      Copyright 1999  Francis Beaudet
 *      Copyright 1999  Noel Borthwick
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "commctrl.h"
#include "ole2.h"
#include "ole2ver.h"
#include "windef.h"
#include "winbase.h"
#include "winerror.h"
#include "winuser.h"
#include "winreg.h"

#include "wine/obj_clientserver.h"
#ifdef __WIN32OS2__ /* we've got older headers! */
#include "wine/obj_propertystorage.h"
#include "wine/obj_oleaut.h"
#endif
#include "wine/winbase16.h"
#include "wine/wingdi16.h"
#include "wine/winuser16.h"
#include "ole32_main.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ole);
WINE_DECLARE_DEBUG_CHANNEL(accel);

/******************************************************************************
 * These are static/global variables and internal data structures that the
 * OLE module uses to maintain it's state.
 */
typedef struct tagDropTargetNode
{
  HWND          hwndTarget;
  IDropTarget*    dropTarget;
#ifdef __WIN32OS2__
  BOOL          fDragEnter;
  DWORD         dwEffect;
  IDataObject * pDataObject;
#endif
  struct tagDropTargetNode* prevDropTarget;
  struct tagDropTargetNode* nextDropTarget;
} DropTargetNode;

typedef struct tagTrackerWindowInfo
{
  IDataObject* dataObject;
  IDropSource* dropSource;
  DWORD        dwOKEffect;
  DWORD*       pdwEffect;
  BOOL       trackingDone;
  HRESULT      returnValue;

  BOOL       escPressed;
  HWND       curTargetHWND;	/* window the mouse is hovering over */
  HWND       curDragTargetHWND; /* might be a ancestor of curTargetHWND */
  IDropTarget* curDragTarget;
} TrackerWindowInfo;

typedef struct tagOleMenuDescriptor  /* OleMenuDescriptor */
{
  HWND               hwndFrame;         /* The containers frame window */
  HWND               hwndActiveObject;  /* The active objects window */
  OLEMENUGROUPWIDTHS mgw;               /* OLE menu group widths for the shared menu */
  HMENU              hmenuCombined;     /* The combined menu */
  BOOL               bIsServerItem;     /* True if the currently open popup belongs to the server */
} OleMenuDescriptor;

typedef struct tagOleMenuHookItem   /* OleMenu hook item in per thread hook list */
{
  DWORD tid;                /* Thread Id  */
  HANDLE hHeap;             /* Heap this is allocated from */
  HHOOK GetMsg_hHook;       /* message hook for WH_GETMESSAGE */
  HHOOK CallWndProc_hHook;  /* message hook for WH_CALLWNDPROC */
  struct tagOleMenuHookItem *next;
} OleMenuHookItem;

static OleMenuHookItem *hook_list;

/*
 * This is the lock count on the OLE library. It is controlled by the
 * OLEInitialize/OLEUninitialize methods.
 */
static ULONG OLE_moduleLockCount = 0;

/*
 * Name of our registered window class.
 */
static const char OLEDD_DRAGTRACKERCLASS[] = "WineDragDropTracker32";

/*
 * This is the head of the Drop target container.
 */
static DropTargetNode* targetListHead = NULL;

/******************************************************************************
 * These are the prototypes of miscelaneous utility methods
 */
static void OLEUTL_ReadRegistryDWORDValue(HKEY regKey, DWORD* pdwValue);

/******************************************************************************
 * These are the prototypes of the utility methods used to manage a shared menu
 */
#ifdef __WIN32OS2__
/* Changed () -> (void) as () isn't a prototype good enought for VAC. */
#endif
static void OLEMenu_Initialize(void);
static void OLEMenu_UnInitialize(void);
BOOL OLEMenu_InstallHooks( DWORD tid );
BOOL OLEMenu_UnInstallHooks( DWORD tid );
OleMenuHookItem * OLEMenu_IsHookInstalled( DWORD tid );
static BOOL OLEMenu_FindMainMenuIndex( HMENU hMainMenu, HMENU hPopupMenu, UINT *pnPos );
BOOL OLEMenu_SetIsServerMenu( HMENU hmenu, OleMenuDescriptor *pOleMenuDescriptor );
LRESULT CALLBACK OLEMenu_CallWndProc(INT code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OLEMenu_GetMsgProc(INT code, WPARAM wParam, LPARAM lParam);

/******************************************************************************
 * These are the prototypes of the OLE Clipboard initialization methods (in clipboard.c)
 */
void OLEClipbrd_UnInitialize(void);
void OLEClipbrd_Initialize(void);

/******************************************************************************
 * These are the prototypes of the utility methods used for OLE Drag n Drop
 */
static void            OLEDD_Initialize(void);
static void            OLEDD_UnInitialize(void);
static void            OLEDD_InsertDropTarget(
			 DropTargetNode* nodeToAdd);
static DropTargetNode* OLEDD_ExtractDropTarget(
                         HWND hwndOfTarget);
static DropTargetNode* OLEDD_FindDropTarget(
                         HWND hwndOfTarget);
static LRESULT WINAPI  OLEDD_DragTrackerWindowProc(
			 HWND   hwnd,
			 UINT   uMsg,
			 WPARAM wParam,
			 LPARAM   lParam);
static void OLEDD_TrackMouseMove(
                         TrackerWindowInfo* trackerInfo,
			 POINT            mousePos,
			 DWORD              keyState);
static void OLEDD_TrackStateChange(
                         TrackerWindowInfo* trackerInfo,
			 POINT            mousePos,
			 DWORD              keyState);
static DWORD OLEDD_GetButtonState(void);

#ifdef __WIN32OS2__
static IDropTarget *IDropTarget_Constructor(void);

HWND hwndTracker = 0;
#endif

/******************************************************************************
 *		OleBuildVersion	[OLE2.1]
 *		OleBuildVersion [OLE32.@]
 */
DWORD WINAPI OleBuildVersion(void)
{
    TRACE("Returning version %d, build %d.\n", rmm, rup);
    return (rmm<<16)+rup;
}

/***********************************************************************
 *           OleInitialize       (OLE2.2)
 *           OleInitialize       (OLE32.@)
 */
HRESULT WINAPI OleInitialize(LPVOID reserved)
{
  HRESULT hr;

  TRACE("(%p)\n", reserved);

  /*
   * The first duty of the OleInitialize is to initialize the COM libraries.
   */
  hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

  /*
   * If the CoInitializeEx call failed, the OLE libraries can't be
   * initialized.
   */
  if (FAILED(hr))
    return hr;

  /*
   * Then, it has to initialize the OLE specific modules.
   * This includes:
   *     Clipboard
   *     Drag and Drop
   *     Object linking and Embedding
   *     In-place activation
   */
  if (OLE_moduleLockCount==0)
  {
    /*
     * Initialize the libraries.
     */
    TRACE("() - Initializing the OLE libraries\n");

    /*
     * OLE Clipboard
     */
    OLEClipbrd_Initialize();

    /*
     * Drag and Drop
     */
    OLEDD_Initialize();

    /*
     * OLE shared menu
     */
    OLEMenu_Initialize();
  }

  /*
   * Then, we increase the lock count on the OLE module.
   */
  OLE_moduleLockCount++;

  return hr;
}

/******************************************************************************
 *		CoGetCurrentProcess	[COMPOBJ.34]
 *		CoGetCurrentProcess	[OLE32.@]
 *
 * NOTES
 *   Is DWORD really the correct return type for this function?
 */
DWORD WINAPI CoGetCurrentProcess(void)
{
	return GetCurrentProcessId();
}

/******************************************************************************
 *		OleUninitialize	[OLE2.3]
 *		OleUninitialize	[OLE32.@]
 */
void WINAPI OleUninitialize(void)
{
  TRACE("()\n");

  /*
   * Decrease the lock count on the OLE module.
   */
  OLE_moduleLockCount--;

  /*
   * If we hit the bottom of the lock stack, free the libraries.
   */
  if (OLE_moduleLockCount==0)
  {
    /*
     * Actually free the libraries.
     */
    TRACE("() - Freeing the last reference count\n");

    /*
     * OLE Clipboard
     */
    OLEClipbrd_UnInitialize();

    /*
     * Drag and Drop
     */
    OLEDD_UnInitialize();

    /*
     * OLE shared menu
     */
    OLEMenu_UnInitialize();
  }

  /*
   * Then, uninitialize the COM libraries.
   */
  CoUninitialize();
}

/******************************************************************************
 *		CoRegisterMessageFilter	[OLE32.@]
 */
HRESULT WINAPI CoRegisterMessageFilter(
    LPMESSAGEFILTER lpMessageFilter,	/* [in] Pointer to interface */
    LPMESSAGEFILTER *lplpMessageFilter	/* [out] Indirect pointer to prior instance if non-NULL */
) {
    FIXME("stub\n");
    if (lplpMessageFilter) {
	*lplpMessageFilter = NULL;
    }
    return S_OK;
}

/******************************************************************************
 *		OleInitializeWOW	[OLE32.@]
 */
HRESULT WINAPI OleInitializeWOW(DWORD x) {
        FIXME("(0x%08lx),stub!\n",x);
        return 0;
}

/***********************************************************************
 *           RegisterDragDrop (OLE32.@)
 */
HRESULT WINAPI RegisterDragDrop(
	HWND hwnd,
	LPDROPTARGET pDropTarget)
{
  DropTargetNode* dropTargetInfo;

  TRACE("(%p,%p)\n", hwnd, pDropTarget);

  /*
   * First, check if the window is already registered.
   */
  dropTargetInfo = OLEDD_FindDropTarget(hwnd);

  if (dropTargetInfo!=NULL)
    return DRAGDROP_E_ALREADYREGISTERED;

  /*
   * If it's not there, we can add it. We first create a node for it.
   */
  dropTargetInfo = HeapAlloc(GetProcessHeap(), 0, sizeof(DropTargetNode));

  if (dropTargetInfo==NULL)
    return E_OUTOFMEMORY;

  dropTargetInfo->hwndTarget     = hwnd;
  dropTargetInfo->prevDropTarget = NULL;
  dropTargetInfo->nextDropTarget = NULL;

#ifdef __WIN32OS2__
  dropTargetInfo->pDataObject    = NULL;
  dropTargetInfo->fDragEnter     = FALSE;
  dropTargetInfo->dwEffect       = 0;
#endif
  /*
   * Don't forget that this is an interface pointer, need to nail it down since
   * we keep a copy of it.
   */
  dropTargetInfo->dropTarget  = pDropTarget;
  IDropTarget_AddRef(dropTargetInfo->dropTarget);

  OLEDD_InsertDropTarget(dropTargetInfo);

	return S_OK;
}

/***********************************************************************
 *           RevokeDragDrop (OLE32.@)
 */
HRESULT WINAPI RevokeDragDrop(
	HWND hwnd)
{
  DropTargetNode* dropTargetInfo;

  TRACE("(%p)\n", hwnd);

  /*
   * First, check if the window is already registered.
   */
  dropTargetInfo = OLEDD_ExtractDropTarget(hwnd);

  /*
   * If it ain't in there, it's an error.
   */
  if (dropTargetInfo==NULL)
    return DRAGDROP_E_NOTREGISTERED;

#ifdef __WIN32OS2__
  if(dropTargetInfo->pDataObject) {
      IDataObject_Release(dropTargetInfo->pDataObject);
  }
#endif

  /*
   * If it's in there, clean-up it's used memory and
   * references
   */
  IDropTarget_Release(dropTargetInfo->dropTarget);
  HeapFree(GetProcessHeap(), 0, dropTargetInfo);

	return S_OK;
}

/***********************************************************************
 *           OleRegGetUserType (OLE32.@)
 *
 * This implementation of OleRegGetUserType ignores the dwFormOfType
 * parameter and always returns the full name of the object. This is
 * not too bad since this is the case for many objects because of the
 * way they are registered.
 */
HRESULT WINAPI OleRegGetUserType(
	REFCLSID clsid,
	DWORD dwFormOfType,
	LPOLESTR* pszUserType)
{
  char    keyName[60];
  DWORD   dwKeyType;
  DWORD   cbData;
  HKEY    clsidKey;
  LONG    hres;
  LPBYTE  buffer;
  HRESULT retVal;
  /*
   * Initialize the out parameter.
   */
  *pszUserType = NULL;

  /*
   * Build the key name we're looking for
   */
  sprintf( keyName, "CLSID\\{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\\",
           clsid->Data1, clsid->Data2, clsid->Data3,
           clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3],
           clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7] );

  TRACE("(%s, %ld, %p)\n", keyName, dwFormOfType, pszUserType);

  /*
   * Open the class id Key
   */
  hres = RegOpenKeyA(HKEY_CLASSES_ROOT,
		     keyName,
		     &clsidKey);

  if (hres != ERROR_SUCCESS)
    return REGDB_E_CLASSNOTREG;

  /*
   * Retrieve the size of the name string.
   */
  cbData = 0;

  hres = RegQueryValueExA(clsidKey,
			  "",
			  NULL,
			  &dwKeyType,
			  NULL,
			  &cbData);

  if (hres!=ERROR_SUCCESS)
  {
    RegCloseKey(clsidKey);
    return REGDB_E_READREGDB;
  }

  /*
   * Allocate a buffer for the registry value.
   */
  *pszUserType = CoTaskMemAlloc(cbData*2);

  if (*pszUserType==NULL)
  {
    RegCloseKey(clsidKey);
    return E_OUTOFMEMORY;
  }

  buffer = HeapAlloc(GetProcessHeap(), 0, cbData);

  if (buffer == NULL)
  {
    RegCloseKey(clsidKey);
    CoTaskMemFree(*pszUserType);
    *pszUserType=NULL;
    return E_OUTOFMEMORY;
  }

  hres = RegQueryValueExA(clsidKey,
			  "",
			  NULL,
			  &dwKeyType,
			  buffer,
			  &cbData);

  RegCloseKey(clsidKey);


  if (hres!=ERROR_SUCCESS)
  {
    CoTaskMemFree(*pszUserType);
    *pszUserType=NULL;

    retVal = REGDB_E_READREGDB;
  }
  else
  {
    MultiByteToWideChar( CP_ACP, 0, buffer, -1, *pszUserType, cbData /*FIXME*/ );
    retVal = S_OK;
  }
  HeapFree(GetProcessHeap(), 0, buffer);

  return retVal;
}

/***********************************************************************
 * DoDragDrop [OLE32.@]
 */
HRESULT WINAPI DoDragDrop (
  IDataObject *pDataObject,  /* [in] ptr to the data obj           */
  IDropSource* pDropSource,  /* [in] ptr to the source obj         */
  DWORD       dwOKEffect,    /* [in] effects allowed by the source */
  DWORD       *pdwEffect)    /* [out] ptr to effects of the source */
{
  TrackerWindowInfo trackerInfo;
  HWND            hwndTrackWindow;
  MSG             msg;

  TRACE("(DataObject %p, DropSource %p)\n", pDataObject, pDropSource);

  /*
   * Setup the drag n drop tracking window.
   */
  if (!IsValidInterface((LPUNKNOWN)pDropSource))
      return E_INVALIDARG;

  trackerInfo.dataObject        = pDataObject;
  trackerInfo.dropSource        = pDropSource;
  trackerInfo.dwOKEffect        = dwOKEffect;
  trackerInfo.pdwEffect         = pdwEffect;
  trackerInfo.trackingDone      = FALSE;
  trackerInfo.escPressed        = FALSE;
  trackerInfo.curDragTargetHWND = 0;
  trackerInfo.curTargetHWND     = 0;
  trackerInfo.curDragTarget     = 0;

#ifdef __WIN32OS2__
  IDataObject_AddRef(trackerInfo.dataObject);
#endif

  hwndTrackWindow = CreateWindowA(OLEDD_DRAGTRACKERCLASS,
				    "TrackerWindow",
				    WS_POPUP,
				    CW_USEDEFAULT, CW_USEDEFAULT,
				    CW_USEDEFAULT, CW_USEDEFAULT,
				    0,
				    0,
				    0,
				    (LPVOID)&trackerInfo);

  if (hwndTrackWindow!=0)
  {
#ifdef __WIN32OS2__
    hwndTracker = hwndTrackWindow;
#endif

    /*
     * Capture the mouse input
     */
    SetCapture(hwndTrackWindow);

    /*
     * Pump messages. All mouse input should go the the capture window.
     */
    while (!trackerInfo.trackingDone && GetMessageA(&msg, 0, 0, 0) )
    {
      if ( (msg.message >= WM_KEYFIRST) &&
	   (msg.message <= WM_KEYLAST) )
      {
#ifdef __WIN32OS2__
        dprintf(("dragloop: key msg %x\n", msg.message));
#endif
	/*
	 * When keyboard messages are sent to windows on this thread, we
	 * want to ignore notify the drop source that the state changed.
	 * in the case of the Escape key, we also notify the drop source
	 * we give it a special meaning.
	 */
	if ( (msg.message==WM_KEYDOWN) &&
	     (msg.wParam==VK_ESCAPE) )
	{
	  trackerInfo.escPressed = TRUE;
	}

	/*
	 * Notify the drop source.
	 */
	OLEDD_TrackStateChange(&trackerInfo,
			       msg.pt,
			       OLEDD_GetButtonState());
      }
      else
      {
#ifdef __WIN32OS2__
        dprintf(("dragloop: dispmsg %x\n", msg.message));
#endif
	/*
	 * Dispatch the messages only when it's not a keyboard message.
	 */
	DispatchMessageA(&msg);
      }
    }

    /*
     * Destroy the temporary window.
     */
    DestroyWindow(hwndTrackWindow);

#ifdef __WIN32OS2__
    IDataObject_Release(trackerInfo.dataObject);
#endif

    return trackerInfo.returnValue;
  }

#ifdef __WIN32OS2__
  IDataObject_Release(trackerInfo.dataObject);
#endif

  return E_FAIL;
}

/***********************************************************************
 * OleQueryLinkFromData [OLE32.@]
 */
HRESULT WINAPI OleQueryLinkFromData(
  IDataObject* pSrcDataObject)
{
  FIXME("(%p),stub!\n", pSrcDataObject);
  return S_OK;
}

/***********************************************************************
 * OleRegGetMiscStatus [OLE32.@]
 */
HRESULT WINAPI OleRegGetMiscStatus(
  REFCLSID clsid,
  DWORD    dwAspect,
  DWORD*   pdwStatus)
{
  char    keyName[60];
  HKEY    clsidKey;
  HKEY    miscStatusKey;
  HKEY    aspectKey;
  LONG    result;

  /*
   * Initialize the out parameter.
   */
  *pdwStatus = 0;

  /*
   * Build the key name we're looking for
   */
  sprintf( keyName, "CLSID\\{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\\",
           clsid->Data1, clsid->Data2, clsid->Data3,
           clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3],
           clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7] );

  TRACE("(%s, %ld, %p)\n", keyName, dwAspect, pdwStatus);

  /*
   * Open the class id Key
   */
  result = RegOpenKeyA(HKEY_CLASSES_ROOT,
		       keyName,
		       &clsidKey);

  if (result != ERROR_SUCCESS)
    return REGDB_E_CLASSNOTREG;

  /*
   * Get the MiscStatus
   */
  result = RegOpenKeyA(clsidKey,
		       "MiscStatus",
		       &miscStatusKey);


  if (result != ERROR_SUCCESS)
  {
    RegCloseKey(clsidKey);
    return REGDB_E_READREGDB;
  }

  /*
   * Read the default value
   */
  OLEUTL_ReadRegistryDWORDValue(miscStatusKey, pdwStatus);

  /*
   * Open the key specific to the requested aspect.
   */
  sprintf(keyName, "%ld", dwAspect);

  result = RegOpenKeyA(miscStatusKey,
		       keyName,
		       &aspectKey);

  if (result == ERROR_SUCCESS)
  {
    OLEUTL_ReadRegistryDWORDValue(aspectKey, pdwStatus);
    RegCloseKey(aspectKey);
  }

  /*
   * Cleanup
   */
  RegCloseKey(miscStatusKey);
  RegCloseKey(clsidKey);

  return S_OK;
}

/******************************************************************************
 *              OleSetContainedObject        [OLE32.@]
 */
HRESULT WINAPI OleSetContainedObject(
  LPUNKNOWN pUnknown,
  BOOL      fContained)
{
  IRunnableObject* runnable = NULL;
  HRESULT          hres;

  TRACE("(%p,%x), stub!\n", pUnknown, fContained);

  hres = IUnknown_QueryInterface(pUnknown,
				 &IID_IRunnableObject,
				 (void**)&runnable);

  if (SUCCEEDED(hres))
  {
    hres = IRunnableObject_SetContainedObject(runnable, fContained);

    IRunnableObject_Release(runnable);

    return hres;
  }

  return S_OK;
}

/******************************************************************************
 *              OleLoad        [OLE32.@]
 */
HRESULT WINAPI OleLoad(
  LPSTORAGE       pStg,
  REFIID          riid,
  LPOLECLIENTSITE pClientSite,
  LPVOID*         ppvObj)
{
  IPersistStorage* persistStorage = NULL;
  IOleObject*      oleObject      = NULL;
  STATSTG          storageInfo;
  HRESULT          hres;

  TRACE("(%p,%p,%p,%p)\n", pStg, riid, pClientSite, ppvObj);

  /*
   * TODO, Conversion ... OleDoAutoConvert
   */

  /*
   * Get the class ID for the object.
   */
  hres = IStorage_Stat(pStg, &storageInfo, STATFLAG_NONAME);

  /*
   * Now, try and create the handler for the object
   */
  hres = CoCreateInstance(&storageInfo.clsid,
			  NULL,
			  CLSCTX_INPROC_HANDLER,
			  &IID_IOleObject,
			  (void**)&oleObject);

  /*
   * If that fails, as it will most times, load the default
   * OLE handler.
   */
  if (FAILED(hres))
  {
    hres = OleCreateDefaultHandler(&storageInfo.clsid,
				   NULL,
				   &IID_IOleObject,
				   (void**)&oleObject);
  }

  /*
   * If we couldn't find a handler... this is bad. Abort the whole thing.
   */
  if (FAILED(hres))
    return hres;

  /*
   * Inform the new object of it's client site.
   */
  hres = IOleObject_SetClientSite(oleObject, pClientSite);

  /*
   * Initialize the object with it's IPersistStorage interface.
   */
  hres = IOleObject_QueryInterface(oleObject,
				   &IID_IPersistStorage,
				   (void**)&persistStorage);

  if (SUCCEEDED(hres))
  {
    IPersistStorage_Load(persistStorage, pStg);

    IPersistStorage_Release(persistStorage);
    persistStorage = NULL;
  }

  /*
   * Return the requested interface to the caller.
   */
  hres = IOleObject_QueryInterface(oleObject, riid, ppvObj);

  /*
   * Cleanup interfaces used internally
   */
  IOleObject_Release(oleObject);

  return hres;
}

/***********************************************************************
 *           OleSave     [OLE32.@]
 */
HRESULT WINAPI OleSave(
  LPPERSISTSTORAGE pPS,
  LPSTORAGE        pStg,
  BOOL             fSameAsLoad)
{
  HRESULT hres;
  CLSID   objectClass;

  TRACE("(%p,%p,%x)\n", pPS, pStg, fSameAsLoad);

  /*
   * First, we transfer the class ID (if available)
   */
  hres = IPersistStorage_GetClassID(pPS, &objectClass);

  if (SUCCEEDED(hres))
  {
    WriteClassStg(pStg, &objectClass);
  }

  /*
   * Then, we ask the object to save itself to the
   * storage. If it is successful, we commit the storage.
   */
  hres = IPersistStorage_Save(pPS, pStg, fSameAsLoad);

  if (SUCCEEDED(hres))
  {
    IStorage_Commit(pStg,
		    STGC_DEFAULT);
  }

  return hres;
}


/******************************************************************************
 *              OleLockRunning        [OLE32.@]
 */
HRESULT WINAPI OleLockRunning(LPUNKNOWN pUnknown, BOOL fLock, BOOL fLastUnlockCloses)
{
  IRunnableObject* runnable = NULL;
  HRESULT          hres;

  TRACE("(%p,%x,%x)\n", pUnknown, fLock, fLastUnlockCloses);

  hres = IUnknown_QueryInterface(pUnknown,
				 &IID_IRunnableObject,
				 (void**)&runnable);

  if (SUCCEEDED(hres))
  {
    hres = IRunnableObject_LockRunning(runnable, fLock, fLastUnlockCloses);

    IRunnableObject_Release(runnable);

    return hres;
  }
  else
    return E_INVALIDARG;
}


/**************************************************************************
 * Internal methods to manage the shared OLE menu in response to the
 * OLE***MenuDescriptor API
 */

/***
 * OLEMenu_Initialize()
 *
 * Initializes the OLEMENU data structures.
 */
static void OLEMenu_Initialize()
{
}

/***
 * OLEMenu_UnInitialize()
 *
 * Releases the OLEMENU data structures.
 */
static void OLEMenu_UnInitialize()
{
}

/*************************************************************************
 * OLEMenu_InstallHooks
 * Install thread scope message hooks for WH_GETMESSAGE and WH_CALLWNDPROC
 *
 * RETURNS: TRUE if message hooks were successfully installed
 *          FALSE on failure
 */
BOOL OLEMenu_InstallHooks( DWORD tid )
{
  OleMenuHookItem *pHookItem = NULL;

  /* Create an entry for the hook table */
  if ( !(pHookItem = HeapAlloc(GetProcessHeap(), 0,
                               sizeof(OleMenuHookItem)) ) )
    return FALSE;

  pHookItem->tid = tid;
  pHookItem->hHeap = GetProcessHeap();

  /* Install a thread scope message hook for WH_GETMESSAGE */
  pHookItem->GetMsg_hHook = SetWindowsHookExA( WH_GETMESSAGE, OLEMenu_GetMsgProc,
                                               0, GetCurrentThreadId() );
  if ( !pHookItem->GetMsg_hHook )
    goto CLEANUP;

  /* Install a thread scope message hook for WH_CALLWNDPROC */
  pHookItem->CallWndProc_hHook = SetWindowsHookExA( WH_CALLWNDPROC, OLEMenu_CallWndProc,
                                                    0, GetCurrentThreadId() );
  if ( !pHookItem->CallWndProc_hHook )
    goto CLEANUP;

  /* Insert the hook table entry */
  pHookItem->next = hook_list;
  hook_list = pHookItem;

  return TRUE;

CLEANUP:
  /* Unhook any hooks */
  if ( pHookItem->GetMsg_hHook )
    UnhookWindowsHookEx( pHookItem->GetMsg_hHook );
  if ( pHookItem->CallWndProc_hHook )
    UnhookWindowsHookEx( pHookItem->CallWndProc_hHook );
  /* Release the hook table entry */
  HeapFree(pHookItem->hHeap, 0, pHookItem );

  return FALSE;
}

/*************************************************************************
 * OLEMenu_UnInstallHooks
 * UnInstall thread scope message hooks for WH_GETMESSAGE and WH_CALLWNDPROC
 *
 * RETURNS: TRUE if message hooks were successfully installed
 *          FALSE on failure
 */
BOOL OLEMenu_UnInstallHooks( DWORD tid )
{
  OleMenuHookItem *pHookItem = NULL;
  OleMenuHookItem **ppHook = &hook_list;

  while (*ppHook)
  {
      if ((*ppHook)->tid == tid)
      {
          pHookItem = *ppHook;
          *ppHook = pHookItem->next;
          break;
      }
      ppHook = &(*ppHook)->next;
  }
  if (!pHookItem) return FALSE;

  /* Uninstall the hooks installed for this thread */
  if ( !UnhookWindowsHookEx( pHookItem->GetMsg_hHook ) )
    goto CLEANUP;
  if ( !UnhookWindowsHookEx( pHookItem->CallWndProc_hHook ) )
    goto CLEANUP;

  /* Release the hook table entry */
  HeapFree(pHookItem->hHeap, 0, pHookItem );

  return TRUE;

CLEANUP:
  /* Release the hook table entry */
  HeapFree(pHookItem->hHeap, 0, pHookItem );

  return FALSE;
}

/*************************************************************************
 * OLEMenu_IsHookInstalled
 * Tests if OLEMenu hooks have been installed for a thread
 *
 * RETURNS: The pointer and index of the hook table entry for the tid
 *          NULL and -1 for the index if no hooks were installed for this thread
 */
OleMenuHookItem * OLEMenu_IsHookInstalled( DWORD tid )
{
  OleMenuHookItem *pHookItem = NULL;

  /* Do a simple linear search for an entry whose tid matches ours.
   * We really need a map but efficiency is not a concern here. */
  for (pHookItem = hook_list; pHookItem; pHookItem = pHookItem->next)
  {
    if ( tid == pHookItem->tid )
      return pHookItem;
  }

  return NULL;
}

/***********************************************************************
 *           OLEMenu_FindMainMenuIndex
 *
 * Used by OLEMenu API to find the top level group a menu item belongs to.
 * On success pnPos contains the index of the item in the top level menu group
 *
 * RETURNS: TRUE if the ID was found, FALSE on failure
 */
static BOOL OLEMenu_FindMainMenuIndex( HMENU hMainMenu, HMENU hPopupMenu, UINT *pnPos )
{
  UINT i, nItems;

  nItems = GetMenuItemCount( hMainMenu );

  for (i = 0; i < nItems; i++)
  {
    HMENU hsubmenu;

    /*  Is the current item a submenu? */
    if ( (hsubmenu = GetSubMenu(hMainMenu, i)) )
    {
      /* If the handle is the same we're done */
      if ( hsubmenu == hPopupMenu )
      {
        if (pnPos)
          *pnPos = i;
        return TRUE;
      }
      /* Recursively search without updating pnPos */
      else if ( OLEMenu_FindMainMenuIndex( hsubmenu, hPopupMenu, NULL ) )
      {
        if (pnPos)
          *pnPos = i;
        return TRUE;
      }
    }
  }

  return FALSE;
}

/***********************************************************************
 *           OLEMenu_SetIsServerMenu
 *
 * Checks whether a popup menu belongs to a shared menu group which is
 * owned by the server, and sets the menu descriptor state accordingly.
 * All menu messages from these groups should be routed to the server.
 *
 * RETURNS: TRUE if the popup menu is part of a server owned group
 *          FASE if the popup menu is part of a container owned group
 */
BOOL OLEMenu_SetIsServerMenu( HMENU hmenu, OleMenuDescriptor *pOleMenuDescriptor )
{
  UINT nPos = 0, nWidth, i;

  pOleMenuDescriptor->bIsServerItem = FALSE;

  /* Don't bother searching if the popup is the combined menu itself */
  if ( hmenu == pOleMenuDescriptor->hmenuCombined )
    return FALSE;

  /* Find the menu item index in the shared OLE menu that this item belongs to */
  if ( !OLEMenu_FindMainMenuIndex( pOleMenuDescriptor->hmenuCombined, hmenu,  &nPos ) )
    return FALSE;

  /* The group widths array has counts for the number of elements
   * in the groups File, Edit, Container, Object, Window, Help.
   * The Edit, Object & Help groups belong to the server object
   * and the other three belong to the container.
   * Loop through the group widths and locate the group we are a member of.
   */
  for ( i = 0, nWidth = 0; i < 6; i++ )
  {
    nWidth += pOleMenuDescriptor->mgw.width[i];
    if ( nPos < nWidth )
    {
      /* Odd elements are server menu widths */
      pOleMenuDescriptor->bIsServerItem = (i%2) ? TRUE : FALSE;
      break;
    }
  }

  return pOleMenuDescriptor->bIsServerItem;
}

/*************************************************************************
 * OLEMenu_CallWndProc
 * Thread scope WH_CALLWNDPROC hook proc filter function (callback)
 * This is invoked from a message hook installed in OleSetMenuDescriptor.
 */
LRESULT CALLBACK OLEMenu_CallWndProc(INT code, WPARAM wParam, LPARAM lParam)
{
  LPCWPSTRUCT pMsg = NULL;
  HOLEMENU hOleMenu = 0;
  OleMenuDescriptor *pOleMenuDescriptor = NULL;
  OleMenuHookItem *pHookItem = NULL;
  WORD fuFlags;

  TRACE("%i, %04x, %08x\n", code, wParam, (unsigned)lParam );

  /* Check if we're being asked to process the message */
  if ( HC_ACTION != code )
    goto NEXTHOOK;

  /* Retrieve the current message being dispatched from lParam */
  pMsg = (LPCWPSTRUCT)lParam;

  /* Check if the message is destined for a window we are interested in:
   * If the window has an OLEMenu property we may need to dispatch
   * the menu message to its active objects window instead. */

  hOleMenu = (HOLEMENU)GetPropA( pMsg->hwnd, "PROP_OLEMenuDescriptor" );
  if ( !hOleMenu )
    goto NEXTHOOK;

  /* Get the menu descriptor */
  pOleMenuDescriptor = (OleMenuDescriptor *) GlobalLock( hOleMenu );
  if ( !pOleMenuDescriptor ) /* Bad descriptor! */
    goto NEXTHOOK;

  /* Process menu messages */
  switch( pMsg->message )
  {
    case WM_INITMENU:
    {
      /* Reset the menu descriptor state */
      pOleMenuDescriptor->bIsServerItem = FALSE;

      /* Send this message to the server as well */
      SendMessageA( pOleMenuDescriptor->hwndActiveObject,
                  pMsg->message, pMsg->wParam, pMsg->lParam );
      goto NEXTHOOK;
    }

    case WM_INITMENUPOPUP:
    {
      /* Save the state for whether this is a server owned menu */
      OLEMenu_SetIsServerMenu( (HMENU)pMsg->wParam, pOleMenuDescriptor );
      break;
    }

    case WM_MENUSELECT:
    {
      fuFlags = HIWORD(pMsg->wParam);  /* Get flags */
      if ( fuFlags & MF_SYSMENU )
         goto NEXTHOOK;

      /* Save the state for whether this is a server owned popup menu */
      else if ( fuFlags & MF_POPUP )
        OLEMenu_SetIsServerMenu( (HMENU)pMsg->lParam, pOleMenuDescriptor );

      break;
    }

    case WM_DRAWITEM:
    {
      LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) pMsg->lParam;
      if ( pMsg->wParam != 0 || lpdis->CtlType != ODT_MENU )
        goto NEXTHOOK;  /* Not a menu message */

      break;
    }

    default:
      goto NEXTHOOK;
  }

  /* If the message was for the server dispatch it accordingly */
  if ( pOleMenuDescriptor->bIsServerItem )
  {
    SendMessageA( pOleMenuDescriptor->hwndActiveObject,
                  pMsg->message, pMsg->wParam, pMsg->lParam );
  }

NEXTHOOK:
  if ( pOleMenuDescriptor )
    GlobalUnlock( hOleMenu );

  /* Lookup the hook item for the current thread */
  if ( !( pHookItem = OLEMenu_IsHookInstalled( GetCurrentThreadId() ) ) )
  {
    /* This should never fail!! */
    WARN("could not retrieve hHook for current thread!\n" );
    return 0;
  }

  /* Pass on the message to the next hooker */
  return CallNextHookEx( pHookItem->CallWndProc_hHook, code, wParam, lParam );
}

/*************************************************************************
 * OLEMenu_GetMsgProc
 * Thread scope WH_GETMESSAGE hook proc filter function (callback)
 * This is invoked from a message hook installed in OleSetMenuDescriptor.
 */
LRESULT CALLBACK OLEMenu_GetMsgProc(INT code, WPARAM wParam, LPARAM lParam)
{
  LPMSG pMsg = NULL;
  HOLEMENU hOleMenu = 0;
  OleMenuDescriptor *pOleMenuDescriptor = NULL;
  OleMenuHookItem *pHookItem = NULL;
  WORD wCode;

  TRACE("%i, %04x, %08x\n", code, wParam, (unsigned)lParam );

  /* Check if we're being asked to process a  messages */
  if ( HC_ACTION != code )
    goto NEXTHOOK;

  /* Retrieve the current message being dispatched from lParam */
  pMsg = (LPMSG)lParam;

  /* Check if the message is destined for a window we are interested in:
   * If the window has an OLEMenu property we may need to dispatch
   * the menu message to its active objects window instead. */

  hOleMenu = (HOLEMENU)GetPropA( pMsg->hwnd, "PROP_OLEMenuDescriptor" );
  if ( !hOleMenu )
    goto NEXTHOOK;

  /* Process menu messages */
  switch( pMsg->message )
  {
    case WM_COMMAND:
    {
      wCode = HIWORD(pMsg->wParam);  /* Get notification code */
      if ( wCode )
        goto NEXTHOOK;  /* Not a menu message */
      break;
    }
    default:
      goto NEXTHOOK;
  }

  /* Get the menu descriptor */
  pOleMenuDescriptor = (OleMenuDescriptor *) GlobalLock( hOleMenu );
  if ( !pOleMenuDescriptor ) /* Bad descriptor! */
    goto NEXTHOOK;

  /* If the message was for the server dispatch it accordingly */
  if ( pOleMenuDescriptor->bIsServerItem )
  {
    /* Change the hWnd in the message to the active objects hWnd.
     * The message loop which reads this message will automatically
     * dispatch it to the embedded objects window. */
    pMsg->hwnd = pOleMenuDescriptor->hwndActiveObject;
  }

NEXTHOOK:
  if ( pOleMenuDescriptor )
    GlobalUnlock( hOleMenu );

  /* Lookup the hook item for the current thread */
  if ( !( pHookItem = OLEMenu_IsHookInstalled( GetCurrentThreadId() ) ) )
  {
    /* This should never fail!! */
    WARN("could not retrieve hHook for current thread!\n" );
    return FALSE;
  }

  /* Pass on the message to the next hooker */
  return CallNextHookEx( pHookItem->GetMsg_hHook, code, wParam, lParam );
}

/***********************************************************************
 * OleCreateMenuDescriptor [OLE32.@]
 * Creates an OLE menu descriptor for OLE to use when dispatching
 * menu messages and commands.
 *
 * PARAMS:
 *    hmenuCombined  -  Handle to the objects combined menu
 *    lpMenuWidths   -  Pointer to array of 6 LONG's indicating menus per group
 *
 */
HOLEMENU WINAPI OleCreateMenuDescriptor(
  HMENU                hmenuCombined,
  LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
  HOLEMENU hOleMenu;
  OleMenuDescriptor *pOleMenuDescriptor;
  int i;

  if ( !hmenuCombined || !lpMenuWidths )
    return 0;

  /* Create an OLE menu descriptor */
  if ( !(hOleMenu = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
                                sizeof(OleMenuDescriptor) ) ) )
  return 0;

  pOleMenuDescriptor = (OleMenuDescriptor *) GlobalLock( hOleMenu );
  if ( !pOleMenuDescriptor )
    return 0;

  /* Initialize menu group widths and hmenu */
  for ( i = 0; i < 6; i++ )
    pOleMenuDescriptor->mgw.width[i] = lpMenuWidths->width[i];

  pOleMenuDescriptor->hmenuCombined = hmenuCombined;
  pOleMenuDescriptor->bIsServerItem = FALSE;
  GlobalUnlock( hOleMenu );

  return hOleMenu;
}

/***********************************************************************
 * OleDestroyMenuDescriptor [OLE32.@]
 * Destroy the shared menu descriptor
 */
HRESULT WINAPI OleDestroyMenuDescriptor(
  HOLEMENU hmenuDescriptor)
{
  if ( hmenuDescriptor )
    GlobalFree( hmenuDescriptor );
	return S_OK;
}

/***********************************************************************
 * OleSetMenuDescriptor [OLE32.@]
 * Installs or removes OLE dispatching code for the containers frame window
 * FIXME: The lpFrame and lpActiveObject parameters are currently ignored
 * OLE should install context sensitive help F1 filtering for the app when
 * these are non null.
 *
 * PARAMS:
 *     hOleMenu         Handle to composite menu descriptor
 *     hwndFrame        Handle to containers frame window
 *     hwndActiveObject Handle to objects in-place activation window
 *     lpFrame          Pointer to IOleInPlaceFrame on containers window
 *     lpActiveObject   Pointer to IOleInPlaceActiveObject on active in-place object
 *
 * RETURNS:
 *      S_OK                               - menu installed correctly
 *      E_FAIL, E_INVALIDARG, E_UNEXPECTED - failure
 */
HRESULT WINAPI OleSetMenuDescriptor(
  HOLEMENU               hOleMenu,
  HWND                   hwndFrame,
  HWND                   hwndActiveObject,
  LPOLEINPLACEFRAME        lpFrame,
  LPOLEINPLACEACTIVEOBJECT lpActiveObject)
{
  OleMenuDescriptor *pOleMenuDescriptor = NULL;

  /* Check args */
  if ( !hwndFrame || (hOleMenu && !hwndActiveObject) )
    return E_INVALIDARG;

  if ( lpFrame || lpActiveObject )
  {
     FIXME("(%x, %p, %p, %p, %p), Context sensitive help filtering not implemented!\n",
	(unsigned int)hOleMenu,
	hwndFrame,
	hwndActiveObject,
	lpFrame,
	lpActiveObject);
  }

  /* Set up a message hook to intercept the containers frame window messages.
   * The message filter is responsible for dispatching menu messages from the
   * shared menu which are intended for the object.
   */

  if ( hOleMenu )  /* Want to install dispatching code */
  {
    /* If OLEMenu hooks are already installed for this thread, fail
     * Note: This effectively means that OleSetMenuDescriptor cannot
     * be called twice in succession on the same frame window
     * without first calling it with a null hOleMenu to uninstall */
    if ( OLEMenu_IsHookInstalled( GetCurrentThreadId() ) )
  return E_FAIL;

    /* Get the menu descriptor */
    pOleMenuDescriptor = (OleMenuDescriptor *) GlobalLock( hOleMenu );
    if ( !pOleMenuDescriptor )
      return E_UNEXPECTED;

    /* Update the menu descriptor */
    pOleMenuDescriptor->hwndFrame = hwndFrame;
    pOleMenuDescriptor->hwndActiveObject = hwndActiveObject;

    GlobalUnlock( hOleMenu );
    pOleMenuDescriptor = NULL;

    /* Add a menu descriptor windows property to the frame window */
    SetPropA( hwndFrame, "PROP_OLEMenuDescriptor", hOleMenu );

    /* Install thread scope message hooks for WH_GETMESSAGE and WH_CALLWNDPROC */
    if ( !OLEMenu_InstallHooks( GetCurrentThreadId() ) )
      return E_FAIL;
  }
  else  /* Want to uninstall dispatching code */
  {
    /* Uninstall the hooks */
    if ( !OLEMenu_UnInstallHooks( GetCurrentThreadId() ) )
      return E_FAIL;

    /* Remove the menu descriptor property from the frame window */
    RemovePropA( hwndFrame, "PROP_OLEMenuDescriptor" );
  }

  return S_OK;
}

/******************************************************************************
 *              IsAccelerator        [OLE32.@]
 * Mostly copied from controls/menu.c TranslateAccelerator implementation
 */
BOOL WINAPI IsAccelerator(HACCEL hAccel, int cAccelEntries, LPMSG lpMsg, WORD* lpwCmd)
{
    /* YES, Accel16! */
    LPACCEL16 lpAccelTbl;
    int i;

    if(!lpMsg) return FALSE;

#ifdef __WIN32OS2__
    if (!hAccel || !(lpAccelTbl = (LPACCEL16)LockResource(hAccel)))
#else
    if (!hAccel || !(lpAccelTbl = (LPACCEL16)LockResource16(hAccel)))
#endif
    {
	WARN_(accel)("invalid accel handle=%p\n", hAccel);
	return FALSE;
    }
    if((lpMsg->message != WM_KEYDOWN &&
	lpMsg->message != WM_KEYUP &&
	lpMsg->message != WM_SYSKEYDOWN &&
	lpMsg->message != WM_SYSKEYUP &&
	lpMsg->message != WM_CHAR)) return FALSE;

    TRACE_(accel)("hAccel=%p, cAccelEntries=%d,"
		"msg->hwnd=%p, msg->message=%04x, wParam=%08x, lParam=%08lx\n",
		hAccel, cAccelEntries,
		lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
    for(i = 0; i < cAccelEntries; i++)
    {
	if(lpAccelTbl[i].key != lpMsg->wParam)
	    continue;

	if(lpMsg->message == WM_CHAR)
	{
	    if(!(lpAccelTbl[i].fVirt & FALT) && !(lpAccelTbl[i].fVirt & FVIRTKEY))
	    {
		TRACE_(accel)("found accel for WM_CHAR: ('%c')\n", lpMsg->wParam & 0xff);
		goto found;
	    }
	}
	else
	{
	    if(lpAccelTbl[i].fVirt & FVIRTKEY)
	    {
		INT mask = 0;
		TRACE_(accel)("found accel for virt_key %04x (scan %04x)\n",
				lpMsg->wParam, HIWORD(lpMsg->lParam) & 0xff);
		if(GetKeyState(VK_SHIFT) & 0x8000) mask |= FSHIFT;
		if(GetKeyState(VK_CONTROL) & 0x8000) mask |= FCONTROL;
		if(GetKeyState(VK_MENU) & 0x8000) mask |= FALT;
		if(mask == (lpAccelTbl[i].fVirt & (FSHIFT | FCONTROL | FALT))) goto found;
		TRACE_(accel)("incorrect SHIFT/CTRL/ALT-state\n");
	    }
	    else
	    {
		if(!(lpMsg->lParam & 0x01000000))  /* no special_key */
		{
		    if((lpAccelTbl[i].fVirt & FALT) && (lpMsg->lParam & 0x20000000))
		    {						       /* ^^ ALT pressed */
			TRACE_(accel)("found accel for Alt-%c\n", lpMsg->wParam & 0xff);
			goto found;
		    }
		}
	    }
	}
    }	

    WARN_(accel)("couldn't translate accelerator key\n");
    return FALSE;

found:
    if(lpwCmd) *lpwCmd = lpAccelTbl[i].cmd;
    return TRUE;
}

/***********************************************************************
 * ReleaseStgMedium [OLE32.@]
 */
void WINAPI ReleaseStgMedium(
  STGMEDIUM* pmedium)
{
  switch (pmedium->tymed)
  {
    case TYMED_HGLOBAL:
    {
      if ( (pmedium->pUnkForRelease==0) &&
	   (pmedium->u.hGlobal!=0) )
	GlobalFree(pmedium->u.hGlobal);

      pmedium->u.hGlobal = 0;
      break;
    }
    case TYMED_FILE:
    {
      if (pmedium->u.lpszFileName!=0)
      {
	if (pmedium->pUnkForRelease==0)
	{
	  DeleteFileW(pmedium->u.lpszFileName);
	}
	
	CoTaskMemFree(pmedium->u.lpszFileName);
      }

      pmedium->u.lpszFileName = 0;
      break;
    }
    case TYMED_ISTREAM:
    {
      if (pmedium->u.pstm!=0)
      {
	IStream_Release(pmedium->u.pstm);
      }

      pmedium->u.pstm = 0;
      break;
    }
    case TYMED_ISTORAGE:
    {
      if (pmedium->u.pstg!=0)
      {
	IStorage_Release(pmedium->u.pstg);
      }

      pmedium->u.pstg = 0;
      break;
    }
    case TYMED_GDI:
    {
      if ( (pmedium->pUnkForRelease==0) &&
	   (pmedium->u.hGlobal!=0) )
	DeleteObject(pmedium->u.hGlobal);

      pmedium->u.hGlobal = 0;
      break;
    }
    case TYMED_MFPICT:
    {
      if ( (pmedium->pUnkForRelease==0) &&
	   (pmedium->u.hMetaFilePict!=0) )
      {
	LPMETAFILEPICT pMP = GlobalLock(pmedium->u.hGlobal);
	DeleteMetaFile(pMP->hMF);
	GlobalUnlock(pmedium->u.hGlobal);
	GlobalFree(pmedium->u.hGlobal);
      }

      pmedium->u.hMetaFilePict = 0;
      break;
    }
    case TYMED_ENHMF:
    {
      if ( (pmedium->pUnkForRelease==0) &&
	   (pmedium->u.hEnhMetaFile!=0) )
      {
	DeleteEnhMetaFile(pmedium->u.hEnhMetaFile);
      }

      pmedium->u.hEnhMetaFile = 0;
      break;
    }
    case TYMED_NULL:
    default:
      break;
  }
  pmedium->tymed=TYMED_NULL;

  /*
   * After cleaning up, the unknown is released
   */
  if (pmedium->pUnkForRelease!=0)
  {
    IUnknown_Release(pmedium->pUnkForRelease);
    pmedium->pUnkForRelease = 0;
  }
}

/***
 * OLEDD_Initialize()
 *
 * Initializes the OLE drag and drop data structures.
 */
static void OLEDD_Initialize()
{
    WNDCLASSA wndClass;

    ZeroMemory (&wndClass, sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)OLEDD_DragTrackerWindowProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(TrackerWindowInfo*);
    wndClass.hCursor       = 0;
    wndClass.hbrBackground = 0;
    wndClass.lpszClassName = OLEDD_DRAGTRACKERCLASS;

    RegisterClassA (&wndClass);

#ifdef __WIN32OS2__
    {
      IDropTarget *desktopdnd;

      desktopdnd = IDropTarget_Constructor();
      RegisterDragDrop(GetDesktopWindow(), desktopdnd);
    }
#endif
}

/***
 * OLEDD_UnInitialize()
 *
 * Releases the OLE drag and drop data structures.
 */
static void OLEDD_UnInitialize()
{
  /*
   * Simply empty the list.
   */
  while (targetListHead!=NULL)
  {
    RevokeDragDrop(targetListHead->hwndTarget);
  }
}

/***
 * OLEDD_InsertDropTarget()
 *
 * Insert the target node in the tree.
 */
static void OLEDD_InsertDropTarget(DropTargetNode* nodeToAdd)
{
  DropTargetNode*  curNode;
  DropTargetNode** parentNodeLink;

  /*
   * Iterate the tree to find the insertion point.
   */
  curNode        = targetListHead;
  parentNodeLink = &targetListHead;

  while (curNode!=NULL)
  {
    if (nodeToAdd->hwndTarget<curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a smaller HWND, go left
       */
      parentNodeLink = &curNode->prevDropTarget;
      curNode        =  curNode->prevDropTarget;
    }
    else if (nodeToAdd->hwndTarget>curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a larger HWND, go right
       */
      parentNodeLink = &curNode->nextDropTarget;
      curNode        =  curNode->nextDropTarget;
    }
    else
    {
      /*
       * The item was found in the list. It shouldn't have been there
       */
      assert(FALSE);
      return;
    }
  }

  /*
   * If we get here, we have found a spot for our item. The parentNodeLink
   * pointer points to the pointer that we have to modify.
   * The curNode should be NULL. We just have to establish the link and Voila!
   */
  assert(curNode==NULL);
  assert(parentNodeLink!=NULL);
  assert(*parentNodeLink==NULL);

  *parentNodeLink=nodeToAdd;
}

/***
 * OLEDD_ExtractDropTarget()
 *
 * Removes the target node from the tree.
 */
static DropTargetNode* OLEDD_ExtractDropTarget(HWND hwndOfTarget)
{
  DropTargetNode*  curNode;
  DropTargetNode** parentNodeLink;

  /*
   * Iterate the tree to find the insertion point.
   */
  curNode        = targetListHead;
  parentNodeLink = &targetListHead;

  while (curNode!=NULL)
  {
    if (hwndOfTarget<curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a smaller HWND, go left
       */
      parentNodeLink = &curNode->prevDropTarget;
      curNode        =  curNode->prevDropTarget;
    }
    else if (hwndOfTarget>curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a larger HWND, go right
       */
      parentNodeLink = &curNode->nextDropTarget;
      curNode        =  curNode->nextDropTarget;
    }
    else
    {
      /*
       * The item was found in the list. Detach it from it's parent and
       * re-insert it's kids in the tree.
       */
      assert(parentNodeLink!=NULL);
      assert(*parentNodeLink==curNode);

      /*
       * We arbitrately re-attach the left sub-tree to the parent.
       */
      *parentNodeLink = curNode->prevDropTarget;

      /*
       * And we re-insert the right subtree
       */
      if (curNode->nextDropTarget!=NULL)
      {
	OLEDD_InsertDropTarget(curNode->nextDropTarget);
      }

      /*
       * The node we found is still a valid node once we complete
       * the unlinking of the kids.
       */
      curNode->nextDropTarget=NULL;
      curNode->prevDropTarget=NULL;

      return curNode;
    }
  }

  /*
   * If we get here, the node is not in the tree
   */
  return NULL;
}

/***
 * OLEDD_FindDropTarget()
 *
 * Finds information about the drop target.
 */
static DropTargetNode* OLEDD_FindDropTarget(HWND hwndOfTarget)
{
  DropTargetNode*  curNode;

  /*
   * Iterate the tree to find the HWND value.
   */
  curNode        = targetListHead;

  while (curNode!=NULL)
  {
    if (hwndOfTarget<curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a smaller HWND, go left
       */
      curNode =  curNode->prevDropTarget;
    }
    else if (hwndOfTarget>curNode->hwndTarget)
    {
      /*
       * If the node we want to add has a larger HWND, go right
       */
      curNode =  curNode->nextDropTarget;
    }
    else
    {
      /*
       * The item was found in the list.
       */
      return curNode;
    }
  }

  /*
   * If we get here, the item is not in the list
   */
  return NULL;
}

/***
 * OLEDD_DragTrackerWindowProc()
 *
 * This method is the WindowProcedure of the drag n drop tracking
 * window. During a drag n Drop operation, an invisible window is created
 * to receive the user input and act upon it. This procedure is in charge
 * of this behavior.
 */
static LRESULT WINAPI OLEDD_DragTrackerWindowProc(
			 HWND   hwnd,
			 UINT   uMsg,
			 WPARAM wParam,
			 LPARAM   lParam)
{
  dprintf(("OLEDD_DragTrackerWindowProc(%x, %x, %x, %x)\n", hwnd, uMsg, wParam, lParam));
  switch (uMsg)
  {
    case WM_CREATE:
    {
      LPCREATESTRUCTA createStruct = (LPCREATESTRUCTA)lParam;

      SetWindowLongA(hwnd, 0, (LONG)createStruct->lpCreateParams);

      break;
    }
    case WM_MOUSEMOVE:
    {
      TrackerWindowInfo* trackerInfo = (TrackerWindowInfo*)GetWindowLongA(hwnd, 0);
      POINT            mousePos;

      /*
       * Get the current mouse position in screen coordinates.
       */
      mousePos.x = LOWORD(lParam);
      mousePos.y = HIWORD(lParam);
      ClientToScreen(hwnd, &mousePos);

      /*
       * Track the movement of the mouse.
       */
      #ifdef __WIN32OS2__
      wParam = OLEDD_GetButtonState();
      #endif
      OLEDD_TrackMouseMove(trackerInfo, mousePos, wParam);

      break;
    }
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
      TrackerWindowInfo* trackerInfo = (TrackerWindowInfo*)GetWindowLongA(hwnd, 0);
      POINT            mousePos;

      /*
       * Get the current mouse position in screen coordinates.
       */
      mousePos.x = LOWORD(lParam);
      mousePos.y = HIWORD(lParam);
      ClientToScreen(hwnd, &mousePos);

      /*
       * Notify everyone that the button state changed
       * TODO: Check if the "escape" key was pressed.
       */
      OLEDD_TrackStateChange(trackerInfo, mousePos, wParam);

      break;
    }
  }

  /*
   * This is a window proc after all. Let's call the default.
   */
  return DefWindowProcA (hwnd, uMsg, wParam, lParam);
}

/***
 * OLEDD_TrackMouseMove()
 *
 * This method is invoked while a drag and drop operation is in effect.
 * it will generate the appropriate callbacks in the drop source
 * and drop target. It will also provide the expected feedback to
 * the user.
 *
 * params:
 *    trackerInfo - Pointer to the structure identifying the
 *                  drag & drop operation that is currently
 *                  active.
 *    mousePos    - Current position of the mouse in screen
 *                  coordinates.
 *    keyState    - Contains the state of the shift keys and the
 *                  mouse buttons (MK_LBUTTON and the like)
 */
static void OLEDD_TrackMouseMove(
  TrackerWindowInfo* trackerInfo,
  POINT            mousePos,
  DWORD              keyState)
{
  HWND     hwndNewTarget;
  HRESULT  hr;

#ifdef __WIN32OS2__
  TRACE("OLEDD_TrackMouseMove: %p, (%d,%d), %x\n", trackerInfo, mousePos.x, mousePos.y, keyState);
#endif

  /*
   * Get the handle of the window under the mouse
   */
  hwndNewTarget = WindowFromPoint(mousePos);

#ifdef __WIN32OS2__
  dprintf(("OLEDD_TrackMouseMove: hwndNewTarget %x current %x", hwndNewTarget, trackerInfo->curDragTargetHWND));
#endif

  /*
   * Every time, we re-initialize the effects passed to the
   * IDropTarget to the effects allowed by the source.
   */
  *trackerInfo->pdwEffect = trackerInfo->dwOKEffect;

  /*
   * If we are hovering over the same target as before, send the
   * DragOver notification
   */
  if ( (trackerInfo->curDragTarget != 0) &&
       (trackerInfo->curTargetHWND == hwndNewTarget) )
  {
    POINTL  mousePosParam;

    /*
     * The documentation tells me that the coordinate should be in the target
     * window's coordinate space. However, the tests I made tell me the
     * coordinates should be in screen coordinates.
     */
    mousePosParam.x = mousePos.x;
    mousePosParam.y = mousePos.y;

#ifdef __WIN32OS2__
    hr =
#endif
    IDropTarget_DragOver(trackerInfo->curDragTarget,
			 keyState,
			 mousePosParam,
			 trackerInfo->pdwEffect);
#ifdef __WIN32OS2__
    TRACE("OLEDD_TrackMouseMove: IDropTarget_DragEnter -> %x, *pdwEffect=%x\n", hr, *trackerInfo->pdwEffect);
#endif
  }
  else
  {
    DropTargetNode* newDropTargetNode = 0;

    /*
     * If we changed window, we have to notify our old target and check for
     * the new one.
     */
    if (trackerInfo->curDragTarget!=0)
    {
#ifdef __WIN32OS2__
      hr =
#endif
      IDropTarget_DragLeave(trackerInfo->curDragTarget);
#ifdef __WIN32OS2__
      TRACE("OLEDD_TrackMouseMove: IDropTarget_DragLeave -> %x\n", hr);
#endif
    }

    /*
     * Make sure we're hovering over a window.
     */
    if (hwndNewTarget!=0)
    {
      /*
       * Find-out if there is a drag target under the mouse
       */
      HWND nexttar = hwndNewTarget;
      trackerInfo->curTargetHWND = hwndNewTarget;

      do {
	newDropTargetNode = OLEDD_FindDropTarget(nexttar);
      } while (!newDropTargetNode && (nexttar = GetParent(nexttar)) != 0);
      if(nexttar) hwndNewTarget = nexttar;

      trackerInfo->curDragTargetHWND = hwndNewTarget;
      trackerInfo->curDragTarget     = newDropTargetNode ? newDropTargetNode->dropTarget : 0;

      /*
       * If there is, notify it that we just dragged-in
       */
      if (trackerInfo->curDragTarget!=0)
      {
	POINTL  mousePosParam;
	
	/*
	 * The documentation tells me that the coordinate should be in the target
	 * window's coordinate space. However, the tests I made tell me the
	 * coordinates should be in screen coordinates.
	 */
	mousePosParam.x = mousePos.x;
	mousePosParam.y = mousePos.y;
	
#ifdef __WIN32OS2__
        hr =
#endif
	IDropTarget_DragEnter(trackerInfo->curDragTarget,
			      trackerInfo->dataObject,
			      keyState,
			      mousePosParam,
			      trackerInfo->pdwEffect);
#ifdef __WIN32OS2__
        TRACE("OLEDD_TrackMouseMove: IDropTarget_DragEnter -> %x, *pdwEffect=%x\n", hr, *trackerInfo->pdwEffect);
#endif
      }
    }
    else
    {
      /*
       * The mouse is not over a window so we don't track anything.
       */
      trackerInfo->curDragTargetHWND = 0;
      trackerInfo->curTargetHWND     = 0;
      trackerInfo->curDragTarget     = 0;
    }
  }

  /*
   * Now that we have done that, we have to tell the source to give
   * us feedback on the work being done by the target.  If we don't
   * have a target, simulate no effect.
   */
  if (trackerInfo->curDragTarget==0)
  {
    *trackerInfo->pdwEffect = DROPEFFECT_NONE;
  }

  hr = IDropSource_GiveFeedback(trackerInfo->dropSource,
  				*trackerInfo->pdwEffect);
#ifdef __WIN32OS2__
  TRACE("OLEDD_TrackMouseMove: IDropSource_GiveFeedback *pdwEffect=%x -> %x\n", *trackerInfo->pdwEffect, hr);
#endif

  /*
   * When we ask for feedback from the drop source, sometimes it will
   * do all the necessary work and sometimes it will not handle it
   * when that's the case, we must display the standard drag and drop
   * cursors.
   */
  if (hr==DRAGDROP_S_USEDEFAULTCURSORS)
  {
    if (*trackerInfo->pdwEffect & DROPEFFECT_MOVE)
    {
      SetCursor(LoadCursorA(OLE32_hInstance, MAKEINTRESOURCEA(1)));
    }
    else if (*trackerInfo->pdwEffect & DROPEFFECT_COPY)
    {
      SetCursor(LoadCursorA(OLE32_hInstance, MAKEINTRESOURCEA(2)));
    }
    else if (*trackerInfo->pdwEffect & DROPEFFECT_LINK)
    {
      SetCursor(LoadCursorA(OLE32_hInstance, MAKEINTRESOURCEA(3)));
    }
    else
    {
      SetCursor(LoadCursorA(OLE32_hInstance, MAKEINTRESOURCEA(0)));
    }
  }
}

/***
 * OLEDD_TrackStateChange()
 *
 * This method is invoked while a drag and drop operation is in effect.
 * It is used to notify the drop target/drop source callbacks when
 * the state of the keyboard or mouse button change.
 *
 * params:
 *    trackerInfo - Pointer to the structure identifying the
 *                  drag & drop operation that is currently
 *                  active.
 *    mousePos    - Current position of the mouse in screen
 *                  coordinates.
 *    keyState    - Contains the state of the shift keys and the
 *                  mouse buttons (MK_LBUTTON and the like)
 */
static void OLEDD_TrackStateChange(
  TrackerWindowInfo* trackerInfo,
  POINT            mousePos,
  DWORD              keyState)
{
#ifdef __WIN32OS2__
  TRACE("OLEDD_TrackStateChange: %p (%d,%d) %x\n", trackerInfo, mousePos.x, mousePos.y, keyState);
#endif

  /*
   * Ask the drop source what to do with the operation.
   */
  trackerInfo->returnValue = IDropSource_QueryContinueDrag(
			       trackerInfo->dropSource,
			       trackerInfo->escPressed,
			       keyState);
#ifdef __WIN32OS2__
  TRACE("OLEDD_TrackStateChange: IDropSource_QueryContinueDrag -> %x\n", trackerInfo->returnValue);
#endif

  /*
   * All the return valued will stop the operation except the S_OK
   * return value.
   */
  if (trackerInfo->returnValue!=S_OK)
  {
    /*
     * Make sure the message loop in DoDragDrop stops
     */
    trackerInfo->trackingDone = TRUE;

    /*
     * Release the mouse in case the drop target decides to show a popup
     * or a menu or something.
     */
    ReleaseCapture();

    /*
     * If we end-up over a target, drop the object in the target or
     * inform the target that the operation was cancelled.
     */
    if (trackerInfo->curDragTarget!=0)
    {
      switch (trackerInfo->returnValue)
      {
	/*
	 * If the source wants us to complete the operation, we tell
	 * the drop target that we just dropped the object in it.
	 */
        case DRAGDROP_S_DROP:
	{
	  POINTL  mousePosParam;
	
	  /*
	   * The documentation tells me that the coordinate should be
	   * in the target window's coordinate space. However, the tests
	   * I made tell me the coordinates should be in screen coordinates.
	   */
	  mousePosParam.x = mousePos.x;
	  mousePosParam.y = mousePos.y;
	
	  IDropTarget_Drop(trackerInfo->curDragTarget,
			   trackerInfo->dataObject,
			   keyState,
			   mousePosParam,
			   trackerInfo->pdwEffect);
	  break;
	}
	/*
	 * If the source told us that we should cancel, fool the drop
	 * target by telling it that the mouse left it's window.
	 * Also set the drop effect to "NONE" in case the application
	 * ignores the result of DoDragDrop.
	 */
        case DRAGDROP_S_CANCEL:
	  IDropTarget_DragLeave(trackerInfo->curDragTarget);
	  *trackerInfo->pdwEffect = DROPEFFECT_NONE;
	  break;

#ifdef __WIN32OS2__
#ifdef DEBUG
        default:
          TRACE("OLEDD_TrackStateChange: unknown return value %x\n", trackerInfo->returnValue);
          assert(FALSE);
          break;
#endif
#endif
      }
    }
  }
#ifdef __WIN32OS2__
  else  /* Read DoDragDrop() docs. This is point 5 in the remark section. */
    OLEDD_TrackMouseMove(trackerInfo, mousePos, keyState);
  TRACE("OLEDD_TrackStateChange: IDropSource_QueryContinueDrag -> %x\n", trackerInfo->returnValue);
#endif
}

/***
 * OLEDD_GetButtonState()
 *
 * This method will use the current state of the keyboard to build
 * a button state mask equivalent to the one passed in the
 * WM_MOUSEMOVE wParam.
 */
static DWORD OLEDD_GetButtonState(void)
{
  BYTE  keyboardState[256];
  DWORD keyMask = 0;

  GetKeyboardState(keyboardState);

  if ( (keyboardState[VK_SHIFT] & 0x80) !=0)
    keyMask |= MK_SHIFT;

  if ( (keyboardState[VK_CONTROL] & 0x80) !=0)
    keyMask |= MK_CONTROL;

#ifdef __WIN32OS2__
  if ( (keyboardState[VK_MENU] & 0x80) != 0)
    keyMask |= MK_ALT;
#endif

  if ( (keyboardState[VK_LBUTTON] & 0x80) !=0)
    keyMask |= MK_LBUTTON;

  if ( (keyboardState[VK_RBUTTON] & 0x80) !=0)
    keyMask |= MK_RBUTTON;

  if ( (keyboardState[VK_MBUTTON] & 0x80) !=0)
    keyMask |= MK_MBUTTON;


  return keyMask;
}

/***
 * OLEDD_GetButtonState()
 *
 * This method will read the default value of the registry key in
 * parameter and extract a DWORD value from it. The registry key value
 * can be in a string key or a DWORD key.
 *
 * params:
 *     regKey   - Key to read the default value from
 *     pdwValue - Pointer to the location where the DWORD
 *                value is returned. This value is not modified
 *                if the value is not found.
 */

static void OLEUTL_ReadRegistryDWORDValue(
  HKEY   regKey,
  DWORD* pdwValue)
{
  char  buffer[20];
  DWORD dwKeyType;
  DWORD cbData = 20;
  LONG  lres;

  lres = RegQueryValueExA(regKey,
			  "",
			  NULL,
			  &dwKeyType,
			  (LPBYTE)buffer,
			  &cbData);

  if (lres==ERROR_SUCCESS)
  {
    switch (dwKeyType)
    {
      case REG_DWORD:
	*pdwValue = *(DWORD*)buffer;
	break;
      case REG_EXPAND_SZ:
      case REG_MULTI_SZ:
      case REG_SZ:
	*pdwValue = (DWORD)strtoul(buffer, NULL, 10);
	break;
    }
  }
}


/******************************************************************************
 * OleDraw (OLE32.@)
 *
 * The operation of this function is documented literally in the WinAPI
 * documentation to involve a QueryInterface for the IViewObject interface,
 * followed by a call to IViewObject::Draw.
 */
HRESULT WINAPI OleDraw(
	IUnknown *pUnk,
	DWORD dwAspect,
	HDC hdcDraw,
	LPCRECT lprcBounds)
{
  HRESULT hres;
  IViewObject *viewobject;

  hres = IUnknown_QueryInterface(pUnk,
				 &IID_IViewObject,
				 (void**)&viewobject);

  if (SUCCEEDED(hres))
  {
    RECTL rectl;

    rectl.left = lprcBounds->left;
    rectl.right = lprcBounds->right;
    rectl.top = lprcBounds->top;
    rectl.bottom = lprcBounds->bottom;
    hres = IViewObject_Draw(viewobject, dwAspect, -1, 0, 0, 0, hdcDraw, &rectl, 0, 0, 0);

    IViewObject_Release(viewobject);
    return hres;
  }
  else
  {
    return DV_E_NOIVIEWOBJECT;
  }
}

/***********************************************************************
 *             OleTranslateAccelerator [OLE32.@]
 */
HRESULT WINAPI OleTranslateAccelerator (LPOLEINPLACEFRAME lpFrame,
                   LPOLEINPLACEFRAMEINFO lpFrameInfo, LPMSG lpmsg)
{
    WORD wID;

    TRACE("(%p,%p,%p)\n", lpFrame, lpFrameInfo, lpmsg);

    if (IsAccelerator(lpFrameInfo->haccel,lpFrameInfo->cAccelEntries,lpmsg,&wID))
        return IOleInPlaceFrame_TranslateAccelerator(lpFrame,lpmsg,wID);

    return S_FALSE;
}

/******************************************************************************
 *              OleCreate        [OLE32.@]
 *
 */
HRESULT WINAPI OleCreate(
	REFCLSID rclsid,
	REFIID riid,
	DWORD renderopt,
	LPFORMATETC pFormatEtc,
	LPOLECLIENTSITE pClientSite,
	LPSTORAGE pStg,
	LPVOID* ppvObj)
{
    HRESULT hres, hres1;
    IUnknown * pUnk = NULL;

    FIXME("\n\t%s\n\t%s semi-stub!\n", debugstr_guid(rclsid), debugstr_guid(riid));

    if (SUCCEEDED((hres = CoCreateInstance(rclsid, 0, CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER|CLSCTX_LOCAL_SERVER , riid, (LPVOID*)&pUnk))))
    {
        if (pClientSite)
        {
            IOleObject * pOE;
            IPersistStorage * pPS;
            if (SUCCEEDED((hres = IUnknown_QueryInterface( pUnk, &IID_IOleObject, (LPVOID*)&pOE))))
            {
                TRACE("trying to set clientsite %p\n", pClientSite);
                hres1 = IOleObject_SetClientSite(pOE, pClientSite);
                TRACE("-- result 0x%08lx\n", hres1);
                IOleObject_Release(pOE);
            }
            if (SUCCEEDED((hres = IUnknown_QueryInterface( pUnk, &IID_IPersistStorage, (LPVOID*)&pPS))))
            {
                TRACE("trying to set stg %p\n", pStg);
                hres1 = IPersistStorage_InitNew(pPS, pStg);
                TRACE("-- result 0x%08lx\n", hres1);
                IPersistStorage_Release(pPS);
            }
        }
    }

    *ppvObj = pUnk;

    TRACE("-- %p \n", pUnk);
    return hres;
}

/***********************************************************************
 *           OLE_FreeClipDataArray   [internal]
 *
 * NOTES:
 *  frees the data associated with an array of CLIPDATAs
 */
static void OLE_FreeClipDataArray(ULONG count, CLIPDATA * pClipDataArray)
{
    ULONG i;
    for (i = 0; i < count; i++)
        if (pClipDataArray[i].pClipData)
            CoTaskMemFree(pClipDataArray[i].pClipData);
}

HRESULT WINAPI FreePropVariantArray(ULONG,PROPVARIANT*);

/***********************************************************************
 *           PropVariantClear			    [OLE32.@]
 */
HRESULT WINAPI PropVariantClear(PROPVARIANT * pvar) /* [in/out] */
{
    TRACE("(%p)\n", pvar);

    if (!pvar)
        return S_OK;

    switch(pvar->vt)
    {
    case VT_STREAM:
    case VT_STREAMED_OBJECT:
    case VT_STORAGE:
    case VT_STORED_OBJECT:
        IUnknown_Release((LPUNKNOWN)pvar->u.pStream);
        break;
    case VT_CLSID:
    case VT_LPSTR:
    case VT_LPWSTR:
        /* pick an arbitary typed pointer - we don't care about the type
         * as we are just freeing it */
        CoTaskMemFree(pvar->u.puuid);
        break;
    case VT_BLOB:
    case VT_BLOB_OBJECT:
        CoTaskMemFree(pvar->u.blob.pBlobData);
        break;
    case VT_BSTR:
        FIXME("Need to load OLEAUT32 for SysFreeString\n");
        /* SysFreeString(pvar->u.bstrVal); */
        break;
   case VT_CF:
        if (pvar->u.pclipdata)
        {
            OLE_FreeClipDataArray(1, pvar->u.pclipdata);
            CoTaskMemFree(pvar->u.pclipdata);
        }
        break;
    default:
        if (pvar->vt & VT_ARRAY)
        {
            FIXME("Need to call SafeArrayDestroy\n");
            /* SafeArrayDestroy(pvar->u.caub); */
        }
        switch (pvar->vt & VT_VECTOR)
        {
        case VT_VARIANT:
            FreePropVariantArray(pvar->u.capropvar.cElems, pvar->u.capropvar.pElems);
            break;
        case VT_CF:
            OLE_FreeClipDataArray(pvar->u.caclipdata.cElems, pvar->u.caclipdata.pElems);
            break;
        case VT_BSTR:
        case VT_LPSTR:
        case VT_LPWSTR:
            FIXME("Freeing of vector sub-type not supported yet\n");
        }
        if (pvar->vt & VT_VECTOR)
        {
            /* pick an arbitary VT_VECTOR structure - they all have the same
             * memory layout */
            CoTaskMemFree(pvar->u.capropvar.pElems);
        }
    }

    ZeroMemory(pvar, sizeof(*pvar));

    return S_OK;
}

/***********************************************************************
 *           PropVariantCopy			    [OLE32.@]
 */
HRESULT WINAPI PropVariantCopy(PROPVARIANT *pvarDest,      /* [out] */
                               const PROPVARIANT *pvarSrc) /* [in] */
{
    ULONG len;
    TRACE("(%p, %p): stub:\n", pvarDest, pvarSrc);

    /* this will deal with most cases */
    CopyMemory(pvarDest, pvarSrc, sizeof(*pvarDest));

    switch(pvarSrc->vt)
    {
    case VT_STREAM:
    case VT_STREAMED_OBJECT:
    case VT_STORAGE:
    case VT_STORED_OBJECT:
        IUnknown_AddRef((LPUNKNOWN)pvarDest->u.pStream);
        break;
    case VT_CLSID:
        pvarDest->u.puuid = CoTaskMemAlloc(sizeof(CLSID));
        CopyMemory(pvarDest->u.puuid, pvarSrc->u.puuid, sizeof(CLSID));
        break;
    case VT_LPSTR:
        len = strlen(pvarSrc->u.pszVal);
        pvarDest->u.pszVal = CoTaskMemAlloc(len);
        CopyMemory(pvarDest->u.pszVal, pvarSrc->u.pszVal, len);
        break;
    case VT_LPWSTR:
        len = lstrlenW(pvarSrc->u.pwszVal);
        pvarDest->u.pwszVal = CoTaskMemAlloc(len);
        CopyMemory(pvarDest->u.pwszVal, pvarSrc->u.pwszVal, len);
        break;
    case VT_BLOB:
    case VT_BLOB_OBJECT:
        if (pvarSrc->u.blob.pBlobData)
        {
            len = pvarSrc->u.blob.cbSize;
            pvarDest->u.blob.pBlobData = CoTaskMemAlloc(len);
            CopyMemory(pvarDest->u.blob.pBlobData, pvarSrc->u.blob.pBlobData, len);
        }
        break;
    case VT_BSTR:
        FIXME("Need to copy BSTR\n");
        break;
    case VT_CF:
        if (pvarSrc->u.pclipdata)
        {
            len = pvarSrc->u.pclipdata->cbSize - sizeof(pvarSrc->u.pclipdata->ulClipFmt);
            CoTaskMemAlloc(len);
            CopyMemory(pvarDest->u.pclipdata->pClipData, pvarSrc->u.pclipdata->pClipData, len);
        }
        break;
    default:
        if (pvarSrc->vt & VT_ARRAY)
        {
            FIXME("Need to call SafeArrayCopy\n");
            /* SafeArrayCopy(...); */
        }
        if (pvarSrc->vt & VT_VECTOR)
        {
            int elemSize;
            switch(pvarSrc->vt & VT_VECTOR)
            {
            case VT_I1:       elemSize = sizeof(pvarSrc->u.cVal); break;
            case VT_UI1:      elemSize = sizeof(pvarSrc->u.bVal); break;
            case VT_I2:       elemSize = sizeof(pvarSrc->u.iVal); break;
            case VT_UI2:      elemSize = sizeof(pvarSrc->u.uiVal); break;
            case VT_BOOL:     elemSize = sizeof(pvarSrc->u.boolVal); break;
            case VT_I4:       elemSize = sizeof(pvarSrc->u.lVal); break;
            case VT_UI4:      elemSize = sizeof(pvarSrc->u.ulVal); break;
            case VT_R4:       elemSize = sizeof(pvarSrc->u.fltVal); break;
            case VT_R8:       elemSize = sizeof(pvarSrc->u.dblVal); break;
            case VT_ERROR:    elemSize = sizeof(pvarSrc->u.scode); break;
            case VT_I8:       elemSize = sizeof(pvarSrc->u.hVal); break;
            case VT_UI8:      elemSize = sizeof(pvarSrc->u.uhVal); break;
            case VT_CY:       elemSize = sizeof(pvarSrc->u.cyVal); break;
            case VT_DATE:     elemSize = sizeof(pvarSrc->u.date); break;
            case VT_FILETIME: elemSize = sizeof(pvarSrc->u.filetime); break;
            case VT_CLSID:    elemSize = sizeof(*pvarSrc->u.puuid); break;
            case VT_CF:       elemSize = sizeof(*pvarSrc->u.pclipdata); break;

            case VT_BSTR:
            case VT_LPSTR:
            case VT_LPWSTR:
            case VT_VARIANT:
            default:
                FIXME("Invalid element type: %ul\n", pvarSrc->vt & VT_VECTOR);
                return E_INVALIDARG;
            }
            len = pvarSrc->u.capropvar.cElems;
            pvarDest->u.capropvar.pElems = CoTaskMemAlloc(len * elemSize);
            if (pvarSrc->vt == (VT_VECTOR | VT_VARIANT))
            {
                ULONG i;
                for (i = 0; i < len; i++)
                    PropVariantCopy(&pvarDest->u.capropvar.pElems[i], &pvarSrc->u.capropvar.pElems[i]);
            }
            else if (pvarSrc->vt == (VT_VECTOR | VT_CF))
            {
                FIXME("Copy clipformats\n");
            }
            else if (pvarSrc->vt == (VT_VECTOR | VT_BSTR))
            {
                FIXME("Copy BSTRs\n");
            }
            else if (pvarSrc->vt == (VT_VECTOR | VT_LPSTR))
            {
                FIXME("Copy LPSTRs\n");
            }
            else if (pvarSrc->vt == (VT_VECTOR | VT_LPSTR))
            {
                FIXME("Copy LPWSTRs\n");
            }
            else
                CopyMemory(pvarDest->u.capropvar.pElems, pvarSrc->u.capropvar.pElems, len * elemSize);
        }
    }

    return S_OK;
}

/***********************************************************************
 *           FreePropVariantArray			    [OLE32.@]
 */
HRESULT WINAPI FreePropVariantArray(ULONG cVariants, /* [in] */
                                    PROPVARIANT *rgvars)    /* [in/out] */
{
    ULONG i;

    TRACE("(%lu, %p)\n", cVariants, rgvars);

    for(i = 0; i < cVariants; i++)
        PropVariantClear(&rgvars[i]);

    return S_OK;
}



#ifdef __WIN32OS2__
#include <dbglog.h>

/***********************************************************************
*   IEnumFORMATETC implementation
*/

typedef struct
{
    /* IUnknown fields */
    ICOM_VFIELD(IEnumFORMATETC);
    DWORD                        ref;
    /* IEnumFORMATETC fields */
    UINT        posFmt;
    UINT        countFmt;
    LPFORMATETC pFmt;
} IEnumFORMATETCImpl;

static HRESULT WINAPI IEnumFORMATETC_fnQueryInterface(LPENUMFORMATETC iface, REFIID riid, LPVOID* ppvObj);
static ULONG WINAPI IEnumFORMATETC_fnAddRef(LPENUMFORMATETC iface);
static ULONG WINAPI IEnumFORMATETC_fnRelease(LPENUMFORMATETC iface);
static HRESULT WINAPI IEnumFORMATETC_fnNext(LPENUMFORMATETC iface, ULONG celt, FORMATETC* rgelt, ULONG* pceltFethed);
static HRESULT WINAPI IEnumFORMATETC_fnSkip(LPENUMFORMATETC iface, ULONG celt);
static HRESULT WINAPI IEnumFORMATETC_fnReset(LPENUMFORMATETC iface);
static HRESULT WINAPI IEnumFORMATETC_fnClone(LPENUMFORMATETC iface, LPENUMFORMATETC* ppenum);

static struct ICOM_VTABLE(IEnumFORMATETC) efvt =
{
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IEnumFORMATETC_fnQueryInterface,
    IEnumFORMATETC_fnAddRef,
    IEnumFORMATETC_fnRelease,
    IEnumFORMATETC_fnNext,
    IEnumFORMATETC_fnSkip,
    IEnumFORMATETC_fnReset,
    IEnumFORMATETC_fnClone
};

static LPENUMFORMATETC IEnumFORMATETC_Constructor(UINT cfmt, const FORMATETC afmt[])
{
	IEnumFORMATETCImpl* ef;
	DWORD size=cfmt * sizeof(FORMATETC);
	
	ef=(IEnumFORMATETCImpl*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IEnumFORMATETCImpl));

	if(ef)
	{
	  ef->ref=1;
	  ICOM_VTBL(ef)=&efvt;

	  ef->countFmt = cfmt;
	  ef->pFmt = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

	  if (ef->pFmt)
	  {
	    memcpy(ef->pFmt, afmt, size);
	  }
	}

	TRACE("(%p)->(%u,%p)\n",ef, cfmt, afmt);
	return (LPENUMFORMATETC)ef;
}

static HRESULT WINAPI IEnumFORMATETC_fnQueryInterface(LPENUMFORMATETC iface, REFIID riid, LPVOID* ppvObj)
{
	ICOM_THIS(IEnumFORMATETCImpl,iface);
	TRACE("(%p)->(\n\tIID:\t%s,%p)\n",This,debugstr_guid(riid),ppvObj);

	*ppvObj = NULL;

	if(IsEqualIID(riid, &IID_IUnknown))
	{
	  *ppvObj = This;
	}
	else if(IsEqualIID(riid, &IID_IEnumFORMATETC))
	{
	  *ppvObj = (IEnumFORMATETC*)This;
	}

	if(*ppvObj)
	{
	  IUnknown_AddRef((IUnknown*)(*ppvObj));
	  TRACE("-- Interface: (%p)->(%p)\n",ppvObj,*ppvObj);
	  return S_OK;
	}
	TRACE("-- Interface: E_NOINTERFACE\n");
	return E_NOINTERFACE;

}

static ULONG WINAPI IEnumFORMATETC_fnAddRef(LPENUMFORMATETC iface)
{
	ICOM_THIS(IEnumFORMATETCImpl,iface);
	TRACE("(%p)->(count=%lu)\n",This, This->ref);
	return ++(This->ref);
}

static ULONG WINAPI IEnumFORMATETC_fnRelease(LPENUMFORMATETC iface)
{
	ICOM_THIS(IEnumFORMATETCImpl,iface);
	TRACE("(%p)->()\n",This);

    if (!--(This->ref))
	{
	  TRACE(" destroying IEnumFORMATETC(%p)\n",This);
      HeapFree(GetProcessHeap(),0, This->pFmt);
	  HeapFree(GetProcessHeap(),0,This);
	  return 0;
	}
	return This->ref;
}

static HRESULT WINAPI IEnumFORMATETC_fnNext(LPENUMFORMATETC iface, ULONG celt, FORMATETC *rgelt, ULONG *pceltFethed)
{
	ICOM_THIS(IEnumFORMATETCImpl,iface);
	int i;

	TRACE("(%p)->(%lu,%p)\n", This, celt, rgelt);

	if(!This->pFmt)return S_FALSE;
	if(!rgelt) return E_INVALIDARG;
	if (pceltFethed)  *pceltFethed = 0;

	for(i = 0; This->posFmt < This->countFmt && celt > i; i++)
   	{
	  *rgelt++ = This->pFmt[This->posFmt++];
	}

	if (pceltFethed) *pceltFethed = i;

	return ((i == celt) ? S_OK : S_FALSE);
}

static HRESULT WINAPI IEnumFORMATETC_fnSkip(LPENUMFORMATETC iface, ULONG celt)
{
	ICOM_THIS(IEnumFORMATETCImpl,iface);
	TRACE("(%p)->(num=%lu)\n", This, celt);

	if((This->posFmt + celt) >= This->countFmt) return S_FALSE;
	This->posFmt += celt;
	return S_OK;
}

static HRESULT WINAPI IEnumFORMATETC_fnReset(LPENUMFORMATETC iface)
{
	ICOM_THIS(IEnumFORMATETCImpl,iface);
	TRACE("(%p)->()\n", This);

    This->posFmt = 0;
    return S_OK;
}

static HRESULT WINAPI IEnumFORMATETC_fnClone(LPENUMFORMATETC iface, LPENUMFORMATETC* ppenum)
{
	ICOM_THIS(IEnumFORMATETCImpl,iface);
	TRACE("(%p)->(ppenum=%p)\n", This, ppenum);

	if (!ppenum) return E_INVALIDARG;
	*ppenum = IEnumFORMATETC_Constructor(This->countFmt, This->pFmt);
	return S_OK;
}

/***********************************************************************
*   IDataObject implementation
*/


typedef struct
{
	/* IUnknown fields */
	ICOM_VFIELD(IDataObject);
	DWORD		ref;

	/* IDataObject fields */
    LPFORMATETC pFormatEtc;
    LPSTGMEDIUM pStgMedium;
    DWORD       cDataCount;
} IDataObjectImpl;

static struct ICOM_VTABLE(IDataObject) dtovt;

/**************************************************************************
*  IDataObject_Constructor
*/
static LPDATAOBJECT IDataObject_Constructor(void)
{
	IDataObjectImpl* dto;

	dto = (IDataObjectImpl*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IDataObjectImpl));

	if (dto)
	{
	  dto->ref = 1;
	  ICOM_VTBL(dto) = &dtovt;
	}
	
	return (LPDATAOBJECT)dto;
}

/***************************************************************************
*  IDataObject_QueryInterface
*/
static HRESULT WINAPI IDataObject_fnQueryInterface(LPDATAOBJECT iface, REFIID riid, LPVOID * ppvObj)
{
	ICOM_THIS(IDataObjectImpl,iface);
	dprintf(("IDataObject_fnQueryInterface (%p)->(\n\tIID:\t%s,%p)\n",This,debugstr_guid(riid),ppvObj));

	*ppvObj = NULL;

	if(IsEqualIID(riid, &IID_IUnknown))          /*IUnknown*/
	{
	    *ppvObj = This;
	}
	else if(IsEqualIID(riid, &IID_IDataObject))  /*IDataObject*/
	{
	    *ppvObj = (IDataObject*)This;
	}

	if(*ppvObj)
	{
	    IUnknown_AddRef((IUnknown*)*ppvObj);
	    TRACE("-- Interface: (%p)->(%p)\n",ppvObj,*ppvObj);
	    return S_OK;
	}
	TRACE("-- Interface: E_NOINTERFACE\n");
	return E_NOINTERFACE;
}

/**************************************************************************
*  IDataObject_AddRef
*/
static ULONG WINAPI IDataObject_fnAddRef(LPDATAOBJECT iface)
{
	ICOM_THIS(IDataObjectImpl,iface);

	dprintf(("IDataObject_fnAddRef (%p)->(count=%lu)\n",This, This->ref));

	return ++(This->ref);
}

/**************************************************************************
*  IDataObject_Release
*/
static ULONG WINAPI IDataObject_fnRelease(LPDATAOBJECT iface)
{
	ICOM_THIS(IDataObjectImpl,iface);
	dprintf(("IDataObject_fnRelease (%p)->()\n",This));

    if (!--(This->ref))
	{
	  TRACE(" destroying IDataObject(%p)\n",This);
	  HeapFree(GetProcessHeap(),0,This);
	  return 0;
	}
	return This->ref;
}

/**************************************************************************
* IDataObject_fnGetData
*/
static HRESULT WINAPI IDataObject_fnGetData(LPDATAOBJECT iface, LPFORMATETC pformatetcIn, STGMEDIUM *pmedium)
{
    int i;

	ICOM_THIS(IDataObjectImpl,iface);

    if(pformatetcIn == NULL || pmedium == NULL)
        return E_INVALIDARG;

    dprintf(("IDataObject_fnGetData %x %x", pformatetcIn, pmedium));

	if(pformatetcIn->cfFormat != CF_HDROP && pformatetcIn->cfFormat != CF_TEXT && pformatetcIn->cfFormat != CF_UNICODETEXT)
	{
        FIXME("-- expected clipformat not implemented\n");
        return (E_INVALIDARG);
    }

	/* check our formats table what we have */
	for (i=0; i<This->cDataCount; i++)
	{
	    if ((This->pFormatEtc[i].cfFormat == pformatetcIn->cfFormat)
	         && (This->pFormatEtc[i].tymed == pformatetcIn->tymed))
	    {
            pmedium->u.hGlobal = This->pStgMedium[i].u.hGlobal;
	        break;
	    }
	}
	if (pmedium->u.hGlobal)
	{
	    pmedium->tymed = TYMED_HGLOBAL;
	    pmedium->pUnkForRelease = NULL;
	    return S_OK;
	}
	return E_OUTOFMEMORY;
}

static HRESULT WINAPI IDataObject_fnGetDataHere(LPDATAOBJECT iface, LPFORMATETC pformatetc, STGMEDIUM *pmedium)
{
	ICOM_THIS(IDataObjectImpl,iface);
	
    dprintf(("IDataObject_fnGetDataHere %x %x STUB", pformatetc, pmedium));
	return E_NOTIMPL;
}

static HRESULT WINAPI IDataObject_fnQueryGetData(LPDATAOBJECT iface, LPFORMATETC pformatetc)
{
	ICOM_THIS(IDataObjectImpl,iface);
	UINT i;
	
	dprintf(("IDataObject_fnQueryGetData (%p)->(fmt=0x%08x tym=0x%08lx)\n", This, pformatetc->cfFormat, pformatetc->tymed));
	
	if(!(DVASPECT_CONTENT & pformatetc->dwAspect))
	  return DV_E_DVASPECT;

    if(This->pFormatEtc == NULL) {
        return DV_E_TYMED;
    }
	/* check our formats table what we have */
	for (i=0; i<This->cDataCount; i++)
	{
	    if ((This->pFormatEtc[i].cfFormat == pformatetc->cfFormat)
	        && (This->pFormatEtc[i].tymed == pformatetc->tymed))
	    {
    	    return S_OK;
	    }
	}

	return DV_E_TYMED;
}

static HRESULT WINAPI IDataObject_fnGetCanonicalFormatEtc(LPDATAOBJECT iface, LPFORMATETC pformatectIn, LPFORMATETC pformatetcOut)
{
	ICOM_THIS(IDataObjectImpl,iface);
	
    dprintf(("IDataObject_fnGetCanonicalFormatEtc STUB"));
    return DATA_S_SAMEFORMATETC;
}

static HRESULT WINAPI IDataObject_fnSetData(LPDATAOBJECT iface, LPFORMATETC pformatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
    LPFORMATETC pfeNew, pfeTemp;
    LPSTGMEDIUM psmNew, psmTemp;
	ICOM_THIS(IDataObjectImpl,iface);

    //no need for more than one item
    if(This->cDataCount != 0) {
        DebugInt3();
        return E_OUTOFMEMORY;
    }
    This->cDataCount++;

    pfeNew = malloc(sizeof(FORMATETC) * This->cDataCount);
    psmNew = malloc(sizeof(STGMEDIUM) * This->cDataCount);

    if(pfeNew && psmNew)
    {
        memset(pfeNew, 0, sizeof(FORMATETC) * This->cDataCount);
        memset(psmNew, 0, sizeof(STGMEDIUM) * This->cDataCount);

        /* copy the existing data */
        if(This->pFormatEtc)
        {
            memcpy(pfeNew, This->pFormatEtc, sizeof(FORMATETC) * (This->cDataCount - 1));
        }
        if(This->pStgMedium)
        {
            memcpy(psmNew, This->pStgMedium, sizeof(STGMEDIUM) * (This->cDataCount - 1));
        }

        /* add the new data */
        pfeNew[This->cDataCount - 1] = *pformatetc;
        if(fRelease)
        {
            psmNew[This->cDataCount - 1] = *pmedium;
        }
        else
        {
            DebugInt3();
//            CopyStgMedium(pmedium, &psmNew[This->cDataCount - 1]);
        }

        pfeTemp = This->pFormatEtc;
        This->pFormatEtc = pfeNew;
        pfeNew = pfeTemp;

        psmTemp = This->pStgMedium;
        This->pStgMedium = psmNew;
        psmNew = psmTemp;
    }

    if(pfeNew)
        free(pfeNew);

    if(psmNew)
        free(psmNew);

    if(This->pFormatEtc && This->pStgMedium)
        return S_OK;

    return E_OUTOFMEMORY;
}

static HRESULT WINAPI IDataObject_fnEnumFormatEtc(LPDATAOBJECT iface, DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc)
{
	ICOM_THIS(IDataObjectImpl,iface);

	TRACE("(%p)->()\n", This);
	*ppenumFormatEtc=NULL;

	/* only get data */
	if (DATADIR_GET == dwDirection)
	{
	  *ppenumFormatEtc = IEnumFORMATETC_Constructor(This->cDataCount, This->pFormatEtc);
	  return (*ppenumFormatEtc) ? S_OK : E_FAIL;
	}
	
	return E_NOTIMPL;
}

static HRESULT WINAPI IDataObject_fnDAdvise(LPDATAOBJECT iface, FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	ICOM_THIS(IDataObjectImpl,iface);

    dprintf(("IDataObject_fnDAdvise STUB"));
	return E_NOTIMPL;
}
static HRESULT WINAPI IDataObject_fnDUnadvise(LPDATAOBJECT iface, DWORD dwConnection)
{
	ICOM_THIS(IDataObjectImpl,iface);

    dprintf(("IDataObject_fnDUnadvise STUB"));
	return E_NOTIMPL;
}
static HRESULT WINAPI IDataObject_fnEnumDAdvise(LPDATAOBJECT iface, IEnumSTATDATA **ppenumAdvise)
{
	ICOM_THIS(IDataObjectImpl,iface);

    dprintf(("IDataObject_fnEnumDAdvise STUB"));
    return OLE_E_ADVISENOTSUPPORTED;
}

static struct ICOM_VTABLE(IDataObject) dtovt =
{
	ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
	IDataObject_fnQueryInterface,
	IDataObject_fnAddRef,
	IDataObject_fnRelease,
	IDataObject_fnGetData,
	IDataObject_fnGetDataHere,
	IDataObject_fnQueryGetData,
	IDataObject_fnGetCanonicalFormatEtc,
	IDataObject_fnSetData,
	IDataObject_fnEnumFormatEtc,
	IDataObject_fnDAdvise,
	IDataObject_fnDUnadvise,
	IDataObject_fnEnumDAdvise
};

//******************************************************************************
//******************************************************************************
BOOL WINAPI OLEDD_AcceptsDragDrop(HWND hwnd)
{
    DropTargetNode *pTarget;

    /*
     * Find-out if there is a drag target under the mouse
     */
    HWND nexttar = hwnd;
    do {
        pTarget = OLEDD_FindDropTarget(nexttar);
    }
    while(!pTarget && (nexttar = GetParent(nexttar)) != 0);

    if(pTarget != NULL) {
        dprintf(("OLEDD_AcceptsDragDrop %x accepted", hwnd));
        return TRUE;
    }
    dprintf(("OLEDD_AcceptsDragDrop %x refused", hwnd));
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI OLEDD_DropFiles(HWND hwnd)
{
    DropTargetNode *pTarget;
    DWORD           keyState = 0;
    POINTL          mousePosParam;
    POINT           mousePos;
    HWND            nexttar = hwnd;

    dprintf(("OLEDD_DropFiles %x", hwnd));

    do {
        pTarget = OLEDD_FindDropTarget(nexttar);
    }
    while(!pTarget && (nexttar = GetParent(nexttar)) != 0);
    if(pTarget == NULL) {
        return FALSE;
    }

    /*
     * The documentation tells me that the coordinate should be in the target
     * window's coordinate space. However, the tests I made tell me the
     * coordinates should be in screen coordinates.
     */
    GetCursorPos(&mousePos);
    mousePosParam.x = mousePos.x;
    mousePosParam.y = mousePos.y;

    if(GetKeyState(VK_SHIFT)   & 0x8000) keyState |= MK_SHIFT;
    if(GetKeyState(VK_CONTROL) & 0x8000) keyState |= MK_CONTROL;
    if(GetKeyState(VK_MENU)    & 0x8000) keyState |= MK_ALT;
    if(GetKeyState(VK_LBUTTON) & 0x8000) keyState |= MK_LBUTTON;
    if(GetKeyState(VK_RBUTTON) & 0x8000) keyState |= MK_RBUTTON;
    if(GetKeyState(VK_MBUTTON) & 0x8000) keyState |= MK_MBUTTON;

    return IDropTarget_Drop(pTarget->dropTarget, pTarget->pDataObject,
                            keyState, mousePosParam, &pTarget->dwEffect) == S_OK;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OLEDD_DragOver(HWND hwnd, DWORD dwEffect)
{
    DropTargetNode *pTarget;
    DWORD           keyState = 0;
    POINTL          mousePosParam;
    POINT           mousePos;
    HWND            nexttar = hwnd;

    dprintf(("OLEDD_DragOver %x %d", hwnd, dwEffect));

    do {
        pTarget = OLEDD_FindDropTarget(nexttar);
    }
    while(!pTarget && (nexttar = GetParent(nexttar)) != 0);
    if(pTarget == NULL) {
        return FALSE;
    }

    /*
     * The documentation tells me that the coordinate should be in the target
     * window's coordinate space. However, the tests I made tell me the
     * coordinates should be in screen coordinates.
     */
    GetCursorPos(&mousePos);
    mousePosParam.x = mousePos.x;
    mousePosParam.y = mousePos.y;

    if(GetKeyState(VK_SHIFT)   & 0x8000) keyState |= MK_SHIFT;
    if(GetKeyState(VK_CONTROL) & 0x8000) keyState |= MK_CONTROL;
    if(GetKeyState(VK_MENU)    & 0x8000) keyState |= MK_ALT;
    if(GetKeyState(VK_LBUTTON) & 0x8000) keyState |= MK_LBUTTON;
    if(GetKeyState(VK_RBUTTON) & 0x8000) keyState |= MK_RBUTTON;
    if(GetKeyState(VK_MBUTTON) & 0x8000) keyState |= MK_MBUTTON;
    return IDropTarget_DragOver(pTarget->dropTarget, keyState, mousePosParam, &dwEffect) == S_OK;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OLEDD_DragEnter(HWND hwnd, HDROP hDrop, DWORD dwEffect)
{
    FORMATETC       fe;
    STGMEDIUM       medium;
    DropTargetNode *pTarget;
    DWORD           keyState = 0;
    POINTL          mousePosParam;
    POINT           mousePos;
    HWND            nexttar = hwnd;

    dprintf(("OLEDD_DragEnter %x %x %d", hwnd, hDrop, dwEffect));

    do {
        pTarget = OLEDD_FindDropTarget(nexttar);
    }
    while(!pTarget && (nexttar = GetParent(nexttar)) != 0);
    if(pTarget == NULL) {
        return FALSE;
    }

    /*
     * The documentation tells me that the coordinate should be in the target
     * window's coordinate space. However, the tests I made tell me the
     * coordinates should be in screen coordinates.
     */
    GetCursorPos(&mousePos);
    mousePosParam.x = mousePos.x;
    mousePosParam.y = mousePos.y;

    if(GetKeyState(VK_SHIFT)   & 0x8000) keyState |= MK_SHIFT;
    if(GetKeyState(VK_CONTROL) & 0x8000) keyState |= MK_CONTROL;
    if(GetKeyState(VK_MENU)    & 0x8000) keyState |= MK_ALT;
    if(GetKeyState(VK_LBUTTON) & 0x8000) keyState |= MK_LBUTTON;
    if(GetKeyState(VK_RBUTTON) & 0x8000) keyState |= MK_RBUTTON;
    if(GetKeyState(VK_MBUTTON) & 0x8000) keyState |= MK_MBUTTON;

    /* Note: It's the application's responsibility to free hDrop */
    /* TODO: Possible memory leak if app never calls GetData */
    fe.cfFormat = CF_HDROP;
    fe.ptd      = NULL;
    fe.dwAspect = DVASPECT_CONTENT;
    fe.lindex   = -1;
    fe.tymed    = TYMED_HGLOBAL;

    medium.u.hGlobal      = hDrop;
    medium.tymed          = TYMED_HGLOBAL;
    medium.pUnkForRelease = NULL;

    pTarget->fDragEnter = TRUE;
    pTarget->dwEffect   = dwEffect;

    if(pTarget->pDataObject) {
        IDataObject_Release(pTarget->pDataObject);
    }

    pTarget->pDataObject = IDataObject_Constructor();
    IDataObject_SetData(pTarget->pDataObject, &fe, &medium, TRUE);
    return IDropTarget_DragEnter(pTarget->dropTarget, pTarget->pDataObject, keyState, mousePosParam, &dwEffect) == S_OK;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OLEDD_DragLeave(HWND hwnd)
{
    DropTargetNode *pTarget;
    HWND            nexttar = hwnd;

    dprintf(("OLEDD_DragLeave %x", hwnd));

    do {
        pTarget = OLEDD_FindDropTarget(nexttar);
    }
    while(!pTarget && (nexttar = GetParent(nexttar)) != 0);
    if(pTarget == NULL) {
        return FALSE;
    }
    pTarget->fDragEnter = FALSE;
    if(pTarget->pDataObject) {
        IDataObject_Release(pTarget->pDataObject);
        pTarget->pDataObject = NULL;
    }
    return IDropTarget_DragLeave(pTarget->dropTarget) == S_OK;
}
//******************************************************************************
//******************************************************************************

#include <oslibdnd.h>

typedef struct
{
    /* IUnknown fields */
    ICOM_VFIELD(IDropTarget);
    DWORD ref;

    LPVOID       lpDnDData;
    LPSTR        lpOS2StringData;
    LPVOID       lpDragStruct;
    POINTL       pt;
    HGLOBAL      hDndData;
    FORMATETC    format;
    STGMEDIUM    medium;
    LPDATAOBJECT pDataObject;
} IDropTargetImpl;


/***************************************************************************
*  IDropTarget_QueryInterface
*/
static HRESULT WINAPI IDropTarget_fnQueryInterface(IDropTarget *iface, REFIID riid, LPVOID * ppvObj)
{
    ICOM_THIS(IDropTargetImpl,iface);

    dprintf(("IDropTarget_fnQueryInterface (%p)->(\n\tIID:\t%s,%p)\n",This,debugstr_guid(riid),ppvObj));

    *ppvObj = NULL;

    if(IsEqualIID(riid, &IID_IUnknown))          /*IUnknown*/
    {
        *ppvObj = This;
    }
    else if(IsEqualIID(riid, &IID_IDropTarget))  /*IDropTarget*/
    {
        *ppvObj = (IDropTarget*)This;
    }

    if(*ppvObj)
    {
        IUnknown_AddRef((IUnknown*)*ppvObj);
        TRACE("-- Interface: (%p)->(%p)\n",ppvObj,*ppvObj);
        return S_OK;
    }
    TRACE("-- Interface: E_NOINTERFACE\n");
    return E_NOINTERFACE;
}

/**************************************************************************
*  IDropTarget_AddRef
*/
static ULONG WINAPI IDropTarget_fnAddRef(IDropTarget *iface)
{
    ICOM_THIS(IDropTargetImpl,iface);

    dprintf(("IDropTarget_fnAddRef (%p)->(count=%lu)\n",This, This->ref));

    return ++(This->ref);
}

/**************************************************************************
*  IDropTarget_fnCleanup
*/
static ULONG WINAPI IDropTarget_fnCleanup(IDropTarget *iface)
{
    ICOM_THIS(IDropTargetImpl,iface);

    if(This->pDataObject) {
        IDataObject_Release(This->pDataObject);
        This->pDataObject = 0;
        if(This->lpDnDData) {
            HeapFree(GetProcessHeap(), 0, This->lpDnDData);
            This->lpDnDData = NULL;
        }
        if(This->lpOS2StringData) {
            HeapFree(GetProcessHeap(), 0, This->lpOS2StringData);
            This->lpOS2StringData = NULL;
        }
        if(This->lpDragStruct) {
            OSLibFreeDragStruct(This->lpDragStruct);
            This->lpDragStruct = NULL;
        }
    }
    return S_OK;
}
/**************************************************************************
*  IDropTarget_Release
*/
static ULONG WINAPI IDropTarget_fnRelease(IDropTarget *iface)
{
    ICOM_THIS(IDropTargetImpl,iface);

    dprintf(("IDropTarget_fnRelease (%p)->(count=%lu)\n",This, This->ref));

    IDropTarget_fnCleanup(iface);
    if(This->ref == 1) {
        if(This->hDndData) {
            GlobalFree(This->hDndData);
            This->hDndData = 0;
        }
    }
    return --(This->ref);
}
//******************************************************************************
//******************************************************************************
static HRESULT WINAPI IDropTarget_fnDragEnter(IDropTarget *iface, IDataObject* pDataObject,
                                DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
    HRESULT ret;
    DWORD   size, dwEffect;
    LPVOID  lpData;
    DROPFILES *lpDrop;
    LPSTR     *lpStringData;
    ICOM_THIS(IDropTargetImpl,iface);
    DWORD   supportedformats[] = {CF_HDROP, CF_TEXT};
    int     i;

    dprintf(("IDropTarget_fnDragEnter %x (%d,%d)", grfKeyState, pt.x, pt.y));
    IDropTarget_fnCleanup(iface);

    IDataObject_AddRef(pDataObject);
    This->pDataObject = pDataObject;

#ifdef DEBUG
    {
      IEnumFORMATETC *enumfmt = NULL;

      if(IDataObject_EnumFormatEtc(pDataObject, DATADIR_GET, &enumfmt) == S_OK) {
          for (;;) {
	
             FORMATETC tmp;
             ULONG     actual = 1, res;

             res = IEnumFORMATETC_Next(enumfmt, 1, &tmp, &actual);

             if(res != S_OK) break;

             dprintf(("format %x typed %x", tmp.cfFormat, tmp.tymed));
          }
          IEnumFORMATETC_Release(enumfmt);
      }
    }
#endif

    for(i=0;i<sizeof(supportedformats)/sizeof(supportedformats[0]);i++)
    {
        This->format.cfFormat = supportedformats[i];
        This->format.ptd      = NULL;
        This->format.dwAspect = DVASPECT_CONTENT;
        This->format.lindex   = -1;
        This->format.tymed    = TYMED_HGLOBAL;
        ret = IDataObject_GetData(pDataObject, &This->format, &This->medium);
        if(ret != S_OK) {
            dprintf(("IDataObject_GetData failed with %x", ret));
            continue;
        }
        size = GlobalSize(This->medium.u.hGlobal);
        if(size == 0) {
            dprintf(("GlobalSize failed for %x", This->medium.u.hGlobal));
            ReleaseStgMedium(&This->medium);
            return E_OUTOFMEMORY;
        }
        dprintf(("handle %x size %d, format %x tymed %x", This->medium.u.hGlobal, size, This->format.cfFormat, This->format.tymed));

        if(size == 1) {//empty string; use previous data
             if(This->hDndData == 0) {
                 DebugInt3();
                 ReleaseStgMedium(&This->medium);
                 return E_OUTOFMEMORY;
             }
             This->medium.u.hGlobal = This->hDndData;

             dprintf(("Reuse old global handle %x", This->hDndData));
             size = GlobalSize(This->medium.u.hGlobal);
             if(size == 0) {
                 dprintf(("GlobalSize failed for %x", This->medium.u.hGlobal));
                 ReleaseStgMedium(&This->medium);
                 return E_OUTOFMEMORY;
             }
             dprintf(("handle %x size %d, format %x tymed %x", This->medium.u.hGlobal, size, This->format.cfFormat, This->format.tymed));
        }
        else This->hDndData = This->medium.u.hGlobal;

        This->lpDnDData = (LPVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
        if(This->lpDnDData == NULL) {
            dprintf(("HeapAlloc failed for %d bytes", size));
            return E_OUTOFMEMORY;
        }
        This->lpOS2StringData = (LPVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size*2);
        if(This->lpOS2StringData == NULL) {
            dprintf(("HeapAlloc failed for %d bytes", size));
            return E_OUTOFMEMORY;
        }
        lpData = GlobalLock(This->medium.u.hGlobal);

        if(This->format.cfFormat == CF_HDROP) {
            memcpy(This->lpDnDData, lpData, size);
            lpDrop = (DROPFILES *)This->lpDnDData;
            lpStringData = (LPSTR)(lpDrop) + lpDrop->pFiles;
            if(lpDrop->fWide == FALSE) {
                 OemToCharA((LPSTR)lpStringData, (LPSTR)This->lpOS2StringData);
            }
            else {
                 int len;
                 len = lstrlenW((LPWSTR)lpStringData);
                 WideCharToMultiByte( CP_OEMCP, 0, (LPWSTR)lpStringData, len, (LPSTR)This->lpOS2StringData, len, NULL, NULL );
            }
        }
        else
        if(This->format.cfFormat == CF_TEXT) {
            strcpy(This->lpDnDData, lpData);
            OemToCharA( This->lpDnDData, This->lpOS2StringData );
        }
        dprintf(("Drop string %s", This->lpOS2StringData));
        GlobalUnlock(This->medium.u.hGlobal);

        This->pt = pt;

        This->lpDragStruct = OSLibCreateDragStruct(hwndTracker, This->pt.x, This->pt.y, This->lpOS2StringData);
        if(This->lpDragStruct == NULL) {
            dprintf(("OSLibCreateDragStruct"));
            ReleaseStgMedium(&This->medium);
            return E_OUTOFMEMORY;
        }

        dwEffect = OSLibDragOver(This->lpDragStruct, pt.x, pt.y);
        break;
    }
    if(ret != S_OK) {
        dprintf(("IDataObject_GetData failed (fatal) with %x", ret));
        return ret;
    }
    if(pdwEffect) {
        *pdwEffect = dwEffect;
    }

    return S_OK;
}
//******************************************************************************
//******************************************************************************
static HRESULT WINAPI IDropTarget_fnDragOver(IDropTarget *iface, DWORD grfKeyState, POINTL pt,
                               DWORD* pdwEffect)
{
    DWORD dwEffect;
    ICOM_THIS(IDropTargetImpl,iface);

    dprintf(("IDropTarget_fnDragOver %x (%d,%d)", grfKeyState, pt.x, pt.y));

    dwEffect = OSLibDragOver(This->lpDragStruct, pt.x, pt.y);
    if(pdwEffect) {
        *pdwEffect = dwEffect;
    }
    return S_OK;
}
//******************************************************************************
//******************************************************************************
static HRESULT WINAPI IDropTarget_fnDragLeave(IDropTarget *iface)
{
    ICOM_THIS(IDropTargetImpl,iface);

    dprintf(("IDropTarget_fnDragLeave"));

    OSLibDragLeave(This->lpDragStruct);

    IDropTarget_fnCleanup(iface);
    return S_OK;
}
//******************************************************************************
//******************************************************************************
static HRESULT WINAPI IDropTarget_fnDrop(IDropTarget *iface, IDataObject* pDataObject, DWORD grfKeyState,
                           POINTL pt, DWORD* pdwEffect)
{
    DWORD dwEffect;
    ICOM_THIS(IDropTargetImpl,iface);

    dprintf(("IDropTarget_fnDrop %x (%d,%d) %s", grfKeyState, pt.x, pt.y, This->lpOS2StringData));

    dwEffect = OSLibDragDrop(This->lpDragStruct, pt.x, pt.y, This->lpOS2StringData);
    if(pdwEffect) {
        *pdwEffect = dwEffect;
    }
    return S_OK;
}
//******************************************************************************
//******************************************************************************

static struct ICOM_VTABLE(IDropTarget) droptarget =
{
	ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
	IDropTarget_fnQueryInterface,
	IDropTarget_fnAddRef,
	IDropTarget_fnRelease,
	IDropTarget_fnDragEnter,
	IDropTarget_fnDragOver,
	IDropTarget_fnDragLeave,
	IDropTarget_fnDrop,
};


/**************************************************************************
*  IDropTarget_Constructor
*/
static IDropTarget *IDropTarget_Constructor()
{
	IDropTargetImpl* dto;

	dto = (IDropTargetImpl*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(IDropTargetImpl));

	if (dto)
	{
	  dto->ref = 1;
          dto->pDataObject = NULL;
          dto->lpDnDData   = NULL;
          dto->lpOS2StringData = NULL;
          dto->hDndData = 0;
	  ICOM_VTBL(dto) = &droptarget;
	}
	
	return (IDropTarget *)dto;
}

#endif

#ifndef __WIN32OS2__
/******************************************************************************
 * OleMetaFilePictFromIconAndLabel (OLE2.56)
 *
 * Returns a global memory handle to a metafile which contains the icon and
 * label given.
 * I guess the result of that should look somehow like desktop icons.
 * If no hIcon is given, we load the icon via lpszSourceFile and iIconIndex.
 * This code might be wrong at some places.
 */
HGLOBAL16 WINAPI OleMetaFilePictFromIconAndLabel16(
	HICON16 hIcon,
	LPCOLESTR16 lpszLabel,
	LPCOLESTR16 lpszSourceFile,
	UINT16 iIconIndex
) {
    METAFILEPICT16 *mf;
    HGLOBAL16 hmf;
    HDC16 hdc;

    FIXME("(%04x, '%s', '%s', %d): incorrect metrics, please try to correct them !\n\n\n", hIcon, lpszLabel, lpszSourceFile, iIconIndex);

    if (!hIcon) {
        if (lpszSourceFile) {
	    HINSTANCE16 hInstance = LoadLibrary16(lpszSourceFile);

	    /* load the icon at index from lpszSourceFile */
	    hIcon = (HICON16)LoadIconA(hInstance, (LPCSTR)(DWORD)iIconIndex);
	    FreeLibrary16(hInstance);
	} else
	    return (HGLOBAL)NULL;
    }

    hdc = CreateMetaFile16(NULL);
    DrawIcon(hdc, 0, 0, hIcon); /* FIXME */
    TextOutA(hdc, 0, 0, lpszLabel, 1); /* FIXME */
    hmf = GlobalAlloc16(0, sizeof(METAFILEPICT16));
    mf = (METAFILEPICT16 *)GlobalLock16(hmf);
    mf->mm = MM_ANISOTROPIC;
    mf->xExt = 20; /* FIXME: bogus */
    mf->yExt = 20; /* dito */
    mf->hMF = CloseMetaFile16(hdc);
    return hmf;
}
#endif

/******************************************************************************
 * DllDebugObjectRPCHook (OLE32.@)
 * turns on and off internal debugging,  pointer is only used on macintosh
 */

BOOL WINAPI DllDebugObjectRPCHook(BOOL b, void *dummy)
{
  FIXME("stub\n");
  return TRUE;
}
