
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * WINSPOOL implemention
 *
 * Copyright 1998 Patrick Haller
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2003-2004 InnoTek Systemberatung GmbH
 *
 *
 * Partially based on WineX code (dlls\winspool\info.c (EnumPrinters, DeviceCapabilitiesA/W)
 *
 * Copyright 1996 John Harvey
 * Copyright 1998 Andreas Mohr
 * Copyright 1999 Klaas van Gend
 * Copyright 1999, 2000 Huw D M Davies
 *
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <windows.h>
#include <os2win.h>
#include <winspool.h>
#include <odinwrap.h>
#include <heapstring.h>
#include <win/winnls.h>
#include <win/debugstr.h>
#include <win/debugtools.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <misc.h>
#ifdef __WIN32OS2__
# include <vmutex.h>
# include <unicode.h>
#endif
#include "oslibspl.h"


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#ifdef __WIN32OS2__
/**
 * Debug assertion macro.
 * @param   expr    Assert that this expression is true.
 * @param   msg     Message to print if expr isn't true. It's given to dprintf,
 *                  and must be inclosed in paratheses.
 * @todo move this to some header in /include.
 */
#ifdef DEBUG
#define DebugAssert(expr, msg) \
    do { if (expr) break; \
         dprintf(("!!!ASSERTION FAILED!!!\nFILE=%s\nLINE=%d\nFUNCTION=%s\n", __FILE__, __LINE__, __FUNCTION__)); \
         dprintf(msg); DebugInt3(); \
       } while (0)
#else
#define DebugAssert(expr, msg) do {} while (0)
#endif

/**
 * Debug assertion failed macro.
 * @param   msg     Message to print if expr isn't true. It's given to dprintf,
 *                  and must be inclosed in paratheses.
 * @todo move this to some header in /include.
 */
#ifdef DEBUG
#define DebugAssertFailed(msg) \
    do { dprintf(("!!!ASSERTION FAILED!!!\nFILE=%s\nLINE=%d\nFUNCTION=%s\n", __FILE__, __LINE__, __FUNCTION__)); \
         dprintf(msg); DebugInt3(); \
       } while (0)
#else
#define DebugAssertFailed(msg) do {} while (0)
#endif

#endif

/** Define this to use the OS/2 printer driver names. */
#define USE_OS2_DRIVERNAME

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
#ifdef __WIN32OS2__
/**
 * Open printer instance.
 * @todo protect the structure while using it. (probably not an issue)
 */
typedef struct _OpenPrinter
{
    /** The handle of this instance. */
    HANDLE      hOpenPrinter;
    /** Pointer to the next entry. */
    struct _OpenPrinter *pNext;
    /** Printer name. */
    LPWSTR      pwszPrinterName;

    /** Open spool file. */
    HANDLE      hspl;
    /** Current page number in the job (for StartPagePrinter()). */
    ULONG       ulCurPage;
} OPENPRINTER, *POPENPRINTER;
#endif


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifdef __WIN32OS2__
/** LIFO of open printers. */
POPENPRINTER    gpOpenPrinters;
/** Last Printer handle.
 * This assumes that 4G open printers are enough and that the caller accepts high
 * values. */
HANDLE          ghOpenPrintersLast;
/** Mutex protecting gpOpenPrinters and ghOpenPrintersLast.
  * This protects only the list, not the entries themself. */
VMutex          gOpenPrintersMutex;
#endif


static char Printers[] =
"System\\CurrentControlSet\\control\\Print\\Printers\\";
static char Drivers[] =
"System\\CurrentControlSet\\control\\Print\\Environments\\%s\\Drivers\\";

#ifndef __WIN32OS2__
static LPWSTR *printer_array;
static int nb_printers;
#endif
static WCHAR DefaultEnvironmentW[] = {'W','i','n','e',0};

static WCHAR Configuration_FileW[] = {'C','o','n','f','i','g','u','r','a','t',
				      'i','o','n',' ','F','i','l','e',0};
static WCHAR DatatypeW[] = {'D','a','t','a','t','y','p','e',0};
static WCHAR Data_FileW[] = {'D','a','t','a',' ','F','i','l','e',0};
static WCHAR Default_DevModeW[] = {'D','e','f','a','u','l','t',' ','D','e','v',
				   'M','o','d','e',0};
static WCHAR Dependent_FilesW[] = {'D','e','p','e','n','d','e','n','t',' ','F',
				   'i','l','e','s',0};
static WCHAR DescriptionW[] = {'D','e','s','c','r','i','p','t','i','o','n',0};
static WCHAR DriverW[] = {'D','r','i','v','e','r',0};
static WCHAR Help_FileW[] = {'H','e','l','p',' ','F','i','l','e',0};
static WCHAR LocationW[] = {'L','o','c','a','t','i','o','n',0};
static WCHAR MonitorW[] = {'M','o','n','i','t','o','r',0};
static WCHAR NameW[] = {'N','a','m','e',0};
static WCHAR ParametersW[] = {'P','a','r','a','m','e','t','e','r','s',0};
static WCHAR PortW[] = {'P','o','r','t',0};
static WCHAR Print_ProcessorW[] = {'P','r','i','n','t',' ','P','r','o','c','e',
				   's','s','o','r',0};
static WCHAR Printer_DriverW[] = {'P','r','i','n','t','e','r',' ','D','r','i',
				  'v','e','r',0};
static WCHAR PrinterDriverDataW[] = {'P','r','i','n','t','e','r','D','r','i',
				     'v','e','r','D','a','t','a',0};
static WCHAR Separator_FileW[] = {'S','e','p','a','r','a','t','o','r',' ','F',
				  'i','l','e',0};
static WCHAR Share_NameW[] = {'S','h','a','r','e',' ','N','a','m','e',0};
static WCHAR WinPrintW[] = {'W','i','n','P','r','i','n','t',0};

ODINDEBUGCHANNEL(WINSPOOL)

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#ifdef __WIN32OS2__
BOOL WIN32API SplQueryPMQueueFromHandle(HANDLE hPrinter, char *pszQueue, unsigned cchQueue);
BOOL WIN32API SplQueryPMQueueName(LPSTR pszPrinterName, LPSTR pszQueue, INT cchQueue);
static POPENPRINTER openprinterNew(LPCWSTR pwszPrinterName);
static POPENPRINTER openprinterFind(HANDLE hHandle);
static void         openprinterDelete(POPENPRINTER pOpenPrinter);
static int          openprinterOpenSpoolFile(POPENPRINTER pOpenPrinter);


/**
 * Allocates, copies the data from pOpenPrinter into the new node and
 * inserts it into the list.
 *
 * @returns Pointer to the inserted node.
 * @returns NULL on failure (i.e. out of memory).
 * @param   pwszPrinterName     Pointer to printer name. (Duplicated)
 *
 */
static POPENPRINTER openprinterNew(LPCWSTR pwszPrinterName)
{
    POPENPRINTER    pNew = (POPENPRINTER)malloc(sizeof(*pNew));
    if (pNew)
    {
        memset(pNew, 0, sizeof(*pNew));
        pNew->pwszPrinterName = HEAP_strdupW(GetProcessHeap(), 0, pwszPrinterName);
        if (pNew->pwszPrinterName)
        {
            gOpenPrintersMutex.enter();
            pNew->hOpenPrinter = ++ghOpenPrintersLast;
            pNew->pNext = gpOpenPrinters;
            gpOpenPrinters = pNew;
            gOpenPrintersMutex.leave();
        }
        else
        {
            free(pNew);
            pNew = NULL;
        }
    }
    return pNew;
}


/**
 * Finds the data associated with an open printer handle.
 *
 * @returns Pointer to the data associated with the handle.
 * @param   hHandle     The handle of the open printer.
 */
static POPENPRINTER openprinterFind(HANDLE hHandle)
{
    POPENPRINTER pOpenPrinter;

    gOpenPrintersMutex.enter();
    for (pOpenPrinter = gpOpenPrinters; pOpenPrinter; pOpenPrinter = pOpenPrinter->pNext)
        if (pOpenPrinter->hOpenPrinter == hHandle)
            break;
    gOpenPrintersMutex.leave();
    return pOpenPrinter;
}


/**
 * Unlinks and frees an open printer node.
 * The caller must release any other resources associated with it.
 *
 * @param   pOpenPrinter    Pointer to the node to delete.
 */
static void         openprinterDelete(POPENPRINTER pOpenPrinter)
{
    /*
     * Unlink it.
     */
    gOpenPrintersMutex.enter();
    if (gpOpenPrinters == pOpenPrinter)
        gpOpenPrinters = gpOpenPrinters->pNext;
    else
    {
        POPENPRINTER p;
        for (p = gpOpenPrinters; p->pNext; p = pOpenPrinter->pNext)
        {
            if (p->pNext == pOpenPrinter)
            {
                p->pNext = pOpenPrinter->pNext;
                break;
            }
        }
        if (!p->pNext)
            pOpenPrinter = NULL;
    }
    gOpenPrintersMutex.leave();

    /*
     * Delete it.
     */
    DebugAssert(pOpenPrinter, ("the node %p was not found!\n", pOpenPrinter));
    if (pOpenPrinter)
    {
        pOpenPrinter->pNext = NULL;
        free(pOpenPrinter);
    }
}

/**
 * Opens the OS/2 spool file associated with the open printer if not
 * already opened.
 *
 * @returns NO_ERROR on success.
 * @returns OS/2 error code. May have changed the last error.
 * @param   pOpenPrinter    Open printer instance.
 */
static int        openprinterOpenSpoolFile(POPENPRINTER pOpenPrinter)
{
    if (pOpenPrinter->hspl)
        return NO_ERROR;

    /*
     * First, get the queue name.
     */
    char    szQueue[16];
    int     rc = NO_ERROR;
    if (SplQueryPMQueueFromHandle(pOpenPrinter->hOpenPrinter, &szQueue[0], sizeof(szQueue)))
    {
        POSLIB_PRQINFO3 pPrqInfo3 = (POSLIB_PRQINFO3)OSLibSplQueryQueue(NULL, &szQueue[0], 3);
        if (pPrqInfo3)
        {
            OSLIB_DEVOPENSTRUC  DevOpen = {0};
            DevOpen.pszLogAddress = &szQueue[0];
            DevOpen.pszDriverName = pPrqInfo3->pszDriverName; /* always use default driver */
            pOpenPrinter->hspl = OSLibSplQmOpen(NULL, 4 /* must be 4 or more */, &DevOpen.pszLogAddress);
            if (pOpenPrinter->hspl)
            {
                dprintf(("WINSPOOL: openprinterOpenSpoolFile: handle %d -> hspl %#x\n",
                         pOpenPrinter->hOpenPrinter, pOpenPrinter->hspl));
                rc = NO_ERROR;
            }
            else
            {
                rc = OSLibSplWinGetLastError();
                DebugAssertFailed(("OSLibSplQmOpen failed! rc=%#x\n", rc));
            }
            free(pPrqInfo3);
        }
        else
        {
            rc = ERROR_GEN_FAILURE;
            DebugAssertFailed(("OSLibSplQueryQueue(,%s,3) failed\n", szQueue));
        }
    }
    else
    {
        rc = ERROR_GEN_FAILURE;
        DebugAssertFailed(("SplQueryPMQueueFromHandle failed\n"));
    }
    return rc;
}


/**
 * Gets the OS/2 queue name for an open printer device.
 *
 * @returns Success indicator.
 * @param   hPrinter    Printer handle.
 * @param   pszQueue    Where to put the queue name.
 * @param   cchQueue    Size of the queue buffer pointed to by pszQueue.
 */
BOOL WIN32API SplQueryPMQueueFromHandle(HANDLE hPrinter, char *pszQueue, unsigned cchQueue)
{
    BOOL            fRc = FALSE;
    DRIVER_INFO_1A *pInfo;
    DWORD           cbNeeded = 0;

#ifdef USE_OS2_DRIVERNAME
    /*
     * Validate the handle.
     */
    POPENPRINTER   pPrt = openprinterFind(hPrinter);
    if (pPrt)
    {
        /*
         * Open the printer key in the registry.
         */
        HKEY hkeyPrinters;
        if (RegCreateKeyA(HKEY_LOCAL_MACHINE, Printers, &hkeyPrinters) == ERROR_SUCCESS)
        {
            HKEY hkeyPrinter;
            if (RegOpenKeyW(hkeyPrinters, pPrt->pwszPrinterName, &hkeyPrinter) == ERROR_SUCCESS)
            {
                /*
                 * Get the queue name.
                 */
                DWORD   cb = cchQueue;
                DWORD   dwType = 0;
                if (RegQueryValueExA(hkeyPrinter, "Description", NULL, &dwType, (LPBYTE)pszQueue, &cb) == ERROR_SUCCESS)
                {
                    pszQueue[cb] = '\0';    /* paranoia!!!! */
                    dprintf(("WINSPOOL: SplQueryPMQueueFromHandle: queuename=%s\n", pszQueue));
                    fRc = TRUE;
                }
                else
                {
                    DebugAssertFailed(("Could not query value 'Comment'\n"));
                    SetLastError(ERROR_GEN_FAILURE);
                }
                RegCloseKey(hkeyPrinter);
            }
            else
            {
                DebugAssertFailed(("Could not open printer '%ls' in the registry!!!\n",
                                   pPrt->pwszPrinterName));
                SetLastError(ERROR_INVALID_PRINTER_NAME);
            }
            RegCloseKey(hkeyPrinters);
        }
        else
            SetLastError(ERROR_GEN_FAILURE);
    }
    else
        DebugAssertFailed(("Invalid handle %#x\n", hPrinter));
#else
    /* the old method of storing it in the printer driver name. */
    GetPrinterDriverA(hPrinter, NULL, 1, NULL, 0, &cbNeeded);
    pInfo = (DRIVER_INFO_1A *)malloc(cbNeeded);
    if (pInfo)
    {
        fRc = GetPrinterDriverA(hPrinter, NULL, 1, (LPBYTE)pInfo, cbNeeded, &cbNeeded)
            &&  cbNeeded - sizeof(DRIVER_INFO_1A) < cchQueue;
        DebugAssert(fRc, ("GetPrinterDriverA failed or buffer to small.\n"));
        if (fRc)
            strcpy(pszQueue, pInfo->pName);
        free(pInfo);
    }
#endif

    if (!fRc)
        dprintf(("WINSPOOL: SplQueryPMQueueFromHandle failed!\n"));
    return fRc;
}

/**
 * Get the PM printer queue name associated with the printer name
 *
 * @returns Success indicator.
 * @param   pszPrinterName  Printer device name.
 * @param   pszQueue        Where to put the queue name.
 * @param   cchQueue        Size of the queue buffer pointed to by pszQueue.
 */
BOOL WIN32API SplQueryPMQueueName(LPSTR pszPrinterName, LPSTR pszQueue, INT cchQueue)
{
    BOOL   fRc = FALSE;
    HANDLE hPrinter;

    // Get the PM Queue name corresponding to the printer device
    if (OpenPrinterA(pszPrinterName, &hPrinter, NULL))
    {
        fRc = SplQueryPMQueueFromHandle(hPrinter, pszQueue, cchQueue);
        ClosePrinter(hPrinter);
    }

    return fRc;
}

/*************************************************************************
 * SHDeleteKeyA   [SHLWAPI.@]
 *
 * Delete a registry key and any sub keys/values present
 *
 * PARAMS
 *   hKey       [I] Handle to registry key
 *   lpszSubKey [I] Name of sub key to delete
 *
 * RETURNS
 *   Success: ERROR_SUCCESS. The key is deleted.
 *   Failure: An error code from RegOpenKeyExA, RegQueryInfoKeyA,
 *          RegEnumKeyExA or RegDeleteKeyA.
 *
 *
 * (Rewind code)
 *
 * NOTE: I don't want a dependency on shlwapi in winspool
 *
 */
static DWORD SHDeleteKeyW(HKEY hKey, LPCWSTR lpszSubKey)
{
  DWORD dwRet, dwKeyCount = 0, dwMaxSubkeyLen = 0, dwSize, i;
  WCHAR szNameBuf[MAX_PATH], *lpszName = szNameBuf;
  HKEY hSubKey = 0;

  dwRet = RegOpenKeyExW(hKey, lpszSubKey, 0, KEY_READ, &hSubKey);
  if(!dwRet)
  {
    /* Find how many subkeys there are */
    dwRet = RegQueryInfoKeyW(hSubKey, NULL, NULL, NULL, &dwKeyCount,
                             &dwMaxSubkeyLen, NULL, NULL, NULL, NULL, NULL, NULL);
    if(!dwRet)
    {
      dwMaxSubkeyLen++;
      if (dwMaxSubkeyLen > sizeof(szNameBuf)/sizeof(WCHAR))
        /* Name too big: alloc a buffer for it */
        lpszName = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, dwMaxSubkeyLen*sizeof(WCHAR));

      if(!lpszName)
        dwRet = ERROR_NOT_ENOUGH_MEMORY;
      else
      {
        /* Recursively delete all the subkeys */
        for(i = 0; i < dwKeyCount && !dwRet; i++)
        {
          dwSize = dwMaxSubkeyLen;
          dwRet = RegEnumKeyExW(hSubKey, i, lpszName, &dwSize, NULL, NULL, NULL, NULL);
          if(!dwRet)
            dwRet = SHDeleteKeyW(hSubKey, lpszName);
        }

        if (lpszName != szNameBuf)
          HeapFree(GetProcessHeap(), 0, lpszName); /* Free buffer if allocated */
      }
    }

    RegCloseKey(hSubKey);
    if(!dwRet)
      dwRet = RegDeleteKeyW(hKey, lpszSubKey);
  }
  return dwRet;
}

#endif /* __WIN32OS2__ */


static inline DWORD set_reg_szW(HKEY hkey, WCHAR *keyname, WCHAR *value)
{
#if 0 /* bird: On OS/2 the API expects number of chars of unicode strings.
       * On Win32 you are required to include the '\0' in the count. */
    return RegSetValueExW(hkey, keyname, 0, REG_SZ, (LPBYTE)value,
                   lstrlenW(value) * sizeof(WCHAR));
#else
    return RegSetValueExW(hkey, keyname, 0, REG_SZ, (LPBYTE)value, lstrlenW(value) + 1);
#endif
}

/***********************************************************
 *      DEVMODEcpyAtoW
 */
static LPDEVMODEW DEVMODEcpyAtoW(DEVMODEW *dmW, const DEVMODEA *dmA)
{
    BOOL Formname;
    ptrdiff_t off_formname = (char *)dmA->dmFormName - (char *)dmA;
    DWORD size;

    Formname = (dmA->dmSize > off_formname);
    size = dmA->dmSize + CCHDEVICENAME + (Formname ? CCHFORMNAME : 0);
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)dmA->dmDeviceName, -1, dmW->dmDeviceName,
			CCHDEVICENAME);
    if(!Formname) {
      memcpy(&dmW->dmSpecVersion, &dmA->dmSpecVersion,
	     dmA->dmSize - CCHDEVICENAME);
    } else {
      memcpy(&dmW->dmSpecVersion, &dmA->dmSpecVersion,
	     off_formname - CCHDEVICENAME);
      MultiByteToWideChar(CP_ACP, 0, (LPCSTR)dmA->dmFormName, -1, dmW->dmFormName,
			  CCHFORMNAME);
      memcpy(&dmW->dmLogPixels, &dmA->dmLogPixels, dmA->dmSize -
	     (off_formname + CCHFORMNAME));
    }
    dmW->dmSize = size;
    memcpy((char *)dmW + dmW->dmSize, (char *)dmA + dmA->dmSize,
	   dmA->dmDriverExtra);
    return dmW;
}

/***********************************************************
 *      DEVMODEdupAtoW
 * Creates a unicode copy of supplied devmode on heap
 */
static LPDEVMODEW DEVMODEdupAtoW(HANDLE heap, const DEVMODEA *dmA)
{
    LPDEVMODEW dmW;
    DWORD size;
    BOOL Formname;
    ptrdiff_t off_formname;

    if(!dmA) return NULL;

    off_formname = (char *)dmA->dmFormName - (char *)dmA;
    Formname = (dmA->dmSize > off_formname);
    size = dmA->dmSize + CCHDEVICENAME + (Formname ? CCHFORMNAME : 0);
    dmW = (LPDEVMODEW) HeapAlloc(heap, HEAP_ZERO_MEMORY, size + dmA->dmDriverExtra);
    return DEVMODEcpyAtoW(dmW, dmA);
}

/***********************************************************
 *      DEVMODEdupWtoA
 * Creates an ascii copy of supplied devmode on heap
 */
static LPDEVMODEA DEVMODEdupWtoA(HANDLE heap, const DEVMODEW *dmW)
{
    LPDEVMODEA dmA;
    DWORD size;
    BOOL Formname;
    ptrdiff_t off_formname = (char *)dmW->dmFormName - (char *)dmW;

    if(!dmW) return NULL;
    Formname = (dmW->dmSize > off_formname);
    size = dmW->dmSize - CCHDEVICENAME - (Formname ? CCHFORMNAME : 0);
    dmA = (LPDEVMODEA)HeapAlloc(heap, HEAP_ZERO_MEMORY, size + dmW->dmDriverExtra);
    WideCharToMultiByte(CP_ACP, 0, dmW->dmDeviceName, -1, (LPSTR)dmA->dmDeviceName,
            CCHDEVICENAME, NULL, NULL);
    if(!Formname) {
      memcpy(&dmA->dmSpecVersion, &dmW->dmSpecVersion,
         dmW->dmSize - CCHDEVICENAME * sizeof(WCHAR));
    } else {
      memcpy(&dmA->dmSpecVersion, &dmW->dmSpecVersion,
         off_formname - CCHDEVICENAME * sizeof(WCHAR));
      WideCharToMultiByte(CP_ACP, 0, dmW->dmFormName, -1, (LPSTR)dmA->dmFormName,
              CCHFORMNAME, NULL, NULL);
      memcpy(&dmA->dmLogPixels, &dmW->dmLogPixels, dmW->dmSize -
         (off_formname + CCHFORMNAME * sizeof(WCHAR)));
    }
    dmA->dmSize = size;
    memcpy((char *)dmA + dmA->dmSize, (char *)dmW + dmW->dmSize,
       dmW->dmDriverExtra);
    return dmA;
}

/******************************************************************
 *  WINSPOOL_GetOpenedPrinter
 *  Get the pointer to the opened printer referred by the handle
 */
static LPCWSTR WINSPOOL_GetOpenedPrinter(HANDLE printerHandle)
{
#ifdef __WIN32OS2__
    POPENPRINTER    p = openprinterFind(printerHandle);
    if (p)
        return p->pwszPrinterName;
    SetLastError(ERROR_INVALID_HANDLE);
    return NULL;
#else
    int idx = (int)printerHandle;
    if ((idx <= 0) || (idx > nb_printers))
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
    }
    return printer_array[idx - 1];
#endif
}


/*****************************************************************************
 *          WINSPOOL_OpenDriverReg [internal]
 *
 * opens the registry for the printer drivers depending on the given input
 * variable pEnvironment
 *
 * RETURNS:
 *    the opened hkey on success
 *    NULL on error
 */
static HKEY WINSPOOL_OpenDriverReg( LPVOID pEnvironment, BOOL unicode)
{   HKEY  retval;
    LPSTR lpKey, p = NULL;

#ifdef DEBUG                            /* __WIN32OS2__ */
    dprintf(("%s\n",
	  (unicode) ? debugstr_w((LPCWSTR)pEnvironment) : debugstr_a((LPCSTR)pEnvironment)));
#endif                                  /* __WIN32OS2__ */

    if(pEnvironment)
        p = (unicode) ? (LPSTR)HEAP_strdupWtoA( GetProcessHeap(), 0, (LPCWSTR)pEnvironment) :
                       (LPSTR) pEnvironment;
    else {
        OSVERSIONINFOA ver;
        ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

        if(!GetVersionExA( &ver))
            return 0;

        switch (ver.dwPlatformId) {
             case VER_PLATFORM_WIN32s:
                  ERR("win32 style printing used with 16 bits app, try specifying 'win95' Windows version\n");
                  return 0;

             case VER_PLATFORM_WIN32_NT:
                  p = (LPSTR)"Windows NT x86";
                  break;
             default:
                  p = (LPSTR)"Windows 4.0";
                  break;
        }
        TRACE("set environment to %s\n", p);
    }

    lpKey = (char*) HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY,
                       strlen(p) + strlen(Drivers));
    sprintf( lpKey, Drivers, p);

    TRACE("%s\n", lpKey);

    if(RegCreateKeyA(HKEY_LOCAL_MACHINE, lpKey, &retval) !=
       ERROR_SUCCESS)
       retval = 0;

    if(pEnvironment && unicode)
       HeapFree( GetProcessHeap(), 0, p);
    HeapFree( GetProcessHeap(), 0, lpKey);

    return retval;
}


/*****************************************************************************
 *    WINSPOOL_GetStringFromReg
 *
 * Get ValueName from hkey storing result in ptr.  buflen is space left in ptr
 * String is stored either as unicode or ascii.
 * Bit of a hack here to get the ValueName if we want ascii.
 */
static BOOL WINSPOOL_GetStringFromReg(HKEY hkey, LPCWSTR ValueName, LPBYTE ptr,
				      DWORD buflen, DWORD *needed,
				      BOOL unicode)
{
    DWORD sz = buflen, type;
    LONG ret;

    if(unicode)
        ret = RegQueryValueExW(hkey, (LPWSTR)ValueName, 0, &type, ptr, &sz);
    else {
        LPSTR ValueNameA = HEAP_strdupWtoA(GetProcessHeap(),0,ValueName);
        ret = RegQueryValueExA(hkey, ValueNameA, 0, &type, ptr, &sz);
	HeapFree(GetProcessHeap(),0,ValueNameA);
    }
    if(ret != ERROR_SUCCESS && ret != ERROR_MORE_DATA) {
        WARN("Got ret = %ld\n", ret);
	*needed = 0;
	return FALSE;
    }
    *needed = sz;
    return TRUE;
}


/*****************************************************************************
 *    WINSPOOL_SetDevModeFromReg
 *
 * Get ValueName from hkey storing result in ptr.  buflen is space left in ptr
 * DevMode is stored either as unicode or ascii.
 */
static BOOL WINSPOOL_SetDevModeFromReg(HKEY hkey, LPCWSTR ValueName,
				       LPBYTE ptr, DWORD buflen)
{
    DWORD sz = buflen, type;
    LONG ret;

    ret = RegSetValueExW(hkey, (LPWSTR)ValueName, 0, REG_BINARY, ptr, sz);
    if ((ret != ERROR_SUCCESS && ret != ERROR_MORE_DATA)) sz = 0;
    if (sz < sizeof(DEVMODEA))
    {
        ERR("failed to write DEVMODEA for %s ( size %ld)\n",debugstr_w(ValueName),sz);
	return FALSE;
    }
    return TRUE;
}

/*****************************************************************************
 *    WINSPOOL_GetDevModeFromReg
 *
 * Get ValueName from hkey storing result in ptr.  buflen is space left in ptr
 * DevMode is stored either as unicode or ascii.
 */
static BOOL WINSPOOL_GetDevModeFromReg(HKEY hkey, LPCWSTR ValueName,
				       LPBYTE ptr,
				       DWORD buflen, DWORD *needed,
				       BOOL unicode)
{
    DWORD sz = buflen, type;
    LONG ret;

    if (ptr && buflen>=sizeof(DEVMODEA)) memset(ptr, 0, sizeof(DEVMODEA));
    ret = RegQueryValueExW(hkey, (LPWSTR)ValueName, 0, &type, ptr, &sz);
    if ((ret != ERROR_SUCCESS && ret != ERROR_MORE_DATA)) sz = 0;
    if (sz < sizeof(DEVMODEA))
    {
        ERR("corrupted registry for %s ( size %ld)\n",debugstr_w(ValueName),sz);
	return FALSE;
    }
    /* ensures that dmSize is not erratically bogus if registry is invalid */
    if (ptr && ((DEVMODEA*)ptr)->dmSize < sizeof(DEVMODEA))
        ((DEVMODEA*)ptr)->dmSize = sizeof(DEVMODEA);
    if(unicode) {
	sz += (CCHDEVICENAME + CCHFORMNAME);
	if(buflen >= sz) {
	    DEVMODEW *dmW = DEVMODEdupAtoW(GetProcessHeap(), (DEVMODEA*)ptr);
	    memcpy(ptr, dmW, sz);
	    HeapFree(GetProcessHeap(),0,dmW);
	}
    }
    *needed = sz;
    return TRUE;
}

/*****************************************************************************
 *    WINSPOOL_GetDWORDFromReg
 *
 * Return DWORD associated with ValueName from hkey.
 */
static DWORD WINSPOOL_GetDWORDFromReg(HKEY hkey, LPCSTR ValueName)
{
    DWORD sz = sizeof(DWORD), type, value = 0;
    LONG ret;

    ret = RegQueryValueExA(hkey, (LPSTR)ValueName, 0, &type, (LPBYTE)&value, &sz);

    if(ret != ERROR_SUCCESS) {
        WARN("Got ret = %ld on name %s\n", ret, ValueName);
	return 0;
    }
    if(type != REG_DWORD) {
        ERR("Got type %ld\n", type);
	return 0;
    }
    return value;
}

/*****************************************************************************
 *    WINSPOOL_GetDefaultDevMode
 *
 * Get a default DevMode values for wineps.
 * FIXME - use ppd.
 */

static void WINSPOOL_GetDefaultDevMode(
	LPBYTE ptr,
	DWORD buflen, DWORD *needed,
	BOOL unicode)
{
    DEVMODEA	dm;

	/* fill default DEVMODE - should be read from ppd... */
	ZeroMemory( &dm, sizeof(dm) );
	strcpy((char*)dm.dmDeviceName,"wineps");
	dm.dmSpecVersion = DM_SPECVERSION;
	dm.dmDriverVersion = 1;
	dm.dmSize = sizeof(DEVMODEA);
	dm.dmDriverExtra = 0;
	dm.dmFields =
		DM_ORIENTATION | DM_PAPERSIZE |
		DM_PAPERLENGTH | DM_PAPERWIDTH |
		DM_SCALE | DM_COLLATE |
		DM_COPIES |
		DM_DEFAULTSOURCE | DM_PRINTQUALITY |
		DM_YRESOLUTION | DM_TTOPTION;
	dm.DUMMYSTRUCTNAME1_DOT dmOrientation = DMORIENT_PORTRAIT;
	dm.DUMMYSTRUCTNAME1_DOT dmPaperSize = DMPAPER_A4;
	dm.DUMMYSTRUCTNAME1_DOT dmPaperLength = 2970;
	dm.DUMMYSTRUCTNAME1_DOT dmPaperWidth = 2100;
	dm.dmScale = 100;
	dm.dmCopies = 1;
	dm.dmDefaultSource = DMBIN_AUTO;
	dm.dmPrintQuality = DMRES_MEDIUM;
	/* dm.dmColor */
	/* dm.dmDuplex */
	dm.dmYResolution = 300; /* 300dpi */
	dm.dmTTOption = DMTT_BITMAP;
	dm.dmCollate = 1;
	/* dm.dmFormName */
	/* dm.dmLogPixels */
	/* dm.dmBitsPerPel */
	/* dm.dmPelsWidth */
	/* dm.dmPelsHeight */
	/* dm.dmDisplayFlags */
	/* dm.dmDisplayFrequency */
	/* dm.dmICMMethod */
	/* dm.dmICMIntent */
	/* dm.dmMediaType */
	/* dm.dmDitherType */
	/* dm.dmReserved1 */
	/* dm.dmReserved2 */
	/* dm.dmPanningWidth */
	/* dm.dmPanningHeight */

    if(unicode) {
	if(buflen >= sizeof(DEVMODEW)) {
	    DEVMODEW *pdmW = DEVMODEdupAtoW(GetProcessHeap(), &dm );
	    memcpy(ptr, pdmW, sizeof(DEVMODEW));
	    HeapFree(GetProcessHeap(),0,pdmW);
	}
	*needed = sizeof(DEVMODEW);
    }
    else
    {
	if(buflen >= sizeof(DEVMODEA)) {
	    memcpy(ptr, &dm, sizeof(DEVMODEA));
	}
	*needed = sizeof(DEVMODEA);
    }
}


/*********************************************************************
 *    WINSPOOL_GetPrinter_2
 *
 * Fills out a PRINTER_INFO_2A|W struct storing the strings in buf.
 * The strings are either stored as unicode or ascii.
 */
static BOOL WINSPOOL_GetPrinter_2(HKEY hkeyPrinter, PRINTER_INFO_2W *pi2,
				  LPBYTE buf, DWORD cbBuf, LPDWORD pcbNeeded,
				  BOOL unicode)
{
    DWORD size, left = cbBuf;
    BOOL space = (cbBuf > 0);
    LPBYTE ptr = buf;

    *pcbNeeded = 0;

    if(WINSPOOL_GetStringFromReg(hkeyPrinter, NameW, ptr, left, &size,
				 unicode)) {
        if(space && size <= left) {
	    pi2->pPrinterName = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, Share_NameW, ptr, left, &size,
				 unicode)) {
        if(space && size <= left) {
	    pi2->pShareName = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, PortW, ptr, left, &size,
				 unicode)) {
        if(space && size <= left) {
	    pi2->pPortName = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, Printer_DriverW, ptr, left,
				 &size, unicode)) {
        if(space && size <= left) {
	    pi2->pDriverName = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, DescriptionW, ptr, left, &size,
				 unicode)) {
        if(space && size <= left) {
	    pi2->pComment = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, LocationW, ptr, left, &size,
				 unicode)) {
        if(space && size <= left) {
	    pi2->pLocation = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetDevModeFromReg(hkeyPrinter, Default_DevModeW, ptr, left,
				  &size, unicode)) {
        if(space && size <= left) {
	    pi2->pDevMode = (LPDEVMODEW)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    else
    {
	MESSAGE( "no DevMode in registry. please setup your printer again.\n"
		 "use the default hard-coded DevMode(wineps/A4/300dpi).\n" );
	WINSPOOL_GetDefaultDevMode(ptr, left, &size, unicode);
        if(space && size <= left) {
	    pi2->pDevMode = (LPDEVMODEW)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, Separator_FileW, ptr, left,
				 &size, unicode)) {
        if(space && size <= left) {
            pi2->pSepFile = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, Print_ProcessorW, ptr, left,
				 &size, unicode)) {
        if(space && size <= left) {
	    pi2->pPrintProcessor = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, DatatypeW, ptr, left,
				 &size, unicode)) {
        if(space && size <= left) {
	    pi2->pDatatype = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, ParametersW, ptr, left,
				 &size, unicode)) {
        if(space && size <= left) {
	    pi2->pParameters = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(pi2) {
        pi2->Attributes = WINSPOOL_GetDWORDFromReg(hkeyPrinter, "Attributes");
        pi2->Priority = WINSPOOL_GetDWORDFromReg(hkeyPrinter, "Priority");
        pi2->DefaultPriority = WINSPOOL_GetDWORDFromReg(hkeyPrinter,
							"Default Priority");
        pi2->StartTime = WINSPOOL_GetDWORDFromReg(hkeyPrinter, "StartTime");
        pi2->UntilTime = WINSPOOL_GetDWORDFromReg(hkeyPrinter, "UntilTime");
    }

    if(!space && pi2) /* zero out pi2 if we can't completely fill buf */
        memset(pi2, 0, sizeof(*pi2));

    return space;
}

/*********************************************************************
 *    WINSPOOL_GetPrinter_4
 *
 * Fills out a PRINTER_INFO_4 struct storing the strings in buf.
 */
static BOOL WINSPOOL_GetPrinter_4(HKEY hkeyPrinter, PRINTER_INFO_4W *pi4,
				  LPBYTE buf, DWORD cbBuf, LPDWORD pcbNeeded,
				  BOOL unicode)
{
    DWORD size, left = cbBuf;
    BOOL space = (cbBuf > 0);
    LPBYTE ptr = buf;

    *pcbNeeded = 0;

    if(WINSPOOL_GetStringFromReg(hkeyPrinter, NameW, ptr, left, &size,
				 unicode)) {
        if(space && size <= left) {
	    pi4->pPrinterName = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(pi4) {
        pi4->Attributes = WINSPOOL_GetDWORDFromReg(hkeyPrinter, "Attributes");
    }

    if(!space && pi4) /* zero out pi4 if we can't completely fill buf */
        memset(pi4, 0, sizeof(*pi4));

    return space;
}

/*********************************************************************
 *    WINSPOOL_GetPrinter_5
 *
 * Fills out a PRINTER_INFO_5 struct storing the strings in buf.
 */
static BOOL WINSPOOL_GetPrinter_5(HKEY hkeyPrinter, PRINTER_INFO_5W *pi5,
				  LPBYTE buf, DWORD cbBuf, LPDWORD pcbNeeded,
				  BOOL unicode)
{
    DWORD size, left = cbBuf;
    BOOL space = (cbBuf > 0);
    LPBYTE ptr = buf;

    *pcbNeeded = 0;

    if(WINSPOOL_GetStringFromReg(hkeyPrinter, NameW, ptr, left, &size,
				 unicode)) {
        if(space && size <= left) {
	    pi5->pPrinterName = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(WINSPOOL_GetStringFromReg(hkeyPrinter, PortW, ptr, left, &size,
				 unicode)) {
        if(space && size <= left) {
	    pi5->pPortName = (LPWSTR)ptr;
	    ptr += size;
	    left -= size;
	} else
	    space = FALSE;
	*pcbNeeded += size;
    }
    if(pi5) {
        pi5->Attributes = WINSPOOL_GetDWORDFromReg(hkeyPrinter, "Attributes");
        pi5->DeviceNotSelectedTimeout = WINSPOOL_GetDWORDFromReg(hkeyPrinter,
								"dnsTimeout");
        pi5->TransmissionRetryTimeout = WINSPOOL_GetDWORDFromReg(hkeyPrinter,
								 "txTimeout");
    }

    if(!space && pi5) /* zero out pi5 if we can't completely fill buf */
        memset(pi5, 0, sizeof(*pi5));

    return space;
}


/*****************************************************************************
 *          WINSPOOL_GetPrinter
 *
 *    Implementation of GetPrinterA|W.  Relies on PRINTER_INFO_*W being
 *    essentially the same as PRINTER_INFO_*A. i.e. the structure itself is
 *    just a collection of pointers to strings.
 */
static BOOL WINSPOOL_GetPrinter(HANDLE hPrinter, DWORD Level, LPBYTE pPrinter,
				DWORD cbBuf, LPDWORD pcbNeeded, BOOL unicode)
{
    LPCWSTR name;
    DWORD size, needed = 0;
    LPBYTE ptr = NULL;
    HKEY hkeyPrinter, hkeyPrinters;
    BOOL ret;

    TRACE("(%p,%ld,%p,%ld,%p)\n",hPrinter,Level,pPrinter,cbBuf, pcbNeeded);

    if (!(name = WINSPOOL_GetOpenedPrinter(hPrinter))) return FALSE;

    if(RegCreateKeyA(HKEY_LOCAL_MACHINE, Printers, &hkeyPrinters) !=
       ERROR_SUCCESS) {
        ERR("Can't create Printers key\n");
	return FALSE;
    }
    if(RegOpenKeyW(hkeyPrinters, name, &hkeyPrinter) != ERROR_SUCCESS)
    {
        ERR("Can't find opened printer %s in registry\n", debugstr_w(name));
	RegCloseKey(hkeyPrinters);
        SetLastError(ERROR_INVALID_PRINTER_NAME); /* ? */
	return FALSE;
    }

    switch(Level) {
    case 2:
      {
        PRINTER_INFO_2W *pi2 = (PRINTER_INFO_2W *)pPrinter;

        size = sizeof(PRINTER_INFO_2W);
	if(size <= cbBuf) {
	    ptr = pPrinter + size;
	    cbBuf -= size;
	    memset(pPrinter, 0, size);
	} else {
	    pi2 = NULL;
	    cbBuf = 0;
	}
	ret = WINSPOOL_GetPrinter_2(hkeyPrinter, pi2, ptr, cbBuf, &needed,
				    unicode);
	needed += size;
	break;
      }

    case 4:
      {
	PRINTER_INFO_4W *pi4 = (PRINTER_INFO_4W *)pPrinter;

        size = sizeof(PRINTER_INFO_4W);
	if(size <= cbBuf) {
	    ptr = pPrinter + size;
	    cbBuf -= size;
	    memset(pPrinter, 0, size);
	} else {
	    pi4 = NULL;
	    cbBuf = 0;
	}
	ret = WINSPOOL_GetPrinter_4(hkeyPrinter, pi4, ptr, cbBuf, &needed,
				    unicode);
	needed += size;
	break;
      }


    case 5:
      {
        PRINTER_INFO_5W *pi5 = (PRINTER_INFO_5W *)pPrinter;

        size = sizeof(PRINTER_INFO_5W);
	if(size <= cbBuf) {
	    ptr = pPrinter + size;
	    cbBuf -= size;
	    memset(pPrinter, 0, size);
	} else {
	    pi5 = NULL;
	    cbBuf = 0;
	}

	ret = WINSPOOL_GetPrinter_5(hkeyPrinter, pi5, ptr, cbBuf, &needed,
				    unicode);
	needed += size;
	break;
      }

    default:
        FIXME("Unimplemented level %ld\n", Level);
        SetLastError(ERROR_INVALID_LEVEL);
	RegCloseKey(hkeyPrinters);
	RegCloseKey(hkeyPrinter);
	return FALSE;
    }

    RegCloseKey(hkeyPrinter);
    RegCloseKey(hkeyPrinters);

    TRACE("returing %d needed = %ld\n", ret, needed);
    if(pcbNeeded) *pcbNeeded = needed;
    if(!ret)
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return ret;
}


/*****************************************************************************
 *          WINSPOOL_GetDriverInfoFromReg [internal]
 *
 *    Enters the information from the registry into the DRIVER_INFO struct
 *
 * RETURNS
 *    zero if the printer driver does not exist in the registry
 *    (only if Level > 1) otherwise nonzero
 */
static BOOL WINSPOOL_GetDriverInfoFromReg(
                            HKEY    hkeyDrivers,
                            LPWSTR  DriverName,
                            LPWSTR  pEnvironment,
                            DWORD   Level,
                            LPBYTE  ptr,            /* DRIVER_INFO */
                            LPBYTE  pDriverStrings, /* strings buffer */
                            DWORD   cbBuf,          /* size of string buffer */
                            LPDWORD pcbNeeded,      /* space needed for str. */
                            BOOL    unicode)        /* type of strings */
{   DWORD  dw, size, tmp, type;
    HKEY   hkeyDriver;
    LPBYTE strPtr = pDriverStrings;

    TRACE("%s,%s,%ld,%p,%p,%ld,%d\n",
          debugstr_w(DriverName), debugstr_w(pEnvironment),
          Level, ptr, pDriverStrings, cbBuf, unicode);

    if(unicode) {
        *pcbNeeded = (lstrlenW(DriverName) + 1) * sizeof(WCHAR);
            if (*pcbNeeded <= cbBuf)
               strcpyW((LPWSTR)strPtr, DriverName);
    } else {
        *pcbNeeded = WideCharToMultiByte(CP_ACP, 0, DriverName, -1, NULL, 0,
                                          NULL, NULL);
        if(*pcbNeeded <= cbBuf)
            WideCharToMultiByte(CP_ACP, 0, DriverName, -1, (LPSTR)strPtr, *pcbNeeded,
                                NULL, NULL);
    }
    if(Level == 1) {
       if(ptr)
          ((PDRIVER_INFO_1W) ptr)->pName = (LPWSTR) strPtr;
       return TRUE;
    } else {
       if(ptr)
          ((PDRIVER_INFO_3W) ptr)->pName = (LPWSTR) strPtr;
       strPtr = (pDriverStrings) ? (pDriverStrings + (*pcbNeeded)) : NULL;
    }

    if(!DriverName[0] || RegOpenKeyW(hkeyDrivers, DriverName, &hkeyDriver) != ERROR_SUCCESS) {
        ERR("Can't find driver '%s' in registry\n", debugstr_w(DriverName));
        SetLastError(ERROR_UNKNOWN_PRINTER_DRIVER); /* ? */
        return FALSE;
    }

    size = sizeof(dw);
    if(RegQueryValueExA(hkeyDriver, "Version", 0, &type, (PBYTE)&dw, &size) !=
        ERROR_SUCCESS)
         WARN("Can't get Version\n");
    else if(ptr)
         ((PDRIVER_INFO_3A) ptr)->cVersion = dw;

    if(!pEnvironment)
        pEnvironment = DefaultEnvironmentW;
    if(unicode)
        size = (lstrlenW(pEnvironment) + 1) * sizeof(WCHAR);
    else
        size = WideCharToMultiByte(CP_ACP, 0, pEnvironment, -1, NULL, 0,
			           NULL, NULL);
    *pcbNeeded += size;
    if(*pcbNeeded <= cbBuf) {
        if(unicode)
            strcpyW((LPWSTR)strPtr, pEnvironment);
        else
            WideCharToMultiByte(CP_ACP, 0, pEnvironment, -1, (LPSTR)strPtr, size,
                                NULL, NULL);
        if(ptr)
            ((PDRIVER_INFO_3W) ptr)->pEnvironment = (LPWSTR)strPtr;
        strPtr = (pDriverStrings) ? (pDriverStrings + (*pcbNeeded)) : NULL;
    }

    if(WINSPOOL_GetStringFromReg(hkeyDriver, DriverW, strPtr, 0, &size,
			         unicode)) {
        *pcbNeeded += size;
        if(*pcbNeeded <= cbBuf)
            WINSPOOL_GetStringFromReg(hkeyDriver, DriverW, strPtr, size, &tmp,
                                      unicode);
        if(ptr)
            ((PDRIVER_INFO_3W) ptr)->pDriverPath = (LPWSTR)strPtr;
        strPtr = (pDriverStrings) ? (pDriverStrings + (*pcbNeeded)) : NULL;
    }

    if(WINSPOOL_GetStringFromReg(hkeyDriver, Data_FileW, strPtr, 0, &size,
			         unicode)) {
        *pcbNeeded += size;
        if(*pcbNeeded <= cbBuf)
            WINSPOOL_GetStringFromReg(hkeyDriver, Data_FileW, strPtr, size,
                                      &tmp, unicode);
        if(ptr)
            ((PDRIVER_INFO_3W) ptr)->pDataFile = (LPWSTR)strPtr;
        strPtr = (pDriverStrings) ? pDriverStrings + (*pcbNeeded) : NULL;
    }

    if(WINSPOOL_GetStringFromReg(hkeyDriver, Configuration_FileW, strPtr,
                                 0, &size, unicode)) {
        *pcbNeeded += size;
        if(*pcbNeeded <= cbBuf)
            WINSPOOL_GetStringFromReg(hkeyDriver, Configuration_FileW, strPtr,
                                      size, &tmp, unicode);
        if(ptr)
            ((PDRIVER_INFO_3W) ptr)->pConfigFile = (LPWSTR)strPtr;
        strPtr = (pDriverStrings) ? pDriverStrings + (*pcbNeeded) : NULL;
    }

    if(Level == 2 ) {
        RegCloseKey(hkeyDriver);
        TRACE("buffer space %ld required %ld\n", cbBuf, *pcbNeeded);
        return TRUE;
    }

    if(WINSPOOL_GetStringFromReg(hkeyDriver, Help_FileW, strPtr, 0, &size,
                                 unicode)) {
        *pcbNeeded += size;
        if(*pcbNeeded <= cbBuf)
            WINSPOOL_GetStringFromReg(hkeyDriver, Help_FileW, strPtr,
                                      size, &tmp, unicode);
        if(ptr)
            ((PDRIVER_INFO_3W) ptr)->pHelpFile = (LPWSTR)strPtr;
        strPtr = (pDriverStrings) ? pDriverStrings + (*pcbNeeded) : NULL;
    }

    if(WINSPOOL_GetStringFromReg(hkeyDriver, Dependent_FilesW, strPtr, 0,
			     &size, unicode)) {
        *pcbNeeded += size;
        if(*pcbNeeded <= cbBuf)
            WINSPOOL_GetStringFromReg(hkeyDriver, Dependent_FilesW, strPtr,
                                      size, &tmp, unicode);
        if(ptr)
            ((PDRIVER_INFO_3W) ptr)->pDependentFiles = (LPWSTR)strPtr;
        strPtr = (pDriverStrings) ? pDriverStrings + (*pcbNeeded) : NULL;
    }

    if(WINSPOOL_GetStringFromReg(hkeyDriver, MonitorW, strPtr, 0, &size,
                                 unicode)) {
        *pcbNeeded += size;
        if(*pcbNeeded <= cbBuf)
            WINSPOOL_GetStringFromReg(hkeyDriver, MonitorW, strPtr,
                                      size, &tmp, unicode);
        if(ptr)
            ((PDRIVER_INFO_3W) ptr)->pMonitorName = (LPWSTR)strPtr;
        strPtr = (pDriverStrings) ? pDriverStrings + (*pcbNeeded) : NULL;
    }

    if(WINSPOOL_GetStringFromReg(hkeyDriver, DatatypeW, strPtr, 0, &size,
                                 unicode)) {
        *pcbNeeded += size;
        if(*pcbNeeded <= cbBuf)
            WINSPOOL_GetStringFromReg(hkeyDriver, MonitorW, strPtr,
                                      size, &tmp, unicode);
        if(ptr)
            ((PDRIVER_INFO_3W) ptr)->pDefaultDataType = (LPWSTR)strPtr;
        strPtr = (pDriverStrings) ? pDriverStrings + (*pcbNeeded) : NULL;
    }

    TRACE("buffer space %ld required %ld\n", cbBuf, *pcbNeeded);
    RegCloseKey(hkeyDriver);
    return TRUE;
}

/*****************************************************************************
 *          WINSPOOL_GetPrinterDriver
 */
static BOOL WINSPOOL_GetPrinterDriver(HANDLE hPrinter, LPWSTR pEnvironment,
				      DWORD Level, LPBYTE pDriverInfo,
				      DWORD cbBuf, LPDWORD pcbNeeded,
				      BOOL unicode)
{
    LPCWSTR name;
    WCHAR DriverName[100];
    DWORD ret, type, size, needed = 0;
    LPBYTE ptr = NULL;
    HKEY hkeyPrinter, hkeyPrinters, hkeyDrivers;

    TRACE("(%p,%s,%ld,%p,%ld,%p)\n",hPrinter,debugstr_w(pEnvironment),
	  Level,pDriverInfo,cbBuf, pcbNeeded);

    ZeroMemory(pDriverInfo, cbBuf);

    if (!(name = WINSPOOL_GetOpenedPrinter(hPrinter))) return FALSE;

    if(Level < 1 || Level > 3) {
        SetLastError(ERROR_INVALID_LEVEL);
	return FALSE;
    }
    if(RegCreateKeyA(HKEY_LOCAL_MACHINE, Printers, &hkeyPrinters) !=
       ERROR_SUCCESS) {
        ERR("Can't create Printers key\n");
	return FALSE;
    }
    if(RegOpenKeyW(hkeyPrinters, name, &hkeyPrinter)
       != ERROR_SUCCESS) {
        ERR("Can't find opened printer %s in registry\n", debugstr_w(name));
	RegCloseKey(hkeyPrinters);
        SetLastError(ERROR_INVALID_PRINTER_NAME); /* ? */
	return FALSE;
    }
    size = sizeof(DriverName);
    DriverName[0] = 0;
    ret = RegQueryValueExW(hkeyPrinter, Printer_DriverW, 0, &type,
			   (LPBYTE)DriverName, &size);
    RegCloseKey(hkeyPrinter);
    RegCloseKey(hkeyPrinters);
    if(ret != ERROR_SUCCESS) {
        ERR("Can't get DriverName for printer %s\n", debugstr_w(name));
	return FALSE;
    }

    hkeyDrivers = WINSPOOL_OpenDriverReg( pEnvironment, TRUE);
    if(!hkeyDrivers) {
        ERR("Can't create Drivers key\n");
	return FALSE;
    }

    switch(Level) {
    case 1:
        size = sizeof(DRIVER_INFO_1W);
	break;
    case 2:
        size = sizeof(DRIVER_INFO_2W);
	break;
    case 3:
        size = sizeof(DRIVER_INFO_3W);
	break;
    default:
        ERR("Invalid level\n");
	return FALSE;
    }

    if(size <= cbBuf)
        ptr = pDriverInfo + size;

    if(!WINSPOOL_GetDriverInfoFromReg(hkeyDrivers, DriverName,
                         pEnvironment, Level, pDriverInfo,
                         (cbBuf < size) ? NULL : ptr,
                         (cbBuf < size) ? 0 : cbBuf - size,
                         &needed, unicode)) {
            RegCloseKey(hkeyDrivers);
            return FALSE;
    }

    RegCloseKey(hkeyDrivers);

    if(pcbNeeded) *pcbNeeded = size + needed;
    TRACE("buffer space %ld required %ld\n", cbBuf, *pcbNeeded);
    if(cbBuf >= needed) return TRUE;
    SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;
}

/*****************************************************************************
 *          WINSPOOL_EnumPrinters
 *
 *    Implementation of EnumPrintersA|W
 */
static BOOL WINSPOOL_EnumPrinters(DWORD dwType, LPWSTR lpszName,
				  DWORD dwLevel, LPBYTE lpbPrinters,
				  DWORD cbBuf, LPDWORD lpdwNeeded,
				  LPDWORD lpdwReturned, BOOL unicode)

{
    HKEY hkeyPrinters, hkeyPrinter;
    WCHAR PrinterName[255];
    DWORD needed = 0, number = 0;
    DWORD used, i, left;
    PBYTE pi, buf;

    if(lpbPrinters)
        memset(lpbPrinters, 0, cbBuf);
    if(lpdwReturned)
        *lpdwReturned = 0;
    if(lpdwNeeded)
        *lpdwNeeded = 0;

    /* PRINTER_ENUM_DEFAULT is only supported under win9x, we behave like NT */
    if(dwType == PRINTER_ENUM_DEFAULT)
	return TRUE;

    if (dwType & PRINTER_ENUM_CONNECTIONS) {
        FIXME("We dont handle PRINTER_ENUM_CONNECTIONS\n");
	dwType &= ~PRINTER_ENUM_CONNECTIONS; /* we dont handle that */
        if(!dwType) return TRUE;
    }

    if (!((dwType & PRINTER_ENUM_LOCAL) || (dwType & PRINTER_ENUM_NAME))) {
        FIXME("dwType = %08lx\n", dwType);
	SetLastError(ERROR_INVALID_FLAGS);
	return FALSE;
    }

    if(RegCreateKeyA(HKEY_LOCAL_MACHINE, Printers, &hkeyPrinters) !=
       ERROR_SUCCESS) {
        ERR("Can't create Printers key\n");
	return FALSE;
    }

    if(RegQueryInfoKeyA(hkeyPrinters, NULL, NULL, NULL, &number, NULL, NULL,
			NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
        RegCloseKey(hkeyPrinters);
	ERR("Can't query Printers key\n");
	return FALSE;
    }
    TRACE("Found %ld printers\n", number);

    switch(dwLevel) {
    case 1:
        RegCloseKey(hkeyPrinters);
	if (lpdwReturned)
	    *lpdwReturned = number;
	return TRUE;

    case 2:
        used = number * sizeof(PRINTER_INFO_2W);
	break;
    case 4:
        used = number * sizeof(PRINTER_INFO_4W);
	break;
    case 5:
        used = number * sizeof(PRINTER_INFO_5W);
	break;

    default:
        SetLastError(ERROR_INVALID_LEVEL);
	RegCloseKey(hkeyPrinters);
	return FALSE;
    }
    pi = (used <= cbBuf) ? lpbPrinters : NULL;

    for(i = 0; i < number; i++) {
        if(RegEnumKeyW(hkeyPrinters, i, PrinterName, sizeof(PrinterName)) !=
	   ERROR_SUCCESS) {
	    ERR("Can't enum key number %ld\n", i);
	    RegCloseKey(hkeyPrinters);
	    return FALSE;
	}
	TRACE("Printer %ld is %s\n", i, debugstr_w(PrinterName));
	if(RegOpenKeyW(hkeyPrinters, PrinterName, &hkeyPrinter) !=
	   ERROR_SUCCESS) {
	    ERR("Can't open key %s\n", debugstr_w(PrinterName));
	    RegCloseKey(hkeyPrinters);
	    return FALSE;
	}

	if(cbBuf > used) {
	    buf = lpbPrinters + used;
	    left = cbBuf - used;
	} else {
	    buf = NULL;
	    left = 0;
	}

	switch(dwLevel) {
	case 2:
	    WINSPOOL_GetPrinter_2(hkeyPrinter, (PRINTER_INFO_2W *)pi, buf,
				  left, &needed, unicode);
	    used += needed;
	    if(pi) pi += sizeof(PRINTER_INFO_2W);
	    break;
	case 4:
	    WINSPOOL_GetPrinter_4(hkeyPrinter, (PRINTER_INFO_4W *)pi, buf,
				  left, &needed, unicode);
	    used += needed;
	    if(pi) pi += sizeof(PRINTER_INFO_4W);
	    break;
	case 5:
	    WINSPOOL_GetPrinter_5(hkeyPrinter, (PRINTER_INFO_5W *)pi, buf,
				  left, &needed, unicode);
	    used += needed;
	    if(pi) pi += sizeof(PRINTER_INFO_5W);
	    break;
	default:
	    ERR("Shouldn't be here!\n");
	    RegCloseKey(hkeyPrinter);
	    RegCloseKey(hkeyPrinters);
	    return FALSE;
	}
	RegCloseKey(hkeyPrinter);
    }
    RegCloseKey(hkeyPrinters);

    if(lpdwNeeded)
        *lpdwNeeded = used;

    if(used > cbBuf) {
        if(lpbPrinters)
	    memset(lpbPrinters, 0, cbBuf);
	SetLastError(ERROR_INSUFFICIENT_BUFFER);
	return FALSE;
    }
    if(lpdwReturned)
        *lpdwReturned = number;
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}


/******************************************************************
 *              EnumPrintersA        [WINSPOOL.@]
 *
 */
BOOL WINAPI EnumPrintersA(DWORD dwType, LPSTR lpszName,
			  DWORD dwLevel, LPBYTE lpbPrinters,
			  DWORD cbBuf, LPDWORD lpdwNeeded,
			  LPDWORD lpdwReturned)
{
    BOOL ret;
    LPWSTR pwstrNameW = HEAP_strdupAtoW(GetProcessHeap(),0,lpszName);

    ret = WINSPOOL_EnumPrinters(dwType, pwstrNameW, dwLevel, lpbPrinters, cbBuf,
				lpdwNeeded, lpdwReturned, FALSE);
    HeapFree(GetProcessHeap(),0,pwstrNameW);
    return ret;
}


/*****************************************************************************
 * Name      : BOOL ClosePrinter
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [09.07.98 13:39:08]
 *****************************************************************************/

BOOL WIN32API ClosePrinter(HANDLE hPrinter)
{
#ifdef __WIN32OS2__
    /*
     * Validate handle and get instance data.
     */
    POPENPRINTER    p = openprinterFind(hPrinter);
    if (!p)
    {
        dprintf(("WINSPOOL: ClosePrinter Invalid handle %d\n", hPrinter));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    dprintf(("WINSPOOL: ClosePrinter(%d) name=%ls\n", hPrinter, p->pwszPrinterName));

    /*
     * Close any open OS/2 spool file.
     */
    if (p->hspl)
    {
        dprintf(("WINSPOOL: ClosePrinter closing OS/2 spool file %#x\n", p->hspl));
        if (!OSLibSplQmClose(p->hspl))
           DebugAssertFailed(("OSLibSplQmClose(%#x) failed!!!\n", p->hspl));
        p->hspl = 0;
    }
    openprinterDelete(p);
    return TRUE;
#else
    int i = (int)hPrinter;

    TRACE("Handle %p\n", hPrinter);

    if ((i <= 0) || (i > nb_printers)) return FALSE;
    HeapFree( GetProcessHeap(), 0, printer_array[i - 1] );
    printer_array[i - 1] = NULL;
    return TRUE;
#endif
}


/**
 * Opens a printer.
 *
 * @returns Success indicator, last error set on failure.
 * @param   lpPrinterName   Pointer to printer name.
 * @param   phPrinter       Where to store the handle to the opned printer instance.
 * @param   pDefault        Printer defaults. (Current ignored)
 * @status  partially implemented.
 * @author  Wine???
 */
BOOL WIN32API OpenPrinterA(LPSTR lpPrinterName, HANDLE * phPrinter,
                           PRINTER_DEFAULTSA * pDefault)
{
    LPWSTR lpPrinterNameW = HEAP_strdupAtoW(GetProcessHeap(),0,lpPrinterName);
    PRINTER_DEFAULTSW DefaultW, *pDefaultW = NULL;
    BOOL ret;

    if(pDefault) {
        DefaultW.pDatatype = HEAP_strdupAtoW(GetProcessHeap(), 0,
					     pDefault->pDatatype);
	DefaultW.pDevMode = DEVMODEdupAtoW(GetProcessHeap(),
					   pDefault->pDevMode);
	DefaultW.DesiredAccess = pDefault->DesiredAccess;
	pDefaultW = &DefaultW;
    }
    ret = OpenPrinterW(lpPrinterNameW, phPrinter, pDefaultW);
    if(pDefault) {
        HeapFree(GetProcessHeap(), 0, DefaultW.pDatatype);
	HeapFree(GetProcessHeap(), 0, DefaultW.pDevMode);
    }
    HeapFree(GetProcessHeap(), 0, lpPrinterNameW);
    return ret;
}


/*****************************************************************************
 * Name      : LONG DocumentPropertiesA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Markus Montkowski [09.07.98 13:39:08]
 *****************************************************************************/

LONG WIN32API DocumentPropertiesA(HWND hWnd, HANDLE hPrinter,
                                  LPSTR pDeviceName,
                                  LPDEVMODEA pDevModeOutput,
                                  LPDEVMODEA pDevModeInput,
                                  DWORD fMode)
{
    LPSTR lpName = pDeviceName;
    LONG  ret;

    dprintf(("DocumentPropertiesA(%p,%p,%s,%p,%p,%ld)\n",
	hWnd,hPrinter,pDeviceName,pDevModeOutput,pDevModeInput,fMode
    ));

    if (hWnd)
    {
        char szPMQueue[256];
        if(SplQueryPMQueueName(pDeviceName, szPMQueue, sizeof(szPMQueue)) == TRUE) {
            ret = OSLibShowPrinterDialog(hWnd, szPMQueue);
        }
    }

    if(!pDeviceName) {
        LPCWSTR lpNameW = WINSPOOL_GetOpenedPrinter(hPrinter);
        if(!lpNameW) {
		ERR("no name from hPrinter?\n");
		return -1;
	}
	lpName = HEAP_strdupWtoA(GetProcessHeap(),0,lpNameW);
    }

#if 0
    ret = GDI_CallExtDeviceMode16(hWnd, pDevModeOutput, lpName, "LPT1:",
				  pDevModeInput, NULL, fMode);
#endif

    switch (fMode & ~DM_PROMPT) {
    case 0:
            /* TODO - Printer driver specific data */
            ret = (sizeof(DEVMODEA));
            break;

    case DM_IN_BUFFER:
    case DM_IN_BUFFER | DM_OUT_BUFFER:
    {
             HKEY hkey,hkeyPrinters,hkeyPrinter;

             if(RegCreateKeyA(HKEY_LOCAL_MACHINE, Printers, &hkeyPrinters) !=
                              ERROR_SUCCESS)
             {
                ERR("Can't open Printers key\n");
                SetLastError(ERROR_INVALID_PRINTER_NAME); /* ? */
              	ret = -1;
                break;
             }

             if(RegOpenKeyA(hkeyPrinters, lpName, &hkeyPrinter) != ERROR_SUCCESS)
             {
                ERR("Can't find opened printer %s in registry\n", lpName);
              	RegCloseKey(hkeyPrinters);
                SetLastError(ERROR_INVALID_PRINTER_NAME); /* ? */
              	ret = -1;
                break;
             }

             WINSPOOL_SetDevModeFromReg(hkeyPrinter, Default_DevModeW, (LPBYTE)pDevModeInput,
                                        sizeof(DEVMODEA));
             if (pDevModeOutput) {
                memcpy(pDevModeOutput, pDevModeInput, sizeof(DEVMODEA));
             }
             RegCloseKey(hkeyPrinter);
             RegCloseKey(hkeyPrinters);
             ret = IDOK;
             break;
    }

    case DM_OUT_BUFFER:
            if(pDevModeOutput)
            {
                PRINTER_INFO_2A *pInfo;
                DWORD needed;

                GetPrinterA(hPrinter, 2, NULL, 0, &needed);
                pInfo = (PRINTER_INFO_2A*)HeapAlloc(GetProcessHeap(),0,needed);
                if(GetPrinterA(hPrinter, 2, (LPBYTE)pInfo, needed, &needed) == FALSE) {
                    dprintf(("GetPrinterA failed"));
                    ret = -1;
                    break;
                }
                if (pInfo->pDevMode) {
                  memcpy(pDevModeOutput, pInfo->pDevMode, sizeof(DEVMODEA));
                }
                HeapFree(GetProcessHeap(),0,pInfo);
            }
            ret = IDOK;
            break;

    default:
        dprintf(("Unsupported fMode = %d",fMode));
        ret = -1;
        break;
    }

    if(!pDeviceName)
        HeapFree(GetProcessHeap(),0,lpName);
    return ret;
}



/***********************************************************
 *             PRINTER_INFO_2AtoW
 * Creates a unicode copy of PRINTER_INFO_2A on heap
 */
static LPPRINTER_INFO_2W PRINTER_INFO_2AtoW(HANDLE heap, LPPRINTER_INFO_2A piA)
{
    LPPRINTER_INFO_2W piW;
    if(!piA) return NULL;
    piW = (LPPRINTER_INFO_2W) HeapAlloc(heap, 0, sizeof(*piW));
    memcpy(piW, piA, sizeof(*piW)); /* copy everything first */
    piW->pServerName = HEAP_strdupAtoW(heap, 0, piA->pServerName);
    piW->pPrinterName = HEAP_strdupAtoW(heap, 0, piA->pPrinterName);
    piW->pShareName = HEAP_strdupAtoW(heap, 0, piA->pShareName);
    piW->pPortName = HEAP_strdupAtoW(heap, 0, piA->pPortName);
    piW->pDriverName = HEAP_strdupAtoW(heap, 0, piA->pDriverName);
    piW->pComment = HEAP_strdupAtoW(heap, 0, piA->pComment);
    piW->pLocation = HEAP_strdupAtoW(heap, 0, piA->pLocation);
    piW->pDevMode = DEVMODEdupAtoW(heap, piA->pDevMode);
    piW->pSepFile = HEAP_strdupAtoW(heap, 0, piA->pSepFile);
    piW->pPrintProcessor = HEAP_strdupAtoW(heap, 0, piA->pPrintProcessor);
    piW->pDatatype = HEAP_strdupAtoW(heap, 0, piA->pDatatype);
    piW->pParameters = HEAP_strdupAtoW(heap, 0, piA->pParameters);
    return piW;
}

/***********************************************************
 *       FREE_PRINTER_INFO_2W
 * Free PRINTER_INFO_2W and all strings
 */
static void FREE_PRINTER_INFO_2W(HANDLE heap, LPPRINTER_INFO_2W piW)
{
    if(!piW) return;

    HeapFree(heap,0,piW->pServerName);
    HeapFree(heap,0,piW->pPrinterName);
    HeapFree(heap,0,piW->pShareName);
    HeapFree(heap,0,piW->pPortName);
    HeapFree(heap,0,piW->pDriverName);
    HeapFree(heap,0,piW->pComment);
    HeapFree(heap,0,piW->pLocation);
    HeapFree(heap,0,piW->pDevMode);
    HeapFree(heap,0,piW->pSepFile);
    HeapFree(heap,0,piW->pPrintProcessor);
    HeapFree(heap,0,piW->pDatatype);
    HeapFree(heap,0,piW->pParameters);
    HeapFree(heap,0,piW);
    return;
}


/*****************************************************************************
 * Name      : HANDLE AddPrinterA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             DWORD Level  printer info. structure level
 *             LPBYTE pPrinter  pointer to structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:18:56]
 *****************************************************************************/

HANDLE WIN32API AddPrinterA(LPSTR pName, DWORD Level, LPBYTE pPrinter)
{
    WCHAR *pNameW = NULL;
    PRINTER_INFO_2W *piW;
    PRINTER_INFO_2A *piA = (PRINTER_INFO_2A*)pPrinter;
    HANDLE ret;

    TRACE("(%s,%ld,%p): stub\n", debugstr_a(pName), Level, pPrinter);
    if(Level != 2) {
        ERR("Level = %ld, unsupported!\n", Level);
	SetLastError(ERROR_INVALID_LEVEL);
	return 0;
    }
    if (pName)
      pNameW = HEAP_strdupAtoW(GetProcessHeap(), 0, pName);
    piW = PRINTER_INFO_2AtoW(GetProcessHeap(), piA);

    ret = AddPrinterW(pNameW, Level, (LPBYTE)piW);

    FREE_PRINTER_INFO_2W(GetProcessHeap(), piW);
    HeapFree(GetProcessHeap(),0,pNameW);
    return ret;
}


/*****************************************************************************
 * Name      : HANDLE AddPrinterW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             DWORD Level  printer info. structure level
 *             LPBYTE pPrinter  pointer to structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:18:56]
 *****************************************************************************/

HANDLE WIN32API AddPrinterW(LPWSTR pName, DWORD Level, LPBYTE pPrinter)
{
    PRINTER_INFO_2W *pi = (PRINTER_INFO_2W *) pPrinter;
    LPDEVMODEA dmA;
    LPDEVMODEW dmW;
    HANDLE retval;
    HKEY hkeyPrinter, hkeyPrinters, hkeyDriver, hkeyDrivers;
    LONG size;

    TRACE("(%s,%ld,%p)\n", debugstr_w(pName), Level, pPrinter);

    if(pName != NULL) {
        ERR("pName = %s - unsupported\n", debugstr_w(pName));
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
    }
    if(Level != 2) {
        ERR("Level = %ld, unsupported!\n", Level);
	SetLastError(ERROR_INVALID_LEVEL);
	return 0;
    }
#if 0
    // SvL: OS/2 printer names can be up to 48 characters, whereas this limit
    //      is 32 chars. We don't actually copy the full printer name into the
    //      dmDeviceName string, so it's fairly safe to remove this check.
    //      (Acrobat Reader Defect 937)
    if (strlenW(pi->pPrinterName) >= CCHDEVICENAME) {
	ERR("Printername %s must not exceed length of DEVMODE.dmDeviceName !\n",
		debugstr_w(pi->pPrinterName)
	);
	SetLastError(ERROR_INVALID_LEVEL);
	return 0;
    }
#endif
    if(!pPrinter) {
        SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
    }
    if(RegCreateKeyA(HKEY_LOCAL_MACHINE, Printers, &hkeyPrinters) !=
       ERROR_SUCCESS) {
        ERR("Can't create Printers key\n");
	return 0;
    }
    if(!RegOpenKeyW(hkeyPrinters, pi->pPrinterName, &hkeyPrinter)) {
	if (!RegQueryValueA(hkeyPrinter,"Attributes",NULL,NULL)) {
	    SetLastError(ERROR_PRINTER_ALREADY_EXISTS);
	    RegCloseKey(hkeyPrinter);
	    RegCloseKey(hkeyPrinters);
	    return 0;
	}
	RegCloseKey(hkeyPrinter);
    }
    hkeyDrivers = WINSPOOL_OpenDriverReg( NULL, TRUE);
    if(!hkeyDrivers) {
        ERR("Can't create Drivers key\n");
	RegCloseKey(hkeyPrinters);
	return 0;
    }
    if(RegOpenKeyW(hkeyDrivers, pi->pDriverName, &hkeyDriver) !=
       ERROR_SUCCESS) {
        WARN("Can't find driver %s\n", debugstr_w(pi->pDriverName));
	RegCloseKey(hkeyPrinters);
	RegCloseKey(hkeyDrivers);
	SetLastError(ERROR_UNKNOWN_PRINTER_DRIVER);
	return 0;
    }
    RegCloseKey(hkeyDriver);
    RegCloseKey(hkeyDrivers);

    if(lstrcmpiW(pi->pPrintProcessor, WinPrintW)) {  /* FIXME */
        FIXME("Can't find processor %s\n", debugstr_w(pi->pPrintProcessor));
	SetLastError(ERROR_UNKNOWN_PRINTPROCESSOR);
	RegCloseKey(hkeyPrinters);
	return 0;
    }

    if(RegCreateKeyW(hkeyPrinters, pi->pPrinterName, &hkeyPrinter) !=
       ERROR_SUCCESS) {
        FIXME("Can't create printer %s\n", debugstr_w(pi->pPrinterName));
	SetLastError(ERROR_INVALID_PRINTER_NAME);
	RegCloseKey(hkeyPrinters);
	return 0;
    }
    RegSetValueExA(hkeyPrinter, "Attributes", 0, REG_DWORD,
		   (LPBYTE)&pi->Attributes, sizeof(DWORD));
    set_reg_szW(hkeyPrinter, DatatypeW, pi->pDatatype);

    /* See if we can load the driver.  We may need the devmode structure anyway
     *
     * FIXME:
     * Note that DocumentPropertiesW will briefly try to open the printer we




     * just create to find a DEVMODEA struct (it will use the WINEPS default
     * one in case it is not there, so we are ok).
     */
    size = DocumentPropertiesW(0, 0, pi->pPrinterName, NULL, NULL, 0);
    if(size < 0) {
        FIXME("DocumentPropertiesW on printer '%s' fails\n", debugstr_w(pi->pPrinterName));
	size = sizeof(DEVMODEW);
    }
#if 0
    if(pi->pDevMode)
        dmW = pi->pDevMode;
    else {
	dmW = (LPDEVMODEW) HeapAlloc(GetProcessHeap(), 0, size);
	dmW->dmSize = size;
	if (0>DocumentPropertiesW(0,0,pi->pPrinterName,dmW,NULL,DM_OUT_BUFFER)) {
	    ERR("DocumentPropertiesW on printer '%s' failed!\n", debugstr_w(pi->pPrinterName));
	    SetLastError(ERROR_UNKNOWN_PRINTER_DRIVER);
	    return 0;
	}
	/* set devmode to printer name */
	strcpyW(dmW->dmDeviceName,pi->pPrinterName);
    }

    /* Write DEVMODEA not DEVMODEW into reg.  This is what win9x does
       and we support these drivers.  NT writes DEVMODEW so somehow
       we'll need to distinguish between these when we support NT
       drivers */
    dmA = DEVMODEdupWtoA(GetProcessHeap(), dmW);
    RegSetValueExA(hkeyPrinter, "Default DevMode", 0, REG_BINARY, (LPBYTE)dmA,
		   dmA->dmSize + dmA->dmDriverExtra);
    HeapFree(GetProcessHeap(), 0, dmA);
    if(!pi->pDevMode)
        HeapFree(GetProcessHeap(), 0, dmW);
#endif
    set_reg_szW(hkeyPrinter, DescriptionW, pi->pComment);
    set_reg_szW(hkeyPrinter, LocationW, pi->pLocation);
    set_reg_szW(hkeyPrinter, NameW, pi->pPrinterName);
    set_reg_szW(hkeyPrinter, ParametersW, pi->pParameters);

    set_reg_szW(hkeyPrinter, PortW, pi->pPortName);
    set_reg_szW(hkeyPrinter, Print_ProcessorW, pi->pPrintProcessor);
    set_reg_szW(hkeyPrinter, Printer_DriverW, pi->pDriverName);
    RegSetValueExA(hkeyPrinter, "Priority", 0, REG_DWORD,
		   (LPBYTE)&pi->Priority, sizeof(DWORD));
    set_reg_szW(hkeyPrinter, Separator_FileW, pi->pSepFile);
    set_reg_szW(hkeyPrinter, Share_NameW, pi->pShareName);
    RegSetValueExA(hkeyPrinter, "StartTime", 0, REG_DWORD,
		   (LPBYTE)&pi->StartTime, sizeof(DWORD));
    RegSetValueExA(hkeyPrinter, "Status", 0, REG_DWORD,
		   (LPBYTE)&pi->Status, sizeof(DWORD));
    RegSetValueExA(hkeyPrinter, "UntilTime", 0, REG_DWORD,
		   (LPBYTE)&pi->UntilTime, sizeof(DWORD));

    RegCloseKey(hkeyPrinter);
    RegCloseKey(hkeyPrinters);
    if(!OpenPrinterW(pi->pPrinterName, &retval, NULL)) {
        ERR("OpenPrinter failing\n");
	return 0;
    }
    return retval;
}



/*****************************************************************************
 * Name      : BOOL AddPrinterDriverA
 * Purpose   :
 * Parameters: LPSTR pName  pointer to server name
 *             DWORD Level  printer info. structure level
 *             LPBYTE pDriverInfo  pointer to printer info. structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:20:04]
 *****************************************************************************/

BOOL WIN32API AddPrinterDriverA(LPSTR pName, DWORD level, LPBYTE pDriverInfo)
{
    DRIVER_INFO_3A di3;
    HKEY hkeyDrivers, hkeyName;

    TRACE("(%s,%ld,%p)\n",debugstr_a(pName),level,pDriverInfo);

    if(level != 2 && level != 3) {
        SetLastError(ERROR_INVALID_LEVEL);
	return FALSE;
    }
    if(pName != NULL) {
        FIXME("pName= %s - unsupported\n", debugstr_a(pName));
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
    }
    if(!pDriverInfo) {
        WARN("pDriverInfo == NULL\n");
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
    }

    if(level == 3)
        di3 = *(DRIVER_INFO_3A *)pDriverInfo;
    else {
        memset(&di3, 0, sizeof(di3));
        *(DRIVER_INFO_2A *)&di3 = *(DRIVER_INFO_2A *)pDriverInfo;
    }

    if(!di3.pName || !di3.pDriverPath || !di3.pConfigFile ||
       !di3.pDataFile) {
        SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
    }
    if(!di3.pDefaultDataType) di3.pDefaultDataType = (LPSTR)"";
    if(!di3.pDependentFiles) di3.pDependentFiles = (LPSTR)"\0";
    if(!di3.pHelpFile) di3.pHelpFile = (LPSTR)"";
    if(!di3.pMonitorName) di3.pMonitorName = (LPSTR)"";

    hkeyDrivers = WINSPOOL_OpenDriverReg(di3.pEnvironment, FALSE);

    if(!hkeyDrivers) {
        ERR("Can't create Drivers key\n");
	return FALSE;
    }

    if(level == 2) { /* apparently can't overwrite with level2 */
        if(RegOpenKeyA(hkeyDrivers, di3.pName, &hkeyName) == ERROR_SUCCESS) {
	    RegCloseKey(hkeyName);
	    RegCloseKey(hkeyDrivers);
	    WARN("Trying to create existing printer driver %s\n", debugstr_a(di3.pName));
	    SetLastError(ERROR_PRINTER_DRIVER_ALREADY_INSTALLED);
	    return FALSE;
	}
    }
    if(RegCreateKeyA(hkeyDrivers, di3.pName, &hkeyName) != ERROR_SUCCESS) {
	RegCloseKey(hkeyDrivers);
	ERR("Can't create Name key\n");
	return FALSE;
    }
    RegSetValueExA(hkeyName, "Configuration File", 0, REG_SZ, (LPBYTE)di3.pConfigFile,
		   strlen(di3.pConfigFile)+1);
    RegSetValueExA(hkeyName, "Data File", 0, REG_SZ, (LPBYTE)di3.pDataFile, strlen(di3.pDataFile)+1);
    RegSetValueExA(hkeyName, "Driver", 0, REG_SZ, (LPBYTE)di3.pDriverPath, strlen(di3.pDriverPath)+1);
    RegSetValueExA(hkeyName, "Version", 0, REG_DWORD, (LPBYTE)&di3.cVersion,
		   sizeof(DWORD));
    RegSetValueExA(hkeyName, "Datatype", 0, REG_SZ, (LPBYTE)di3.pDefaultDataType, strlen(di3.pDefaultDataType)+1);
    RegSetValueExA(hkeyName, "Dependent Files", 0, REG_MULTI_SZ,
		   (LPBYTE)di3.pDependentFiles, strlen(di3.pDependentFiles)+1);
    RegSetValueExA(hkeyName, "Help File", 0, REG_SZ, (LPBYTE)di3.pHelpFile, strlen(di3.pHelpFile)+1);
    RegSetValueExA(hkeyName, "Monitor", 0, REG_SZ, (LPBYTE)di3.pMonitorName, strlen(di3.pMonitorName)+1);
    RegCloseKey(hkeyName);
    RegCloseKey(hkeyDrivers);

    return TRUE;
}


/*****************************************************************************
 * Name      : BOOL AddPrinterDriverW
 * Purpose   :
 * Parameters: LPWSTR pName  pointer to server name
 *             DWORD Level  printer info. structure level
 *             LPBYTE pDriverInfo  pointer to printer info. structure
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:20:04]
 *****************************************************************************/

BOOL WIN32API AddPrinterDriverW(LPWSTR pName, DWORD Level, LPBYTE pDriverInfo)
{
  dprintf(("WINSPOOL: AddPrinterDriverW not implemented\n"));
  return (FALSE);
}

/*****************************************************************************
 * Name      : BOOL DeletePrinter
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer object
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:30:50]
 *****************************************************************************/

BOOL WIN32API DeletePrinter(HANDLE hPrinter)
{
    LPCWSTR lpNameW = WINSPOOL_GetOpenedPrinter(hPrinter);
    HKEY hkeyPrinters;

    if(!lpNameW) return FALSE;

    if(RegOpenKeyA(HKEY_LOCAL_MACHINE, Printers, &hkeyPrinters) == ERROR_SUCCESS) {
        SHDeleteKeyW(hkeyPrinters, lpNameW);
	RegCloseKey(hkeyPrinters);
        return TRUE;
    }
    else {
        dprintf(("Can't open Printers key"));
	return FALSE;
    }
}


/*****************************************************************************
 * Name      : DWORD DeviceCapabilitiesA
 * Purpose   :
 * Parameters: LPCSTR pDevice  pointer to a printer-name string
 *             LPCSTR pPort  pointer to a port-name string
 *             WORD fwCapability  device capability to query
 *             LPSTR pOutput  pointer to the output
 *             CONST DEVMODE *pDevMode  pointer to structure with device data
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:27:08]
 *****************************************************************************/

INT WIN32API DeviceCapabilitiesA(LPCSTR pDevice,
                                 LPCSTR pPort,
                                 WORD fwCapability,
                                 LPSTR pOutput,
                                 DEVMODEA * pDevMode)
{
    char szPMQueue[256];

    dprintf(("WINSPOOL: DeviceCapabilitiesA %s %s %x %x %x", pDevice, pPort, fwCapability, pOutput, pDevMode));

    //Get the PM Queue name corresponding to the printer device
    if(SplQueryPMQueueName((LPSTR)pDevice, szPMQueue, sizeof(szPMQueue)) == TRUE) {
        return O32_DeviceCapabilities(szPMQueue, pPort, fwCapability, pOutput, pDevMode);
    }
    return -1;
}


/*****************************************************************************
 *          DeviceCapabilitiesW        [WINSPOOL.152]
 *
 * Call DeviceCapabilitiesA since we later call 16bit stuff anyway
 *
 */
INT WIN32API DeviceCapabilitiesW(LPCWSTR pDevice, LPCWSTR pPort,
                                 WORD fwCapability, LPWSTR pOutput,
                                 CONST DEVMODEW * pDevMode)
{
    LPDEVMODEA dmA = DEVMODEdupWtoA(GetProcessHeap(), pDevMode);
    LPSTR pDeviceA = HEAP_strdupWtoA(GetProcessHeap(),0,pDevice);
    LPSTR pPortA = HEAP_strdupWtoA(GetProcessHeap(),0,pPort);
    INT ret;

    if(pOutput && (fwCapability == DC_BINNAMES ||
           fwCapability == DC_FILEDEPENDENCIES ||
           fwCapability == DC_PAPERNAMES)) {
      /* These need A -> W translation */
        INT size = 0, i;
    LPSTR pOutputA;
        ret = DeviceCapabilitiesA(pDeviceA, pPortA, fwCapability, NULL,
                  dmA);
    if(ret == -1)
        return ret;
    switch(fwCapability) {
    case DC_BINNAMES:
        size = 24;
        break;
    case DC_PAPERNAMES:
    case DC_FILEDEPENDENCIES:
        size = 64;
        break;
    }
    pOutputA = (LPSTR)HeapAlloc(GetProcessHeap(), 0, size * ret);
    ret = DeviceCapabilitiesA(pDeviceA, pPortA, fwCapability, pOutputA,
                  dmA);
    for(i = 0; i < ret; i++)
        MultiByteToWideChar(CP_ACP, 0, pOutputA + (i * size), -1,
                pOutput + (i * size), size);
    HeapFree(GetProcessHeap(), 0, pOutputA);
    } else {
        ret = DeviceCapabilitiesA(pDeviceA, pPortA, fwCapability,
                  (LPSTR)pOutput, dmA);
    }
    HeapFree(GetProcessHeap(),0,pPortA);
    HeapFree(GetProcessHeap(),0,pDeviceA);
    HeapFree(GetProcessHeap(),0,dmA);
    return ret;
}




/******************************************************************
 *              EnumPrintersW        [WINSPOOL.@]
 *
 *    Enumerates the available printers, print servers and print
 *    providers, depending on the specified flags, name and level.
 *
 * RETURNS:
 *
 *    If level is set to 1:
 *      Not implemented yet!
 *      Returns TRUE with an empty list.
 *
 *    If level is set to 2:
 *		Possible flags: PRINTER_ENUM_CONNECTIONS, PRINTER_ENUM_LOCAL.
 *      Returns an array of PRINTER_INFO_2 data structures in the
 *      lpbPrinters buffer. Note that according to MSDN also an
 *      OpenPrinter should be performed on every remote printer.
 *
 *    If level is set to 4 (officially WinNT only):
 *		Possible flags: PRINTER_ENUM_CONNECTIONS, PRINTER_ENUM_LOCAL.
 *      Fast: Only the registry is queried to retrieve printer names,
 *      no connection to the driver is made.
 *      Returns an array of PRINTER_INFO_4 data structures in the
 *      lpbPrinters buffer.
 *
 *    If level is set to 5 (officially WinNT4/Win9x only):
 *      Fast: Only the registry is queried to retrieve printer names,
 *      no connection to the driver is made.
 *      Returns an array of PRINTER_INFO_5 data structures in the
 *      lpbPrinters buffer.
 *
 *    If level set to 3 or 6+:
 *	    returns zero (failure!)
 *
 *    Returns nonzero (TRUE) on success, or zero on failure, use GetLastError
 *    for information.
 *
 * BUGS:
 *    - Only PRINTER_ENUM_LOCAL and PRINTER_ENUM_NAME are implemented.
 *    - Only levels 2, 4 and 5 are implemented at the moment.
 *    - 16-bit printer drivers are not enumerated.
 *    - Returned amount of bytes used/needed does not match the real Windoze
 *      implementation (as in this implementation, all strings are part
 *      of the buffer, whereas Win32 keeps them somewhere else)
 *    - At level 2, EnumPrinters should also call OpenPrinter for remote printers.
 *
 * NOTE:
 *    - In a regular Wine installation, no registry settings for printers
 *      exist, which makes this function return an empty list.
 */
BOOL  WINAPI EnumPrintersW(
		DWORD dwType,        /* [in] Types of print objects to enumerate */
                LPWSTR lpszName,     /* [in] name of objects to enumerate */
	        DWORD dwLevel,       /* [in] type of printer info structure */
                LPBYTE lpbPrinters,  /* [out] buffer which receives info */
		DWORD cbBuf,         /* [in] max size of buffer in bytes */
		LPDWORD lpdwNeeded,  /* [out] pointer to var: # bytes used/needed */
		LPDWORD lpdwReturned /* [out] number of entries returned */
		)
{
    return WINSPOOL_EnumPrinters(dwType, lpszName, dwLevel, lpbPrinters, cbBuf,
				 lpdwNeeded, lpdwReturned, TRUE);
}




/*****************************************************************************
 * Name      : BOOL GetPrinterA
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer of interest
 *             DWORD Level  version of printer info data structure
 *             LPBYTE pPrinter  pointer to array of bytes that receives printer info. structure
 *             DWORD cbBuf  size, in bytes, of array of bytes
 *             LPDWORD pcbNeeded  pointer to variable with count of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:45:36]
 *****************************************************************************/

BOOL WIN32API GetPrinterA(
              HANDLE hPrinter,
              DWORD Level,
              LPBYTE pPrinter,
              DWORD cbBuf,
              LPDWORD pcbNeeded)
{
    return WINSPOOL_GetPrinter(hPrinter, Level, pPrinter, cbBuf, pcbNeeded,
			       FALSE);
}


/*****************************************************************************
 * Name      : BOOL GetPrinterW
 * Purpose   :
 * Parameters: HANDLE hPrinter  handle to printer of interest
 *             DWORD Level  version of printer info data structure
 *             LPBYTE pPrinter  pointer to array of bytes that receives printer info. structure
 *             DWORD cbBuf  size, in bytes, of array of bytes
 *             LPDWORD pcbNeeded  pointer to variable with count of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:45:36]
 *****************************************************************************/

BOOL WIN32API GetPrinterW(
              HANDLE hPrinter,
              DWORD Level,
              LPBYTE pPrinter,
              DWORD cbBuf,
              LPDWORD pcbNeeded)
{
	return WINSPOOL_GetPrinter(hPrinter, Level, pPrinter, cbBuf, pcbNeeded,
				   TRUE);
}



/*****************************************************************************
 * Name      : BOOL GetPrinterDriverA
 * Purpose   :
 * Parameters: HANDLE hPrinter  printer object
 *             LPSTR pEnvironment  address of environment
 *             DWORD Level  structure level
 *             LPBYTE pDriverInfo  address of structure array
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:47:01]
 *****************************************************************************/

BOOL WIN32API GetPrinterDriverA(
              HANDLE hPrinter,
              LPSTR pEnvironment,
              DWORD Level,
              LPBYTE pDriverInfo,
              DWORD cbBuf,
              LPDWORD pcbNeeded)
{
    BOOL ret;
    LPWSTR pEnvW = HEAP_strdupAtoW(GetProcessHeap(),0,pEnvironment);
    ret = WINSPOOL_GetPrinterDriver(hPrinter, pEnvW, Level, pDriverInfo,
				    cbBuf, pcbNeeded, FALSE);
    HeapFree(GetProcessHeap(),0,pEnvW);
    return ret;
}


/*****************************************************************************
 * Name      : BOOL GetPrinterDriverW
 * Purpose   :
 * Parameters: HANDLE hPrinter  printer object
 *             LPWSTR pEnvironment  address of environment
 *             DWORD Level  structure level
 *             LPBYTE pDriverInfo  address of structure array
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:47:01]
 *****************************************************************************/

BOOL WIN32API GetPrinterDriverW(
              HANDLE hPrinter,
              LPWSTR pEnvironment,
              DWORD Level,
              LPBYTE pDriverInfo,
              DWORD cbBuf,
              LPDWORD pcbNeeded)
{
    return WINSPOOL_GetPrinterDriver(hPrinter, pEnvironment, Level, pDriverInfo,
                    cbBuf, pcbNeeded, TRUE);
}


/*****************************************************************************
 * Name      : BOOL GetPrinterDriverDirectoryA
 * Purpose   :
 * Parameters: LPSTR pName  address of server name
 *             LPSTR pEnvironment  address of environment
 *             DWORD Level  address of structure
 *             LPBYTE pDriverDirectory  address of structure array that receives path
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:47:16]
 *****************************************************************************/

BOOL WIN32API GetPrinterDriverDirectoryA(
              LPSTR pName,
              LPSTR pEnvironment,
              DWORD Level,
              LPBYTE pDriverDirectory,
              DWORD cbBuf,
              LPDWORD pcbNeeded)
{
    DWORD needed;

    TRACE("(%s, %s, %ld, %p, %ld, %p)\n", pName, pEnvironment, Level,
	  pDriverDirectory, cbBuf, pcbNeeded);
    if(pName != NULL) {
        FIXME("pName = `%s' - unsupported\n", pName);
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
    }
    if(pEnvironment != NULL) {
        FIXME("pEnvironment = `%s' - unsupported\n", pEnvironment);
	SetLastError(ERROR_INVALID_ENVIRONMENT);
	return FALSE;
    }
    if(Level != 1)  /* win95 ignores this so we just carry on */
        WARN("Level = %ld - assuming 1\n", Level);

    /* FIXME should read from registry */
    needed = GetSystemDirectoryA((LPSTR)pDriverDirectory, cbBuf);
    needed++;
    if(pcbNeeded)
        *pcbNeeded = needed;
    if(needed > cbBuf) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
	return FALSE;
    }
    return TRUE;
}


/*****************************************************************************
 * Name      : BOOL GetPrinterDriverDirectoryW
 * Purpose   :
 * Parameters: LPWSTR pName  address of server name
 *             LPWSTR pEnvironment  address of environment
 *             DWORD Level  address of structure
 *             LPBYTE pDriverDirectory  address of structure array that receives path
 *             DWORD cbBuf  size, in bytes, of array
 *             LPDWORD pcbNeeded  address of variable with number of bytes retrieved (or required)
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Stub Generated through PE2LX Stubwizard 0.02 from Markus Montkowski
 *
 * Author    : Markus Montkowski [09.07.98 14:47:16]
 *****************************************************************************/

BOOL WIN32API GetPrinterDriverDirectoryW(
              LPWSTR pName,
              LPWSTR pEnvironment,
              DWORD Level,
              LPBYTE pDriverDirectory,
              DWORD cbBuf,
              LPDWORD pcbNeeded)
{
    DWORD needed;

	TRACE("(%s, %s, %ld, %p, %ld, %p)\n", debugstr_w(pName), pEnvironment, Level,
	  pDriverDirectory, cbBuf, pcbNeeded);
	if(pName != NULL) {
		FIXME("pName = `%s' - unsupported\n", debugstr_w(pName));
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
	}
	if(pEnvironment != NULL) {
		FIXME("pEnvironment = `%s' - unsupported\n", debugstr_w(pEnvironment));
	SetLastError(ERROR_INVALID_ENVIRONMENT);
	return FALSE;
	}
	if(Level != 1)  /* win95 ignores this so we just carry on */
		WARN("Level = %ld - assuming 1\n", Level);

    /* FIXME should read from registry */
    needed = GetSystemDirectoryW((LPWSTR)pDriverDirectory, cbBuf);
    needed++;
    if(pcbNeeded)
        *pcbNeeded = needed;
    if(needed > cbBuf) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
    return FALSE;
    }
    return TRUE;
}

#ifndef __WIN32OS2__
/******************************************************************
 *  WINSPOOL_GetOpenedPrinterEntry
 *  Get the first place empty in the opened printer table
 */
static HANDLE WINSPOOL_GetOpenedPrinterEntry( LPCWSTR name )
{
    int i;

    for (i = 0; i < nb_printers; i++) if (!printer_array[i]) break;

    if (i >= nb_printers)
    {
        LPWSTR *new_array = (LPWSTR*)HeapReAlloc( GetProcessHeap(), 0, printer_array,
                                         (nb_printers + 16) * sizeof(*new_array) );
        if (!new_array) return 0;
        printer_array = new_array;
        nb_printers += 16;
    }

    if ((printer_array[i] = (WCHAR*)HeapAlloc( GetProcessHeap(), 0, (strlenW(name)+1)*sizeof(WCHAR) )) != NULL)
    {
        strcpyW( printer_array[i], name );
        return (HANDLE)(i + 1);
    }
    return 0;
}
#endif

/**
 * Opens a printer.
 *
 * @returns Success indicator, last error set on failure.
 * @param   lpPrinterName   Pointer to printer name.
 * @param   phPrinter       Where to store the handle to the opned printer instance.
 * @param   pDefault        Printer defaults. (Current ignored)
 * @status  partially implemented.
 * @author  ???
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
BOOL WIN32API OpenPrinterW(LPWSTR lpPrinterName, HANDLE *phPrinter, PRINTER_DEFAULTSW *pDefault)
{
    HKEY hkeyPrinters, hkeyPrinter;

    /*
     * Validate input.
     */
    if (!lpPrinterName)
    {
        dprintf(("WINSPOOL: OpenPrinterW:  printerName=NULL pDefault=%p returning false!\n", pDefault));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    dprintf(("WINSPOOL: OpenPrinterW: printerName='%ls' pDefault=%p\n", lpPrinterName, pDefault));

    /*
     * Check if the Printer exists.
     */
    if (RegCreateKeyA(HKEY_LOCAL_MACHINE, Printers, &hkeyPrinters) != ERROR_SUCCESS)
    {
        dprintf(("Can't create Printers key\n"));
        SetLastError(ERROR_FILE_NOT_FOUND); /* ?? */
        return FALSE;
    }

    if (    lpPrinterName[0] == '\0' /* explicitly fail on "" */
        ||  RegOpenKeyW(hkeyPrinters, lpPrinterName, &hkeyPrinter) != ERROR_SUCCESS)
    {
        dprintf(("Can't find printer '%ls' in registry\n", lpPrinterName));
        RegCloseKey(hkeyPrinters);
        SetLastError(ERROR_INVALID_PRINTER_NAME);
        return FALSE;
    }
    RegCloseKey(hkeyPrinter);
    RegCloseKey(hkeyPrinters);

    /*
     * Seems like win95 doesn't care if there is no where to store
     * the handle. We'll do the same.
     */
    if (!phPrinter) /* This seems to be what win95 does anyway */
    {
        dprintf(("WINSPOOL: OpenPrinterW: phPrinter is NULL, returning succesfully anyhow\n"));
        return TRUE;
    }

    /*
     * Create a handle for this open instance.
     */
    *phPrinter = (HANDLE) NULL;                  /* crash here */
#ifdef __WIN32OS2__
    POPENPRINTER p = openprinterNew(lpPrinterName);
    if (!p)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        dprintf(("WINSPOOL: OpenPrinterW: out of memory allocating handle\n"));
        return FALSE;
    }
    *phPrinter = p->hOpenPrinter;
#else
    *phPrinter = WINSPOOL_GetOpenedPrinterEntry( lpPrinterName );
#endif

    if (pDefault != NULL)
        dprintf(("Not handling pDefault\n"));

    dprintf(("WINSPOOL: OpenPrinterW: returning successfully *phPrinter=%d\n", *phPrinter));
    return TRUE;
}




/*********************************************************************
 *                  DocumentPropertiesW   (WINSPOOL.166)
 */
LONG WIN32API DocumentPropertiesW(
          HWND      hWnd,
          HANDLE    hPrinter,
          LPWSTR    pDeviceName,
          PDEVMODEW pDevModeOutput,
          PDEVMODEW pDevModeInput,
          DWORD     fMode)
{
    LPSTR pDeviceNameA = HEAP_strdupWtoA(GetProcessHeap(),0,pDeviceName);
    LPDEVMODEA pDevModeInputA = DEVMODEdupWtoA(GetProcessHeap(),pDevModeInput);
    LPDEVMODEA pDevModeOutputA = NULL;
    LONG ret;

    TRACE("(%p,%p,%s,%p,%p,%ld)\n",
          hWnd,hPrinter,debugstr_w(pDeviceName),pDevModeOutput,pDevModeInput,
	  fMode);
    if(pDevModeOutput) {
        ret = DocumentPropertiesA(hWnd, hPrinter, pDeviceNameA, NULL, NULL, 0);
	if(ret < 0) return ret;
	pDevModeOutputA = (LPDEVMODEA)HeapAlloc(GetProcessHeap(), 0, ret);
    }
    ret = DocumentPropertiesA(hWnd, hPrinter, pDeviceNameA, pDevModeOutputA,
			      pDevModeInputA, fMode);
    if(ret < 0) return ret;

    if(pDevModeOutput) {
        DEVMODEcpyAtoW(pDevModeOutput, pDevModeOutputA);
	HeapFree(GetProcessHeap(),0,pDevModeOutputA);
    }
    if(fMode == 0 && ret > 0)
        ret += (CCHDEVICENAME + CCHFORMNAME);
    HeapFree(GetProcessHeap(),0,pDevModeInputA);
    HeapFree(GetProcessHeap(),0,pDeviceNameA);
    return ret;
}



#ifdef __WIN32OS2__

/**
 * Start a new printer job.
 *
 * @returns Print job identifier.
 * @returns 0 on failure with last error set.
 * @param   hPrinter    Handle to the printer object.
 * @param   Level       Structure level.
 * @param   pDocInfo    Pointer to structure.
 * @status  partially implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
DWORD WIN32API StartDocPrinterA(
              HANDLE hPrinter,
              DWORD Level,
              LPBYTE pDocInfo)
{
    DWORD   rc = 0;
    dprintf(("WINSPOOL: StartDocPrinterA\n"));

    /*
     * Get printer instance data.
     */
    POPENPRINTER    pPrt = openprinterFind(hPrinter);
    if (!pPrt)
    {
        dprintf(("WINSPOOL: StartDocPrinterA: Invalid handle %d\n", hPrinter));
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }

    /*
     * Switch out based on input data.
     */
    switch (Level)
    {
        case 1:
        {
            PDOC_INFO_1A pDocInfoA = (PDOC_INFO_1A)pDocInfo;
            dprintf(("WINSPOOL: pDocName   =%s\n", pDocInfoA->pDocName));
            dprintf(("WINSPOOL: pDatatype  =%s\n", pDocInfoA->pDatatype));
            dprintf(("WINSPOOL: pOutputFile=%s\n", pDocInfoA->pOutputFile));

            /*
             * Validate input.
             *  ASSUMES: default is RAW as it is on OS/2.
             */
            if (    pDocInfoA->pDatatype
                &&  !strstr(pDocInfoA->pDatatype, "RAW"))
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                dprintf(("WINSPOOL: data type not supported\n"));
                break;
            }

            /*
             * Make sure the OS/2 spool file is opened.
             */
            int rcOs2 = openprinterOpenSpoolFile(pPrt);
            if (!rcOs2)
            {
                /*
                 * Start the document.
                 */
                if (OSLibSplQmStartDoc(pPrt->hspl, pDocInfoA->pDocName))
                {
                    rc = (DWORD)pPrt->hspl; /** @todo get the proper job id! */
                    pPrt->ulCurPage = 0; //reset current page counter
                    dprintf(("WINSPOOL: StartDocPrinterA: returning jobid %d\n", rc));
                    return rc;
                }
                else
                    SetLastError(OSLibSplWinGetLastError()); /** @todo convert error code!!!!!!!! */
            }
            else
                SetLastError(rcOs2); /** @todo convert error code!!!!!!!! */
            break;
        }

        default:
            dprintf(("WINSPOOL: level %d is not supported\n", Level));
            SetLastError(ERROR_INVALID_LEVEL); /* not verified. */
            break;
    }
    dprintf(("WINSPOOL: returning failure\n"));
    return 0;
}


/**
 * Start a new printer job.
 *
 * @returns Print job identifier.
 * @returns 0 on failure with last error set.
 * @param   hPrinter    Handle to the printer object.
 * @param   Level       Structure level.
 * @param   pDocInfo    Pointer to structure.
 * @status  partially implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
DWORD WIN32API StartDocPrinterW(
              HANDLE hPrinter,
              DWORD Level,
              LPBYTE pDocInfo)
{
    DWORD   rc = 0;
    dprintf(("WINSPOOL: StartDocPrinterW\n"));
    switch (Level)
    {
        case 1:
        {
            PDOC_INFO_1W    pDocInfoW = (PDOC_INFO_1W)pDocInfo;

            DOC_INFO_1A     DocInfoA;
            DocInfoA.pDocName    = UnicodeToAsciiString(pDocInfoW->pDocName);
            DocInfoA.pDatatype   = UnicodeToAsciiString(pDocInfoW->pDatatype);
            DocInfoA.pOutputFile = UnicodeToAsciiString(pDocInfoW->pOutputFile);
            rc = StartDocPrinterA(hPrinter, Level, (LPBYTE)&DocInfoA);
            FreeAsciiString(DocInfoA.pOutputFile);
            FreeAsciiString(DocInfoA.pDatatype);
            FreeAsciiString(DocInfoA.pDocName);
            break;
        }

        default:
            dprintf(("WINSPOOL: level %d is not supported\n", Level));
            SetLastError(ERROR_INVALID_LEVEL); /* not verified. */
            break;
    }
    dprintf(("WINSPOOL: returns %d\n", rc));
    return rc;
}


/**
 * Notifies the spooler that a new page is to be written to the
 * current printer job.
 *
 * This is an informational API without much significance I think.
 * The implementation assumes StartDocPrinterA/W() must be called first.
 *
 * @returns Success indicator. last error set on failure.
 * @param   hPrinter    Handle to printer object.
 * @status  completely implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 */
BOOL WIN32API StartPagePrinter(HANDLE hPrinter)
{
    dprintf(("WINSPOOL: StartPagePrinter: hPrinter=%d\n", hPrinter));
    /*
     * Get printer instance data.
     */
    POPENPRINTER    pPrt = openprinterFind(hPrinter);
    if (!pPrt)
    {
        dprintf(("WINSPOOL: StartPagePrinter: Invalid handle %d\n", hPrinter));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    /*
     * Not quite sure what should happen here if no StartDocPrinter was issued.
     * For now we'll just freak out.
     */
    if (!pPrt->hspl)
    {
        DebugAssertFailed(("StartPagePrinter called with no StartDocPrinter issued\n"));
        SetLastError(ERROR_INVALID_PRINTER_STATE); /** @todo test on w2k */
        return FALSE;
    }

    if (OSLibSplQmNewPage(pPrt->hspl, pPrt->ulCurPage + 1))
    {
        pPrt->ulCurPage++;
        dprintf(("WINSPOOL: StartPagePrinter: returning successfully (ulCurPage=%lu)\n", pPrt->ulCurPage));
        return TRUE;
    }

    /* failure */
    SetLastError(OSLibSplWinGetLastError()); /** @todo convert error code!!! */
    dprintf(("WINSPOOL: StartPagePrinter failed. last err=%#x\n", OSLibSplWinGetLastError()));
    return FALSE;
}


/**
 * Write raw data to printer.
 * @returns Successindicator. lasterr set on failure.
 * @param   hPrinter    Handle to printer object.
 * @param   pBuf        Pointer to the data to write.
 * @param   cbBuf       Size of the data to write.
 * @param   pcWritten   Where to put the number of bytes actually written.
 * @status  partially implemented.
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  Current implementation relies on StartDocPrinterA/W being called first.
 */
BOOL WIN32API WritePrinter(HANDLE hPrinter, LPVOID pBuf, DWORD cbBuf, LPDWORD pcWritten)
{
    BOOL    fRc = FALSE;
    dprintf(("WINSPOOL: WritePrinter hPrinter=%#x pBuf=%p cbBuf=%d pcWritten=%p\n",
             hPrinter, pBuf, cbBuf, pcWritten));

    /*
     * Get printer instance data.
     */
    POPENPRINTER    pPrt = openprinterFind(hPrinter);
    if (!pPrt)
    {
        dprintf(("WINSPOOL: WritePrinter: Invalid handle %d\n", hPrinter));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    /*
     * Not quite sure what should happen here if no StartDocPrinter was issued.
     * For now we'll just freak out.
     */
    if (!pPrt->hspl)
    {
        DebugAssertFailed(("WritePrinter called with no StartDocPrinter issued\n"));
        SetLastError(ERROR_INVALID_PRINTER_STATE); /** @todo test on w2k */
        return FALSE;
    }

    /*
     * Push the data thru to the spooler.
     */
    if (OSLibSplQmWrite(pPrt->hspl, cbBuf, pBuf))
    {
        if (pcWritten)
            *pcWritten = cbBuf;
        dprintf(("WINSPOOL: WritePrinter succceeded writing %d bytes\n", cbBuf));
        return TRUE;
    }

    /* failure */
    SetLastError(OSLibSplWinGetLastError()); /** @todo convert error code!!! */
    dprintf(("WINSPOOL: WritePrinter failed. last err=%#x\n", OSLibSplWinGetLastError()));
    return FALSE;
}


/**
 * Notifies the spooler that the writing of a page to the current
 * printer job is completed.
 *
 * This is an informational API without much significance I think.
 * The implementation assumes StartDocPrinterA/W() must be called first.
 *
 * @returns Success indicator, last error set on failure.
 * @param   hPrinter    Handle to printer object which job is to be ended (completed.
 * @status  partially implemented
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  Current implementation relies on StartDocPrinterA/W being called first.
 */
BOOL WIN32API EndPagePrinter(HANDLE hPrinter)
{
    dprintf(("WINSPOOL: EndPagePrinter: hPrinter=%d\n", hPrinter));

    /*
     * Get printer instance data.
     */
    POPENPRINTER    pPrt = openprinterFind(hPrinter);
    if (!pPrt)
    {
        dprintf(("WINSPOOL: EndPagePrinter: Invalid handle %d\n", hPrinter));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    /*
     * Not quite sure what should happen here if no StartDocPrinter was issued.
     * For now we'll just freak out.
     */
    if (!pPrt->hspl)
    {
        DebugAssertFailed(("EndPagePrinter called with no StartDocPrinter issued\n"));
        SetLastError(ERROR_INVALID_PRINTER_STATE); /** @todo test on w2k */
        return FALSE;
    }

    /* There is no OS/2 equivalent. */
    dprintf(("WINSPOOL: EndPagePrinter: returning successfully (ulCurPage=%lu)\n", pPrt->ulCurPage));
    return TRUE;
}


/**
 * Ends a printer job.
 *
 * @returns Success indicator, last error set on failure.
 * @param   hPrinter    Handle to printer object which job is to be ended (completed.
 * @status  partially implemented
 * @author  knut st. osmundsen <bird-srcspam@anduin.net>
 * @remark  Current implementation relies on StartDocPrinterA/W being called first.
 */
BOOL WIN32API EndDocPrinter(HANDLE hPrinter)
{
    BOOL    fRc = FALSE;
    dprintf(("WINSPOOL: WritePrinter hPrinter=%#x\n", hPrinter));

    /*
     * Get printer instance data.
     */
    POPENPRINTER    pPrt = openprinterFind(hPrinter);
    if (!pPrt)
    {
        dprintf(("WINSPOOL: WritePrinter: Invalid handle %d\n", hPrinter));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    /*
     * Not quite sure what should happen here if no StartDocPrinter was issued.
     * For now we'll just freak out.
     */
    if (!pPrt->hspl)
    {
        DebugAssertFailed(("EndDocPrinter called with no StartDocPrinter issued\n"));
        SetLastError(ERROR_INVALID_PRINTER_STATE); /** @todo test on w2k */
        return FALSE;
    }

    /*
     * End the document, thus closing all Spl related stuff.
     */
    if (OSLibSplQmEndDoc(pPrt->hspl))
    {
        dprintf(("WINSPOOL: EndDocPrinter returns successfully\n"));
        return TRUE;
    }

    SetLastError(OSLibSplWinGetLastError()); /** @todo convert error code!!! */
    dprintf(("WINSPOOL: EndDocPrinter returns failure\n"));
    return FALSE;
}



/**
 * Setup the OS/2 Printer Information in the registry and profile.
 *
 * We will only enumerate local printer queues.
 *
 * The mapping between Odin32 and OS/2 is as follows:
 *      Odin32              OS/2
 *      Printer Name        Queue Comment (WPS object title)
 *      Driver Name         Queue Name
 *      Printer Comment     Queue Comment (= WPS object title)
 *      Printer Location    Device Logical address (port sort of)
 *      Printer Port        Device Logical address (port sort of)
 *
 * On OS/2 only queue names are unique. It's very easy to make duplicate
 * printer names, just create printer objects in more than one WPS folder.
 * For duplicate names the queue name will be appended to the name in
 * paratheses so the user can actually print to both printers. (Odin32
 * only have a concept of printer names and they must thus be unique.)
 *
 * @returns Success indicator. May possibly have changed the last error on
 *          both failure and success.
 *
 * @remark  We're only enumerating local printers. This makes sense as remote
 *          printers (on Lan Servers/Managers) isn't normally stored in the
 *          registry (IIRC).
 */
BOOL ExportPrintersToRegistry(void)
{
    BOOL            fRc = FALSE;
    DWORD           cbNeeded;
    DWORD           cUnused;

    dprintf(("WINSPOOL: ExportPrintersToRegistry"));

    /*
     * Get printers.
     */
    ULONG           cQueues = 0;
    POSLIB_PRQINFO3 paQueues;
    paQueues = (POSLIB_PRQINFO3)OSLibSplEnumQueue(NULL, 3, &cQueues, &cUnused);
    if (paQueues)
    {
        /*
         * Get printer devices.
         */
        ULONG   cDevices;
        POSLIB_PRDINFO3 paDevices;
        paDevices = (POSLIB_PRDINFO3)OSLibSplEnumDevice(NULL, 3, &cDevices, &cUnused);
        if (paDevices)
        {
            fRc = TRUE;             /* getting this far is pretty much a success I'd say.... */

            /*
             * Make printer WPS names (queue comment) unique by appending the
             * queuename for conflicting names.
             */
            unsigned    i;
            char **papszNewNames = (char**)calloc(sizeof(char*), cQueues);
            for (i = 0; i < cQueues; i++)
            {
                char *psz = paQueues[i].pszComment;
                for (unsigned j = 0; j < cQueues; j++)
                    if (j != i && !stricmp(paQueues[j].pszComment, psz))
                    {
                        dprintf(("duplicate printer names '%s'. appending queue name", psz));
                        papszNewNames[i] = (char*)malloc(strlen(psz) + strlen(paQueues[i].pszName) + 4);
                        sprintf(papszNewNames[i], "%s (%s)", psz, paQueues[i].pszName);
                        paQueues[i].pszComment = papszNewNames[i];

                        papszNewNames[j] = (char*)malloc(strlen(psz) + strlen(paQueues[j].pszName) + 4);
                        sprintf(papszNewNames[j], "%s (%s)", psz, paQueues[j].pszName);
                        paQueues[j].pszComment = papszNewNames[j];
                    }
            }

            /*
             * Add the printers.
             */
            PRINTER_INFO_2A *papi2 = (PRINTER_INFO_2A *)calloc(cQueues, sizeof(PRINTER_INFO_2A));
            dprintf(("cQueues=%d cDevices=%d", cQueues, cDevices));
            for (i = 0; i < cQueues; i++)
            {
                /* Make printer driver. */
                DRIVER_INFO_3A      di3 = {0};
                di3.cVersion           = 0x400;
#ifdef USE_OS2_DRIVERNAME
                char *pszDot = strchr(paQueues[i].pszDriverName, '.');
                if (pszDot && pszDot != paQueues[i].pszDriverName)  /* we're very careful :-) */
                {   /* split the driver name, getting something very similar to window.s */
                    *pszDot++ = '\0';
                    di3.pName          = pszDot;
                    di3.pDriverPath    = paQueues[i].pszDriverName; /* This is important! */
                }
                else
                {   /* bad printer driver name? try cover up. */
                    di3.pName          = paQueues[i].pszDriverName;
                    di3.pDriverPath    = (LPSTR)"WINSPOOL";   /* This is important! */
                }
#else
                di3.pName              = paQueues[i].pszName; /* printer driver == queue name */
                di3.pDriverPath        = "WINSPOOL";   /* This is important! */
#endif
                di3.pEnvironment       = NULL;         /* NULL means auto */
                di3.pDataFile          = (LPSTR)"<datafile?>";
                di3.pConfigFile        = (LPSTR)"winodin.drv";
                di3.pHelpFile          = (LPSTR)"<helpfile?>";
                di3.pDependentFiles    = (LPSTR)"<dependend files?>";
                di3.pMonitorName       = (LPSTR)"<monitor name?>";
                di3.pDefaultDataType   = (LPSTR)"RAW";
                if (!AddPrinterDriverA(NULL, 3, (LPBYTE)&di3))
                {
                    DebugAssertFailed(("Failed adding Driver (%ld)\n", GetLastError()));
                    fRc = FALSE;
                }

                /* Make printer. */
                papi2[i].pDatatype    = (LPSTR)"RAW";
                papi2[i].pPrintProcessor = (LPSTR)"WinPrint";
#ifdef USE_OS2_DRIVERNAME
                papi2[i].pComment     = paQueues[i].pszName;    /* Queue name. Don't allow any changes of the comment! */
#else
                papi2[i].pComment     = paQueues[i].pszComment; /* WPS printer name */
#endif
                papi2[i].pDriverName  = di3.pName;
                papi2[i].pParameters  = (LPSTR)"<parameters?>";
                papi2[i].pShareName   = (LPSTR)"<share name?>";
                papi2[i].pSepFile     = (LPSTR)"<sep file?>";
#if 0 /* only 'local', remember */
                if (paPrinters[i].pszComputerName) /* this is currnetly not used as we only enum locals. */
                {
                    papi2[i].Attributes |= PRINTER_ATTRIBUTE_NETWORK;
                    papi2[i].pServerName = paPrinters[i].pszComputerName; /** @todo: format!! */
                }
#endif

                /*
                 * We need to make sure the name is valid.
                 * The registry decides what's illegal, slashes evedently are.
                 */
                papi2[i].pPrinterName = paQueues[i].pszComment;
                char *psz = papi2[i].pPrinterName;
                while ((psz = strpbrk(psz, "/\\")) != NULL)
                       *psz = '|';      /* change slashes to pipes... */


                /*
                 * The queue pszPrinters member links with pszPrinterName of the device.
                 * pszPrinters can contain several printer (device) names, separated by commas.
                 * - We only handle the first printer listed. No idea how to get more than one
                 *   the anyway.
                 */
                papi2[i].pLocation    = (LPSTR)"";
                papi2[i].pPortName    = (LPSTR)"";
                psz = strchr(paQueues[i].pszPrinters, ',');
                if (psz)
                    *psz = '\0';
                for (unsigned k = 0; k < cDevices; k++)
                {
                    if (!stricmp(paDevices[k].pszPrinterName, paQueues[i].pszPrinters))
                    {
                        papi2[i].pLocation    = paDevices[k].pszLogAddr;
                        papi2[i].pPortName    = paDevices[k].pszLogAddr;
                        break;
                    }
                }

                /*
                 * Check if default printer and set attribute and write to profile.
                 */
                if (paQueues[i].fsType & OSLIB_PRQ3_TYPE_APPDEFAULT)
                {
                    char    szWinDefPrn[256];
                    dprintf(("Default printer %s,%s,%s", papi2[i].pPrinterName, papi2[i].pDriverName, papi2[i].pPortName));
                    sprintf(szWinDefPrn, "%s,%s,%s", papi2[i].pPrinterName, papi2[i].pDriverName, papi2[i].pPortName);
                    WriteProfileStringA("windows", "device", szWinDefPrn);
                    papi2[i].Attributes |= PRINTER_ATTRIBUTE_DEFAULT;
                }

                /*
                 * Finally, we can add the printer
                 */
                dprintf(("Add printer %s,%s,%s", papi2[i].pPrinterName, papi2[i].pDriverName, papi2[i].pPortName));
                if (!AddPrinterA(NULL, 2, (LPBYTE)&papi2[i]))
                {
                    DebugAssert(GetLastError() == ERROR_PRINTER_ALREADY_EXISTS,
                                ("AddPrinterA(%s) failed with rc=%ld\n", papi2[i].pPrinterName, GetLastError()));
                    fRc = FALSE;
                }
            } /* loop thru printers */


            /*
             * Purge dead printers.
             * (It's important that we don't bail out before this when there is zero printers/devices!)
             */
            PPRINTER_INFO_2A    pi2;
            DWORD               cOdinPrinters = 0;
            cbNeeded = 0;
            if (    EnumPrintersA(PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &cbNeeded, &cOdinPrinters)
                ||  GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                pi2 = (PPRINTER_INFO_2A)malloc(cbNeeded);
                if (EnumPrintersA(PRINTER_ENUM_LOCAL, NULL, 2, (LPBYTE)pi2, cbNeeded, &cbNeeded, &cOdinPrinters))
                {
                    for (i = 0; i < cOdinPrinters; i++)
                    {
                        /* look if we added it */
                        BOOL fFound = FALSE;
                        for (unsigned j = 0; j < cQueues && !fFound; j++)
                            fFound = !strcmp(papi2[j].pPrinterName,pi2[i].pPrinterName)
                                &&   !strcmp(papi2[j].pDriverName, pi2[i].pDriverName)
                                &&   !strcmp(papi2[j].pPortName,   pi2[i].pPortName);
                        /* nuke it */
                        if (!fFound)
                        {
                            HANDLE hPrinter;
                            if (OpenPrinterA(pi2[i].pPrinterName, &hPrinter, NULL) == TRUE)
                            {
                                dprintf(("Delete printer %s,%s,%s", papi2[i].pPrinterName, papi2[i].pDriverName, papi2[i].pPortName));
                                DeletePrinter(hPrinter);
                                ClosePrinter(hPrinter);
                            }
                            else
                            {
                                DebugAssertFailed(("Failed to open a printer returned by EnumPrintersA, %s\n",
                                                pi2[i].pPrinterName));
                                fRc = FALSE;
                            }
                        }
                    }
                }
                else
                {
                    DebugAssertFailed(("EnumPrintersA failed!\n"));
                    fRc = FALSE;
                }
                free(pi2);
            }
            else
            {
                DebugAssertFailed(("EnumPrintersA failed!\n"));
                fRc = FALSE;
            }
            free(papi2);
            free(paDevices);
            for (i = 0; i < cQueues; i++)
                if (papszNewNames[i])
                    free(papszNewNames[i]);
        }
        else
            dprintf(("OSLibSplEnumDevice -> %d, cDevices=%d", fRc, cDevices));
    }
    else
        dprintf(("OSLibSplEnumPrinter failed\n"));

    free(paQueues);
    dprintf(("ExportPrintersToRegistry returns %s", fRc ? "success" : "failure"));
    return fRc;
}

#endif /* __WIN32OS2__ */
