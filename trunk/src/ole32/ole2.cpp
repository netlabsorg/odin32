/* $Id: ole2.cpp,v 1.4 2001-02-27 22:25:54 hugh Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * OLE functions.
 *
 * 2/9/99
 *
 * Copyright 1999 David J. Raison
 *
 * Some portions from Wine Implementation (2/9/99)
 *   Copyright 1995  Martin von Loewis
 *   Copyright 1999  Francis Beaudet
 *   Copyright 1999  Noel Borthwick
 */

#include "ole32.h"
#include "commctrl.h"
#include "oString.h"
#include "heapstring.h"
#include <assert.h>

// ======================================================================
// Local Data
// ======================================================================

/*
 * This is the lock count on the OLE library. It is controlled by the
 * OLEInitialize/OLEUninitialize methods.
 */
static ULONG OLE_moduleLockCount = 0;


// ======================================================================
// Prototypes.
// ======================================================================
static void OLEUTL_ReadRegistryDWORDValue(HKEY regKey, DWORD* pdwValue);

// These are the prototypes of the utility methods used to manage a shared menu
extern  void  OLEMenu_Initialize();
extern  void  OLEMenu_UnInitialize();

// These are the prototypes of the OLE Clipboard initialization methods (in clipboard.c)
extern  void  OLEClipbrd_UnInitialize();
extern  void  OLEClipbrd_Initialize();

// These are the prototypes of the utility methods used for OLE Drag n Drop
extern  void  OLEDD_Initialize();
extern  void  OLEDD_UnInitialize();

// ======================================================================
// Public API's
// ======================================================================

// ----------------------------------------------------------------------
// OleBuildVersion()
// ----------------------------------------------------------------------
DWORD WIN32API OleBuildVersion(void)
{
    dprintf(("OLE32.OleBuildVersion"));
    return (rmm<<16)+rup;
}

// ----------------------------------------------------------------------
// OleInitialize()
// ----------------------------------------------------------------------
HRESULT WIN32API OleInitialize(LPVOID reserved)
{
    HRESULT hr;

    dprintf(("OLE32: OleInitialize"));

    // The first duty of the OleInitialize is to initialize the COM libraries.
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
  return hr;

    // Then, it has to initialize the OLE specific modules.
    // This includes:
    //     Clipboard
    //     Drag and Drop
    //     Object linking and Embedding
    //     In-place activation
    if (++OLE_moduleLockCount == 1)
    {
  dprintf(("    First time through - Initializing"));

  // OLE Clipboard
  OLEClipbrd_Initialize();

  // Drag and Drop
  OLEDD_Initialize();

  // OLE shared menu
  OLEMenu_Initialize();
    }

    return hr;
}

// ----------------------------------------------------------------------
// CoGetCurrentProcess()
// ----------------------------------------------------------------------
DWORD WIN32API CoGetCurrentProcess()
{
    dprintf(("OLE32: CoGetCurrentProcess"));
    return GetCurrentProcessId();
}

// ----------------------------------------------------------------------
// OleUninitialize()
// ----------------------------------------------------------------------
void WIN32API OleUninitialize(void)
{
    dprintf(("OLE32: OleUninitialize"));

    // If we hit the bottom of the lock stack, free the libraries.
    if (--OLE_moduleLockCount == 0)
    {
  dprintf(("    no more references - Terminating"));

  // OLE Clipboard
  OLEClipbrd_UnInitialize();

  // Drag and Drop
  OLEDD_UnInitialize();

  // OLE shared menu
  OLEMenu_UnInitialize();
    }

    // Then, uninitialize the COM libraries.
    CoUninitialize();
}

// ----------------------------------------------------------------------
// OleRegGetUserType()
// ----------------------------------------------------------------------
// This implementation of OleRegGetUserType ignores the dwFormOfType
// parameter and always returns the full name of the object. This is
// not too bad since this is the case for many objects because of the
// way they are registered.
// ----------------------------------------------------------------------
HRESULT WIN32API OleRegGetUserType
   (REFCLSID    clsid,
    DWORD   dwFormOfType,
    LPOLESTR *    pszUserType)

{
    oStringA    tCLSID;
    DWORD   dwKeyType;
    DWORD   cbData;
    HKEY    clsidKey;
    LONG    hres;
    LPSTR     buffer;

    dprintf(("OLE32: OleRegGetUserType"));

    *pszUserType = NULL;

    // Build the key name we're looking for
    tCLSID = "CLSID\\";
    tCLSID += clsid;
    tCLSID += "}";

    // Open the class id Key
    hres = RegOpenKeyA(HKEY_CLASSES_ROOT, tCLSID, &clsidKey);
    if (hres != ERROR_SUCCESS)
  return REGDB_E_CLASSNOTREG;

    // Retrieve the size of the name string.
    cbData = 0;
    hres = RegQueryValueExA(clsidKey, "", NULL, &dwKeyType, NULL, &cbData);
    if (hres != ERROR_SUCCESS)
    {
  RegCloseKey(clsidKey);
  return REGDB_E_READREGDB;
    }

    // Allocate a buffer for the registry value.
    buffer = (LPSTR)HeapAlloc(GetProcessHeap(), 0, cbData);

    if (buffer == NULL)
    {
  RegCloseKey(clsidKey);
  return E_OUTOFMEMORY;
    }

    hres = RegQueryValueExA(HKEY_CLASSES_ROOT, "", NULL, &dwKeyType, (LPBYTE)buffer, &cbData);
    RegCloseKey(clsidKey);
    if (hres != ERROR_SUCCESS)
    {
  HeapFree(GetProcessHeap(), 0, buffer);
  return REGDB_E_READREGDB;
    }

    // Allocate a buffer for the return value.
    *pszUserType = (LPOLESTR)CoTaskMemAlloc(cbData * 2);

    if (*pszUserType == NULL)
    {
  HeapFree(GetProcessHeap(), 0, buffer);
  return E_OUTOFMEMORY;
    }

    // Copy & convert to unicode...
    lstrcpyAtoW(*pszUserType, buffer);
    HeapFree(GetProcessHeap(), 0, buffer);

    return S_OK;
}

// ----------------------------------------------------------------------
// OleRegGetMiscStatus()
// ----------------------------------------------------------------------
HRESULT WIN32API OleRegGetMiscStatus
   (REFCLSID    clsid,
    DWORD   dwAspect,
    DWORD *   pdwStatus)
{
    oStringA    tStr;
    HKEY        clsidKey;
    HKEY        miscStatusKey;
    HKEY        aspectKey;
    LONG        result;

    dprintf(("OLE32: OleRegGetMiscStatus"));

    // Initialize the out parameter.
    *pdwStatus = 0;

    // Build the key name we're looking for
    tStr = "CLSID\\";
    tStr += clsid;
    tStr += "}";
    dprintf(("       Key   : %s", (char *)tStr));

    // Open the class id Key
    result = RegOpenKeyA(HKEY_CLASSES_ROOT, tStr, &clsidKey);

    if (result != ERROR_SUCCESS)
  return REGDB_E_CLASSNOTREG;

    // Get the MiscStatus
    result = RegOpenKeyA(clsidKey, "MiscStatus", &miscStatusKey);

    if (result != ERROR_SUCCESS)
    {
  RegCloseKey(clsidKey);
  return REGDB_E_READREGDB;
    }

    // Read the default value
    OLEUTL_ReadRegistryDWORDValue(miscStatusKey, pdwStatus);

    // Open the key specific to the requested aspect.
    oStringA    tKey(dwAspect);
    dprintf(("       Aspect: %s", (char *)tKey));

    result = RegOpenKeyA(miscStatusKey, tKey, &aspectKey);

    if (result == ERROR_SUCCESS)
    {
  OLEUTL_ReadRegistryDWORDValue(aspectKey, pdwStatus);
  RegCloseKey(aspectKey);
    }

    // Cleanup
    RegCloseKey(miscStatusKey);
    RegCloseKey(clsidKey);

    return S_OK;
}

// ----------------------------------------------------------------------
// OleSetContainedObject()
// ----------------------------------------------------------------------
HRESULT WIN32API OleSetContainedObject
   (LPUNKNOWN   pUnknown,
    BOOL          fContained)
{
    IRunnableObject * runnable = NULL;
    HRESULT           hres;

    dprintf(("OLE32: OleSetContainedObject"));

    hres = IUnknown_QueryInterface(pUnknown, &IID_IRunnableObject, (void**)&runnable);

    if (SUCCEEDED(hres))
    {
  hres = IRunnableObject_SetContainedObject(runnable, fContained);

  IRunnableObject_Release(runnable);

  return hres;
    }

    return S_OK;
}

// ----------------------------------------------------------------------
// OleLoad()
// ----------------------------------------------------------------------
HRESULT WIN32API OleLoad
   (LPSTORAGE   pStg,
    REFIID            riid,
    LPOLECLIENTSITE   pClientSite,
    LPVOID *          ppvObj)
{
    IPersistStorage * persistStorage = NULL;
    IOleObject *        oleObject      = NULL;
    STATSTG           storageInfo;
    HRESULT           hres;

    dprintf(("OLE32: OleLoad"));

    // TODO, Conversion ... OleDoAutoConvert

    // Get the class ID for the object.
    hres = IStorage_Stat(pStg, &storageInfo, STATFLAG_NONAME);

    // Now, try and create the handler for the object
    hres = CoCreateInstance(&storageInfo.clsid,
              NULL,
              CLSCTX_INPROC_HANDLER,
              &IID_IOleObject,
              (void**)&oleObject);

    // If that fails, as it will most times, load the default OLE handler.
    if (FAILED(hres))
    {
  hres = OleCreateDefaultHandler(&storageInfo.clsid,
               NULL,
               &IID_IOleObject,
               (void**)&oleObject);
    }

    // If we couldn't find a handler... this is bad. Abort the whole thing.
    if (FAILED(hres))
  return hres;

    // Inform the new object of it's client site.
    hres = IOleObject_SetClientSite(oleObject, pClientSite);

    // Initialize the object with it's IPersistStorage interface.
    hres = IOleObject_QueryInterface(oleObject, &IID_IPersistStorage, (void**)&persistStorage);

    if (SUCCEEDED(hres))
    {
  IPersistStorage_Load(persistStorage, pStg);

  IPersistStorage_Release(persistStorage);
  persistStorage = NULL;
    }

    // Return the requested interface to the caller.
    hres = IOleObject_QueryInterface(oleObject, riid, ppvObj);

    // Cleanup interfaces used internally
    IOleObject_Release(oleObject);

    return hres;
}

// ----------------------------------------------------------------------
// OleSave()
// ----------------------------------------------------------------------
HRESULT WIN32API OleSave
   (LPPERSISTSTORAGE    pPS,
    LPSTORAGE           pStg,
    BOOL                fSameAsLoad)
{
    HRESULT     hres;
    CLSID     objectClass;

    dprintf(("OLE32: OleSave"));

    // First, we transfer the class ID (if available)
    hres = IPersistStorage_GetClassID(pPS, &objectClass);

    if (SUCCEEDED(hres))
  WriteClassStg(pStg, &objectClass);

    // Then, we ask the object to save itself to the
    // storage. If it is successful, we commit the storage.
    hres = IPersistStorage_Save(pPS, pStg, fSameAsLoad);

    if (SUCCEEDED(hres))
  IStorage_Commit(pStg, STGC_DEFAULT);

    return hres;
}

// ----------------------------------------------------------------------
// ReleaseStgMedium()
// ----------------------------------------------------------------------
void WIN32API ReleaseStgMedium(STGMEDIUM * pmedium)
{
    switch (pmedium->tymed)
    {
  case TYMED_HGLOBAL:
  {
      if ( (pmedium->pUnkForRelease == 0) && (pmedium->u.hGlobal  != 0) )
    GlobalFree(pmedium->u.hGlobal);

      pmedium->u.hGlobal = 0;
      break;
  }
  case TYMED_FILE:
  {
      if (pmedium->u.lpszFileName != 0)
      {
    if (pmedium->pUnkForRelease == 0)
        DeleteFileW(pmedium->u.lpszFileName);

    CoTaskMemFree(pmedium->u.lpszFileName);
      }

      pmedium->u.lpszFileName = 0;
      break;
  }
  case TYMED_ISTREAM:
  {
      if (pmedium->u.pstm != 0)
    IStream_Release(pmedium->u.pstm);

      pmedium->u.pstm = 0;
      break;
  }
  case TYMED_ISTORAGE:
  {
      if (pmedium->u.pstg != 0)
    IStorage_Release(pmedium->u.pstg);

      pmedium->u.pstg = 0;
      break;
  }
  case TYMED_GDI:
  {
      if ( (pmedium->pUnkForRelease == 0) && (pmedium->u.hGlobal != 0) )
    DeleteObject(pmedium->u.hGlobal);

      pmedium->u.hGlobal = 0;
      break;
  }
  case TYMED_MFPICT:
  {
      if ( (pmedium->pUnkForRelease == 0) && (pmedium->u.hMetaFilePict != 0) )
      {
    DeleteMetaFile(pmedium->u.hMetaFilePict);
    GlobalFree(pmedium->u.hMetaFilePict);
      }

      pmedium->u.hMetaFilePict = 0;
      break;
  }
  case TYMED_ENHMF:
  {
      if ( (pmedium->pUnkForRelease == 0) && (pmedium->u.hEnhMetaFile != 0) )
    DeleteEnhMetaFile(pmedium->u.hEnhMetaFile);

      pmedium->u.hEnhMetaFile = 0;
      break;
  }
  case TYMED_NULL:
      // Do nothing
      break;

  default:
      break;
    }

    // After cleaning up, the unknown is released
    if (pmedium->pUnkForRelease != 0)
    {
  IUnknown_Release(pmedium->pUnkForRelease);
  pmedium->pUnkForRelease = 0;
    }
}

// ======================================================================
// Private functions.
// ======================================================================

/***
 * OLEUTL_ReadRegistryDWORDValue
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

  if (lres == ERROR_SUCCESS)
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

