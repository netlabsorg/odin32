/* $Id: ole32.cpp,v 1.1 2001-04-26 19:26:12 sandervl Exp $ */
/* 
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */
/* 
 * COM/OLE misc. functions.
 * 
 * 1/7/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Some portions from Wine Implementation
 *   Copyright 1995  Martin von Loewis
 *   Copyright 1998  Justin Bradford
 *   Copyright 1999  Francis Beaudet
 *   Copyright 1999  Sylvain St-Germain
 */

#include "ole32.h"

#include "oString.h"
#include "moniker.h"		// RunningObjectTableImpl_***
#include "filemoniker.h"	// FileMonikerImpl_***

// ======================================================================
// Local Data
// ======================================================================
typedef HRESULT (CALLBACK *DllGetClassObjectFunc)(REFCLSID clsid, REFIID iid, LPVOID *ppv);

/*
 * This linked list contains the list of registered class objects. These
 * are mostly used to register the factories for out-of-proc servers of OLE
 * objects.
 */
typedef struct tagRegisteredClass
{
  CLSID     classIdentifier;
  LPUNKNOWN classObject;
  DWORD     runContext;
  DWORD     connectFlags;
  DWORD     dwCookie;
  struct tagRegisteredClass* nextClass;
} RegisteredClass;

static RegisteredClass* firstRegisteredClass = NULL;

/*
 * COM External Lock structures and methods declaration
 *
 * This api provides a linked list to managed external references to
 * COM objects.
 *
 */

#define EL_END_OF_LIST 0
#define EL_NOT_FOUND   0

/*
 * Declaration of the static structure that manage the
 * external lock to COM  objects.
 */
typedef struct COM_ExternalLock     COM_ExternalLock;
typedef struct COM_ExternalLockList COM_ExternalLockList;

struct COM_ExternalLock
{
    IUnknown         *pUnk;     /* IUnknown referenced */
    ULONG            uRefCount; /* external lock counter to IUnknown object*/
    COM_ExternalLock *next;     /* Pointer to next element in list */
};

struct COM_ExternalLockList
{
    COM_ExternalLock *head;     /* head of list */
};

/*
 * Declaration and initialization of the static structure that manages
 * the external lock to COM objects.
 */
static COM_ExternalLockList elList = { EL_END_OF_LIST };

/*
 * Com Library reference count...
 *
 * Used to control loading / unloading of Library resources,
 * Runnng object table, DLL's etc...
 */
static LONG	COM_ref = 0;

// ======================================================================
// Prototypes.
// ======================================================================
static HRESULT COM_GetRegisteredClassObject(
	REFCLSID    	rclsid,
	DWORD       	dwClsContext,
	LPUNKNOWN * 	ppUnk);

static void COM_RevokeAllClasses();

static void COM_ExternalLockFreeList();

static void COM_ExternalLockAddRef(
	IUnknown * pUnk);

static void COM_ExternalLockRelease(
	IUnknown *	pUnk,
	BOOL		bRelAll);

static BOOL COM_ExternalLockInsert(
	IUnknown * pUnk);

static void COM_ExternalLockDelete(
	COM_ExternalLock * element);

static COM_ExternalLock * COM_ExternalLockFind(
	IUnknown *	pUnk);

static COM_ExternalLock * COM_ExternalLockLocate(
	COM_ExternalLock * element,
	IUnknown *	pUnk);

// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
// CoBuildVersion()
// ----------------------------------------------------------------------
DWORD WIN32API CoBuildVersion()
{
    dprintf(("OLE32.CoBuildVersion"));
    return (rmm << 16) + rup;
}

// ----------------------------------------------------------------------
// CoDosDateTimeToFileTime
// ----------------------------------------------------------------------
BOOL WIN32API CoDosDateTimeToFileTime(WORD nDosDate, WORD nDosTime,
                                         FILETIME *lpFileTime)
{
    dprintf(("OLE32: CoDosDateTimeToFileTime"));

    return DosDateTimeToFileTime(nDosDate, nDosTime, lpFileTime);
}

// ----------------------------------------------------------------------
// CoDosDateTimeToFileTime
// ----------------------------------------------------------------------
HRESULT WIN32API CoFileTimeNow(FILETIME *lpFileTime)
{
    SYSTEMTIME systime;

    dprintf(("OLE32: CoFileTimeNow"));

    GetSystemTime(&systime);
    return SystemTimeToFileTime(&systime, lpFileTime);
}

// ----------------------------------------------------------------------
// CoDosDateTimeToFileTime
// ----------------------------------------------------------------------
BOOL WIN32API CoFileTimeToDosDateTime(FILETIME *lpFileTime, LPWORD lpDosDate,
                                         LPWORD lpDosTime)
{
    dprintf(("OLE32: CoFileTimeToDosDateTime"));

    return FileTimeToDosDateTime(lpFileTime, lpDosDate, lpDosTime);
}

// ----------------------------------------------------------------------
// CoInitialize()
// ----------------------------------------------------------------------
HRESULT WIN32API CoInitialize(LPVOID lpReserved)
{
    dprintf(("OLE32: CoInitialize\n"));

    return CoInitializeEx(lpReserved, COINIT_APARTMENTTHREADED);
}

// ----------------------------------------------------------------------
// CoInitializeEx()
// ----------------------------------------------------------------------
HRESULT WIN32API CoInitializeEx(
	LPVOID 	lpReserved,	// [in] pointer to win32 malloc interface
	DWORD 	dwCoInit)	// [in] A value from COINIT specifies the thread
{
    HRESULT		hr;

    dprintf(("OLE32: CoInitializeEx(%p, %lx)\n", lpReserved, dwCoInit));

    if (lpReserved != NULL)
    {
	dprintf(("Warning: Bad parameter %p, must be an old Windows Application", lpReserved));
    }

    /*
     * Check for unsupported features.
     */
    if (dwCoInit != COINIT_APARTMENTTHREADED)
    {
	dprintf(("Warning: Unsupported flag %lx", dwCoInit));
	/* Hope for the best and continue anyway */
    }

    /*
     * Initialise the Running Object Table
     */
    hr = S_FALSE;
    if (++COM_ref == 1)
    {
	hr = RunningObjectTableImpl_Initialize();
	if (hr != S_OK)
	    --COM_ref;
    }

    return hr;
}

// ----------------------------------------------------------------------
// CoUninitialize()
// ----------------------------------------------------------------------
void WIN32API CoUninitialize(void)
{
    dprintf(("OLE32: CoUninitialize"));

    if (--COM_ref == 0)
    {
	dprintf(("OLE32: Releasing COM libraries"));

	RunningObjectTableImpl_UnInitialize();

	COM_RevokeAllClasses();

	CoFreeAllLibraries();
	
	COM_ExternalLockFreeList();
    }
}

// ----------------------------------------------------------------------
// CoCreateInstance
// ----------------------------------------------------------------------
HRESULT WIN32API CoCreateInstance
   (REFCLSID		rclsid,
    LPUNKNOWN		pUnkOuter,
    DWORD		dwClsContext,
    REFIID		iid,
    LPVOID *		ppv) 
{
    HRESULT hres;
    LPCLASSFACTORY lpclf = 0;

    oStringA		tCLSID(rclsid);
    oStringA		tIId(iid);

    dprintf(("OLE32: CoCreateInstance"));
    dprintf(("       CLSID:%s", (char *)tCLSID));
    dprintf(("       IID  :%s", (char *)tIId));

    // Sanity check
    if (ppv == 0)
	return E_POINTER;

    *ppv = 0;

    // Get a class factory to construct the object we want.
    hres = CoGetClassObject(rclsid, dwClsContext, NULL, &IID_IClassFactory, (LPVOID *)&lpclf);

    if (FAILED(hres))
	return hres;

    // Create the object and don't forget to release the factory
    hres = IClassFactory_CreateInstance(lpclf, pUnkOuter, iid, ppv);
    IClassFactory_Release(lpclf);

    return hres;
}

// ----------------------------------------------------------------------
// CoCreateInstanceEx
// ----------------------------------------------------------------------
HRESULT WIN32API CoCreateInstanceEx
   (REFCLSID      	rclsid, 
    LPUNKNOWN     	pUnkOuter,
    DWORD         	dwClsContext, 
    COSERVERINFO * 	pServerInfo,
    ULONG         	cmq,
    MULTI_QI *     	pResults)
{
    IUnknown * 		pUnk = NULL;
    HRESULT   		hr;
    ULONG     		index;
    int       		successCount = 0;

    oStringA		tCLSID(rclsid);

    dprintf(("OLE32: CoCreateInstanceEx"));
    dprintf(("       CLSID:%s", (char *)tCLSID));

    // Sanity check
    if ( (cmq == 0) || (pResults == NULL))
	return E_INVALIDARG;

    if (pServerInfo != NULL)
	dprintf(("OLE32: CoCreateInstanceEx - pServerInfo not supported!"));

    // Initialize all the "out" parameters.
    for (index = 0; index < cmq; index++)
    {
	pResults[index].pItf = NULL;
	pResults[index].hr   = E_NOINTERFACE;
    }

    /*
    * Get the object and get it's IUnknown pointer.
    */
    hr = CoCreateInstance(rclsid, pUnkOuter, dwClsContext, &IID_IUnknown, (VOID**)&pUnk);

    if (hr)
	return hr;

    /*
    * Then, query for all the interfaces requested.
    */
    for (index = 0; index < cmq; index++)
    {
	pResults[index].hr = IUnknown_QueryInterface(pUnk, pResults[index].pIID, (VOID**)&(pResults[index].pItf));

	if (pResults[index].hr == S_OK)
	    successCount++;
    }

    /*
    * Release our temporary unknown pointer.
    */
    IUnknown_Release(pUnk);

    if (successCount == 0)
	return E_NOINTERFACE;

    if (successCount != cmq)
	return CO_S_NOTALLINTERFACES;

    return S_OK;
}

// ----------------------------------------------------------------------
// CoGetClassObject
// ----------------------------------------------------------------------
HRESULT WIN32API CoGetClassObject
   (REFCLSID		rclsid,
    DWORD		dwClsContext,
    LPVOID		pvReserved,
    REFIID		iid,
    LPVOID *		ppv)
{
    LPUNKNOWN		regClassObject;
    HRESULT		hres = E_UNEXPECTED;

    char		dllName[MAX_PATH+1];
    LONG		dllNameLen = sizeof(dllName);
    HINSTANCE		hLibrary;

    DllGetClassObjectFunc DllGetClassObject;
    oStringA		tCLSID(rclsid);

#ifdef DEBUG
    oStringA		tIId(iid);
    dprintf(("OLE32: CoGetClassObject"));
    dprintf(("       CLSID:%s", (char *)tCLSID));
    dprintf(("       IID  :%s", (char *)tIId));
#endif

    // First, try and see if we can't match the class ID with one of the 
    // registered classes.
    if (S_OK == COM_GetRegisteredClassObject(rclsid, dwClsContext, &regClassObject))
    {
	// Get the required interface from the retrieved pointer.
	hres = IUnknown_QueryInterface(regClassObject, iid, ppv);

	// Since QI got another reference on the pointer, we want to release the
	// one we already have. If QI was unsuccessful, this will release the object. This
	// is good since we are not returning it in the "out" parameter.
	IUnknown_Release(regClassObject);

	return hres;
    }

    // out of process and remote servers not supported yet...
    if (dwClsContext & CLSCTX_LOCAL_SERVER)
    {
	dprintf(("OLE32: CoGetClassObject - CLSCTX_LOCAL_SERVER not supported!\n"));
//	return E_ACCESSDENIED;
	dwClsContext |= CLSCTX_INPROC_SERVER;	// Kludge as VB uses CLSCTX_LOCAL_SERVER
    }

    if (dwClsContext & CLSCTX_REMOTE_SERVER)
    {
	dprintf(("OLE32: CoGetClassObject - CLSCTX_REMOTE_SERVER not supported!\n"));
	return E_ACCESSDENIED;
    }

    // Get down to the biz..
    if (dwClsContext & (CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER))
    {
        HKEY	CLSIDkey;
        HKEY	key;

        /* lookup CLSID in registry key HKCR/CLSID */
        hres = RegOpenKeyExA(HKEY_CLASSES_ROOT, "CLSID", 0, KEY_READ, &CLSIDkey);
	if (hres != ERROR_SUCCESS)
	    return REGDB_E_READREGDB;

        hres = RegOpenKeyExA(CLSIDkey, tCLSID, 0, KEY_QUERY_VALUE, &key);
	if (hres != ERROR_SUCCESS) 
	{
	    RegCloseKey(CLSIDkey);
	    return REGDB_E_CLASSNOTREG;
	}

	hres = RegQueryValueA(key, "InprocServer32", dllName, &dllNameLen);
	RegCloseKey(key);
	RegCloseKey(CLSIDkey);
	if (hres != ERROR_SUCCESS)
	{
	    dprintf(("OLE32: CoGetClassObject - InprocServer32 not found in registry"));
	    return REGDB_E_READREGDB;
	}

	dprintf(("OLE32: CoGetClassObject - Registry reports InprocServer32 dll as %s\n", dllName));

	/* open dll, call DllGetClassFactory */
	hLibrary = CoLoadLibrary(dllName, TRUE);
	if (hLibrary == 0)
	{
	    dprintf(("OLE32: CoGetClassObject - couldn't load %s\n", dllName));
	    return E_ACCESSDENIED; /* or should this be CO_E_DLLNOTFOUND? */
	}

	DllGetClassObject = (DllGetClassObjectFunc)GetProcAddress(hLibrary, "DllGetClassObject");
	if (!DllGetClassObject) 
	{
	    dprintf(("OLE32: CoGetClassObject - couldn't function DllGetClassObject in %s\n", dllName));
	    /* not sure if this should be called here CoFreeLibrary(hLibrary);*/
	    return E_ACCESSDENIED;
	}

	// Ask the DLL for it's class object. (there was a note here about class
	// factories but this is good.
	return DllGetClassObject(rclsid, iid, ppv);
    }
    return hres;
}

// ----------------------------------------------------------------------
// CoLockObjectExternal
// ----------------------------------------------------------------------
HRESULT WIN32API CoLockObjectExternal(IUnknown *pUnk, BOOL fLock, BOOL fLastUnlockReleases)
{
    dprintf(("OLE32: CoLockObjectExternal"));

    if (fLock) 
    {
	/* 
	 * Increment the external lock coutner, COM_ExternalLockAddRef also
	 * increment the object's internal lock counter.
	 */
	COM_ExternalLockAddRef( pUnk); 
    }
    else
    {
	/* 
	 * Decrement the external lock coutner, COM_ExternalLockRelease also
	 * decrement the object's internal lock counter.
	 */
	COM_ExternalLockRelease( pUnk, fLastUnlockReleases);
    }

    return S_OK;
}

// ----------------------------------------------------------------------
// CoRegisterClassObject
// ----------------------------------------------------------------------
HRESULT WIN32API CoRegisterClassObject(
    REFCLSID		rclsid,
    LPUNKNOWN		pUnk,
    DWORD		dwClsContext, 	// Context in which to run the executable 
    DWORD		flags,        	// how connections are made 
    LPDWORD		lpdwRegister) 
{
    RegisteredClass *	newClass;
    LPUNKNOWN       	foundObject;
    HRESULT         	hr;
    oStringA		tClsid(rclsid);

    dprintf(("OLE32: CoRegisterClassObject(%s)", (char *)tClsid));

    // Perform a sanity check on the parameters
    if ((lpdwRegister == 0) || (pUnk == 0))
	return E_INVALIDARG;

    // Initialize the cookie (out parameter)
    *lpdwRegister = 0;

    // First, check if the class is already registered.
    // If it is, this should cause an error.
    hr = COM_GetRegisteredClassObject(rclsid, dwClsContext, &foundObject);
    if (hr == S_OK)
    {
	// The COM_GetRegisteredClassObject increased the reference count on the
	// object so it has to be released.
	IUnknown_Release(foundObject);

	return CO_E_OBJISREG;
    }

    // If it is not registered, we must create a new entry for this class and
    // append it to the registered class list.
    // We use the address of the chain node as the cookie since we are sure it's
    // unique.
    newClass = (RegisteredClass *)HeapAlloc(GetProcessHeap(), 0, sizeof(RegisteredClass));

    // Initialize the node.
    newClass->classIdentifier = *rclsid;
    newClass->runContext      = dwClsContext;
    newClass->connectFlags    = flags;
    newClass->dwCookie        = (DWORD)newClass;
    newClass->nextClass       = firstRegisteredClass;

    // Since we're making a copy of the object pointer, we have to increase it's
    // reference count.
    newClass->classObject     = pUnk;
    IUnknown_AddRef(newClass->classObject);

    firstRegisteredClass = newClass;

    // Assign the out parameter (cookie)
    *lpdwRegister = newClass->dwCookie;

    return S_OK;
}

// ----------------------------------------------------------------------
// CoRevokeClassObject
//
// This method will remove a class object from the class registry
// ----------------------------------------------------------------------
HRESULT WIN32API CoRevokeClassObject(DWORD dwRegister) 
{
    RegisteredClass * *	prevClassLink;
    RegisteredClass *	curClass;

    dprintf(("OLE32: CoRevokeClassObject"));

    // Iterate through the whole list and try to match the cookie.
    curClass      = firstRegisteredClass;
    prevClassLink = &firstRegisteredClass;

    while (curClass != 0)
    {
	// Check if we have a match on the cookie.
	if (curClass->dwCookie == dwRegister)
	{
	    // Remove the class from the chain.
	    *prevClassLink = curClass->nextClass;

	    // Release the reference to the class object.
	    IUnknown_Release(curClass->classObject);

	    // Free the memory used by the chain node.
	    HeapFree(GetProcessHeap(), 0, curClass);

	    return S_OK;
	}

	// Step to the next class in the list.
	prevClassLink = &(curClass->nextClass);
	curClass      = curClass->nextClass;
    }

    return E_INVALIDARG;
}

// ----------------------------------------------------------------------
// GetClassFile
//
// This function supplies the CLSID associated with the given filename.
// ----------------------------------------------------------------------
HRESULT WIN32API GetClassFile(LPOLESTR filePathName, CLSID *pclsid)
{
    IStorage *	pstg = 0;
    HRESULT 	res;
    int 	nbElm = 0;
    int		length = 0;
    int		i = 0;
    LONG 	sizeProgId = 20;
    LPOLESTR *	pathDec = 0;
    LPOLESTR	absFile = 0;
    LPOLESTR	progId = 0;
    WCHAR 	extention[100] = {0};

    dprintf(("OLE32: GetClassFile"));

    // if the file contain a storage object the return the CLSID
    // writen by IStorage_SetClass method

    if((StgIsStorageFile(filePathName)) == S_OK)
    {
	res = StgOpenStorage(filePathName, NULL, STGM_READ | STGM_SHARE_DENY_WRITE, NULL, 0, &pstg);

	if (SUCCEEDED(res))
	    res = ReadClassStg(pstg, pclsid);

	IStorage_Release(pstg);

	return res;
    }

    /* if the obove strategies fail then search for the extension key in the registry */

    /* get the last element (absolute file) in the path name */
    nbElm = FileMonikerImpl_DecomposePath(filePathName, &pathDec);
    absFile = pathDec[nbElm-1];

    /* failed if the path represente a directory and not an absolute file name*/
    if (lstrcmpW(absFile, (LPOLESTR)"\\"))
	return MK_E_INVALIDEXTENSION;

    /* get the extension of the file */
    length = lstrlenW(absFile);
    for(i = length-1; ( (i >= 0) && (extention[i] = absFile[i]) ); i--);
	
    /* get the progId associated to the extension */
    progId = (WCHAR *)CoTaskMemAlloc(sizeProgId);

    res = RegQueryValueW(HKEY_CLASSES_ROOT, extention, progId, &sizeProgId);

    if (res == ERROR_MORE_DATA)
    {
	CoTaskMemRealloc(progId,sizeProgId);

	res = RegQueryValueW(HKEY_CLASSES_ROOT, extention, progId, &sizeProgId);
    }
    if (res == ERROR_SUCCESS)
	/* return the clsid associated to the progId */
	res =  CLSIDFromProgID(progId, pclsid);

    for(i = 0; pathDec[i] != NULL; i++)
	CoTaskMemFree(pathDec[i]);

    CoTaskMemFree(pathDec);

    CoTaskMemFree(progId);

    if (res == ERROR_SUCCESS)
	return res;

    return MK_E_INVALIDEXTENSION;
}
// ======================================================================
// Private functions.
// ======================================================================

// ----------------------------------------------------------------------
// COM_GetRegisteredClassObject
// ----------------------------------------------------------------------
// This internal method is used to scan the registered class list to 
// find a class object.
//
// Params: 
//   rclsid        Class ID of the class to find.
//   dwClsContext  Class context to match.
//   ppv           [out] returns a pointer to the class object. Complying
//                 to normal COM usage, this method will increase the
//                 reference count on this object.
static HRESULT COM_GetRegisteredClassObject
   (REFCLSID		rclsid,
    DWORD       	dwClsContext,
    LPUNKNOWN *	  	ppUnk)
{
    RegisteredClass* curClass;

    oStringA		tCLSID(rclsid);

    dprintf(("OLE32: COM_GetRegisteredClassObject"));
    dprintf(("       CLSID:%s", (char *)tCLSID));

    // Sanity check
    if (ppUnk == 0)
	return E_POINTER;

    // Iterate through the whole list and try to match the class ID.
    curClass = firstRegisteredClass;

    while (curClass != 0)
    {
	// Check if we have a match on the class ID.
	if (IsEqualGUID(&curClass->classIdentifier, rclsid))
	{
	    // Since we don't do out-of process or DCOM just right away,
	    // let's ignore the class context.

	    // We have a match, return the pointer to the class object.
	    *ppUnk = curClass->classObject;

	    IUnknown_AddRef(curClass->classObject);

	    return S_OK;
	}

    // Step to the next class in the list.
    curClass = curClass->nextClass;
    }

    // If we get to here, we haven't found our class.
    return S_FALSE;
}

// ----------------------------------------------------------------------
// COM_RevokeAllClasses
// ----------------------------------------------------------------------
// This method is called when the COM libraries are uninitialized to 
// release all the references to the class objects registered with
// the library
static void COM_RevokeAllClasses()
{
    dprintf(("OLE32: COM_RevokeAllClasses"));

    while (firstRegisteredClass != 0)
    {
	CoRevokeClassObject(firstRegisteredClass->dwCookie);
    }
}

// ----------------------------------------------------------------------
// COM_ExternalLockAddRef
// ----------------------------------------------------------------------
// Method that increments the count for a IUnknown* in the linked 
// list.  The item is inserted if not already in the list.
static void COM_ExternalLockAddRef(IUnknown * pUnk)
{
    dprintf(("OLE32: COM_ExternalLockAddRef"));

    COM_ExternalLock *externalLock = COM_ExternalLockFind(pUnk);

    /*
     * Add an external lock to the object. If it was already externally
     * locked, just increase the reference count. If it was not.
     * add the item to the list.
     */
    if ( externalLock == EL_NOT_FOUND )
	COM_ExternalLockInsert(pUnk);
    else
	externalLock->uRefCount++;

    /*
     * Add an internal lock to the object
     */
    IUnknown_AddRef(pUnk); 
}

// ----------------------------------------------------------------------
// COM_ExternalLockRelease
// ----------------------------------------------------------------------
// Method that decrements the count for a IUnknown* in the linked 
// list.  The item is removed from the list if its count end up at zero or if
// bRelAll is TRUE.
static void COM_ExternalLockRelease(IUnknown * pUnk, BOOL bRelAll)
{
    dprintf(("OLE32: COM_ExternalLockRelease"));

    COM_ExternalLock * externalLock = COM_ExternalLockFind(pUnk);

    if ( externalLock != EL_NOT_FOUND )
    {
	do
	{
	    externalLock->uRefCount--;  /* release external locks      */
	    IUnknown_Release(pUnk);     /* release local locks as well */

	    if ( bRelAll == FALSE ) 
		break;  /* perform single release */

	} while ( externalLock->uRefCount > 0 );  

	if ( externalLock->uRefCount == 0 )  /* get rid of the list entry */
	    COM_ExternalLockDelete(externalLock);
    }
}

// ----------------------------------------------------------------------
// COM_ExternalLockFreeList
// ----------------------------------------------------------------------
// Method that frees the content of the list.
static void COM_ExternalLockFreeList()
{
    dprintf(("OLE32: COM_ExternalLockFreeList"));

    COM_ExternalLock *head;

    head = elList.head;                 /* grab it by the head             */
    while (head != EL_END_OF_LIST)
    {
	COM_ExternalLockDelete(head);     /* get rid of the head stuff       */

	head = elList.head;               /* get the new head...             */ 
    }
}

// ----------------------------------------------------------------------
// COM_ExternalLockDump
// ----------------------------------------------------------------------
// Method that dump the content of the list.
void COM_ExternalLockDump()
{
    dprintf(("OLE32: COM_ExternalLockDump"));

    COM_ExternalLock *current = elList.head;

    printf("External lock list:");

    while ( current != EL_END_OF_LIST )
    {
	dprintf(("    %p with %lu references count.\n", current->pUnk, current->uRefCount));

	/* Skip to the next item */ 
	current = current->next;
    } 
}

// ----------------------------------------------------------------------
// COM_ExternalLockFind
// ----------------------------------------------------------------------
// Find a IUnknown* in the linked list
static COM_ExternalLock * COM_ExternalLockFind(IUnknown *pUnk)
{
    dprintf(("OLE32: COM_ExternalLockFind"));

    return COM_ExternalLockLocate(elList.head, pUnk);
}

// ----------------------------------------------------------------------
// COM_ExternalLockLocate
// ----------------------------------------------------------------------
// Recursivity agent for IUnknownExternalLockList_Find
static COM_ExternalLock * COM_ExternalLockLocate( COM_ExternalLock * element, IUnknown * pUnk)
{
  if ( element == EL_END_OF_LIST )  
    return EL_NOT_FOUND;

  else if ( element->pUnk == pUnk )    /* We found it */
    return element;

  else                                 /* Not the right guy, keep on looking */ 
    return COM_ExternalLockLocate( element->next, pUnk);
}

// ----------------------------------------------------------------------
// COM_ExternalLockInsert
// ----------------------------------------------------------------------
// Insert a new IUnknown* to the linked list
static BOOL COM_ExternalLockInsert(IUnknown * pUnk)
{
    dprintf(("OLE32: COM_ExternalLockInsert"));

    COM_ExternalLock *	newLock      = NULL;
    COM_ExternalLock *	previousHead = NULL;

    // Allocate space for the new storage object
    newLock = (COM_ExternalLock *)HeapAlloc(GetProcessHeap(), 0, sizeof(COM_ExternalLock));

    if (newLock != NULL)
    {
	if ( elList.head == EL_END_OF_LIST ) 
	    elList.head = newLock;    /* The list is empty */
	else 
	{
	    // insert does it at the head
	    previousHead  = elList.head;
	    elList.head = newLock;
	}

	/*
	 * Set new list item data member 
	 */
	newLock->pUnk      = pUnk;
	newLock->uRefCount = 1;
	newLock->next      = previousHead;

	return TRUE;
    }

    return FALSE;
}

// ----------------------------------------------------------------------
// ExternalLockDelete
// ----------------------------------------------------------------------
// Method that removes an item from the linked list.
static void COM_ExternalLockDelete(COM_ExternalLock * itemList)
{
    dprintf(("OLE32: ExternalLockDelete"));

    COM_ExternalLock *current = elList.head;

    if ( current == itemList )
    {
	// this section handles the deletion of the first node 
	elList.head = itemList->next;
	HeapFree( GetProcessHeap(), 0, itemList);  
    }
    else
    {
	do 
	{
	    if ( current->next == itemList )   /* We found the item to free  */
	    {
		current->next = itemList->next;  /* readjust the list pointers */

		HeapFree( GetProcessHeap(), 0, itemList);  
		break; 
	    }

	    /* Skip to the next item */ 
	    current = current->next;

	} while ( current != EL_END_OF_LIST );
    }
}
