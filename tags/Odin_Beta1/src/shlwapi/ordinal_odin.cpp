/* $Id: ordinal_odin.cpp,v 1.5 2002-02-06 20:18:27 sandervl Exp $ */

/*
 * Win32 Lightweight SHELL32 for OS/2
 *
 * Copyright 2000 Patrick Haller (phaller@gmx.net)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Ordinally Exported Functions
 *
 */

/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <wcstr.h>
#define HAVE_WCTYPE_H

#include "debugtools.h"

#include <winreg.h>

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>
#include <winversion.h>
#include <winuser.h>


#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "winerror.h"
#include "winnls.h"
#include "winversion.h"
#include "heap.h"

#include "win/wine/obj_base.h"
#include "shellapi.h"
#include "shlobj.h"
#include "wine/undocshell.h"

#include "shlwapi.h"
#include "shlwapi_odin.h"


/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHLWAPI-ORDINAL)


#if 0
/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

HANDLE WIN32API SHLWAPI_11(HANDLE, DWORD, DWORD, DWORD, DWORD);



/*****************************************************************************
 * Name      : ???
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

// return characters in protocol part
static INT extractProtocolFromURL(LPSTR  pszURL,
                                  LPSTR* ppszColon)
{
  int i = 0;  // number of characters scanned
  LPSTR s = pszURL;

  *ppszColon = NULL;

  if (*pszURL == 0)
    return 0;


  // scan through the string for the 1st colon
  while(*s)
  {
    // filter non-printable characters
    if (*s < ' ')
      return 0;
    else
      if (*s > 0x80)
        return 0;     

    // Note: actually, the original code has a table with
    // "forbidden" characters, we don't reproduce this here yet.

    if (*s == ':')
    {
       // yes, first colon found!
       if ( (pszURL[0] == 'U' || pszURL[0] == 'u') &&
            (pszURL[1] == 'R' || pszURL[1] == 'r') &&
            (pszURL[2] == 'L' || pszURL[2] == 'l') )
       {
         // restart scan!
         pszURL = s+1;
         i = 0;
       }
       else
       {
         // OK, protocol extracted
         *ppszColon = s;
         return i;
       }
    }

    // skip to next character
    i++;
    s++;
  }  

  return 0;
}

typedef struct tabProtocolTableEntry
{
  LPSTR  pszName; // Note: original is unicode
  DWORD  ID;
  DWORD  dwNameLength;
  DWORD  dwCharacteristic;
} PROTOCOLTABLEENTRY, *LPPROTOCOLTABLEENTRY;


#define PROTOCOL_HTTP 2
#define PROTOCOL_FILE 9

static PROTOCOLTABLEENTRY tabProtocolTable[] =
{
  { "http",        PROTOCOL_HTTP,  4, 0x0a },
  { "file",        PROTOCOL_FILE,  4, 0x08 },
  { "ftp",         1,  3, 0x0a },
  { "https",      11,  5, 0x0a },
  { "news",        5,  4, 0x0a },
  { "mailto",      0,  6, 0x01 }, 
  { "gopher",      3,  6, 0x0a },
  { "nntp",        6,  4, 0x0a },
  { "telnet",      7,  6, 0x0a },
  { "wais",        8,  4, 0x00 },
  { "mk",         10,  2, 0x04 },
  { "shell",      12,  5, 0x01 },
  { "local",      14,  5, 0x00 },
  { "javascript", 15, 10, 0x05 },
  { "vbscript",   16,  8, 0x05 },
  { "snews",      13,  5, 0x0a },
  { "about",      17,  5, 0x05 },
  { "res",        18,  3, 0x04 }
};
#define END_OF_PROTOCOLTABLE (tabProtocolTable + (sizeof(tabProtocolTable) / sizeof(PROTOCOLTABLEENTRY)) )



static DWORD getProtocolTableEntry(LPSTR pszProtocol, DWORD dwNameLength)
{
  LPPROTOCOLTABLEENTRY lpEntry = tabProtocolTable;

  for(;;)
  {
    if (lpEntry->dwNameLength == dwNameLength)
      if (lstrncmpiA( lpEntry->pszName, pszProtocol, dwNameLength) == 0)
      {
        return lpEntry->ID; 
      }

    lpEntry++;

    // if scanning beyond end of the table,
    // abort and return null
    if (lpEntry > END_OF_PROTOCOLTABLE)
      return 0;
  }
}

typedef struct tagProtocolHandlerA
{
  DWORD  dwSize;
  DWORD  dwProtocolNameLength;
  LPSTR  lpszProtocolName;
  LPSTR  lpszURL;
  DWORD  dwURLNameLength;
  DWORD  ProtocolID;
} PROTOCOLHANDLERA, *LPPROTOCOLHANDLERA;

typedef struct tagProtocolHandlerW
{
  DWORD  dwSize;
  DWORD  dwProtocolNameLength;
  LPWSTR lpszProtocolName;
  LPWSTR lpszURL;
  DWORD  dwURLNameLength;
  DWORD  ProtocolID;
} PROTOCOLHANDLERW, *LPPROTOCOLHANDLERW;


ODINFUNCTION2(DWORD,   SHLWAPI_1,
              LPSTR,   lpszURL,
              LPPROTOCOLHANDLERA, lpHandler)
{
  dprintf(("not implemented"));

  if (NULL == lpszURL)
    return E_INVALIDARG;

  if (NULL == lpHandler)
    return E_INVALIDARG;

  if (lpHandler->dwSize != sizeof( PROTOCOLHANDLERA) )
    return E_INVALIDARG;

  dprintf(("SHLWAPI-SHLWAPI1: URL=%s",
    lpszURL));
  
  LPSTR lpszColon;
  INT iProtocolLength = extractProtocolFromURL(lpszURL,
                                               &lpszColon);
  lpHandler->dwProtocolNameLength = iProtocolLength;
  if (0 == iProtocolLength)
    return 0x80041001; // unknown error constant

  lpHandler->lpszProtocolName = lpszURL;

  DWORD ID = getProtocolTableEntry(lpszURL,
                                   iProtocolLength);
  lpHandler->ProtocolID = ID;
  lpHandler->lpszURL    = (LPSTR)(lpszURL + iProtocolLength + 1);
  
  if (ID == PROTOCOL_FILE)
  {
    // cut off leading slashes as required
    if (lpHandler->lpszURL[0] == '/' &&
        lpHandler->lpszURL[1] == '/')
      lpHandler->lpszURL = lpHandler->lpszURL + 2;

    if (lpHandler->lpszURL[0] == '/')
      lpHandler->lpszURL = lpHandler->lpszURL + 1;
  }

  lpHandler->dwURLNameLength = lstrlenA(lpHandler->lpszURL);

  return NO_ERROR;
}
#endif



/*****************************************************************************
 * Name      : SHLWAPI_3
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION2(BOOL,  SHLWAPI_3,
              LPSTR, lpPath,
              LPSTR, lpFile)
{
  dprintf(("not properly implemented"));

  return TRUE;

#if 0
  if (lpPath == NULL)                return FALSE;
  if (PathIsUNCServerA(lpPath))      return FALSE;
  if (PathIsUNCServerShareA(lpPath)) return FALSE;

  if (lpFile == NULL)
    return PathFileExistsA(lpPath);
  else
  {
    // PathFindExtensionA
    // lstrlenA
    // lstrcpyA
    // PathFileExistsA
    return FALSE;
  }

  return FALSE;
#endif
}


/*****************************************************************************
 * Name      : SHLWAPI_4
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION2(BOOL,   SHLWAPI_4,
              LPWSTR, lpPath,
              LPWSTR, lpFile)
{
  dprintf(("not properly implemented"));

  return TRUE;

#if 0
  if (lpPath == NULL)                return FALSE;
  if (PathIsUNCServerW(lpPath))      return FALSE;
  if (PathIsUNCServerShareW(lpPath)) return FALSE;

  if (lpFile == NULL)
    return PathFileExtensionW(lpPath);
  else
  {
    // PathFindExtensionW
    // lstrlenW
    // lstrcpyW
    // PathFileExistsW
    return FALSE;
  }

  return FALSE;
#endif
}


/*****************************************************************************
 * Name      : ???
 * Purpose   : extended version of PathFindOnPathA
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION3(DWORD, SHLWAPI_5,
              LPSTR, lpFile,
              LPSTR, lpDirectories,
              DWORD, dwUnknown)

{
  dprintf(("not implemented"));

  return 0;
}


/*****************************************************************************
 * Name      : ???
 * Purpose   : extended version of PathFindOnPathA
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION3(DWORD, SHLWAPI_6,
              DWORD, arg0,
              DWORD, arg1,
              DWORD, arg2)

{
  dprintf(("not implemented"));

  return 0;
}





/*****************************************************************************
 * Name      : ???
 * Purpose   : Looks like a strdup()
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION2(DWORD,SHLWAPI_12,
              DWORD,arg0,
              DWORD,arg1)
{
  dprintf(("not implemented, explorer.exe will trap now"));

  return 0;
}




/*****************************************************************************
 * Name      : ???
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION2(DWORD,SHLWAPI_17,
              DWORD, arg0,
              DWORD, arg1)
{
  dprintf(("not implemented, explorer.exe will trap now"));

  return 0;
}




/*****************************************************************************
 * Name      : ???
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION2(DWORD,SHLWAPI_20,
              DWORD, arg0,
              DWORD, arg1)
{
  dprintf(("not implemented, explorer.exe will trap now"));

  return 0;
}








/*****************************************************************************
 * Name      : DWORD SHLWAPI_160
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.160
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION2(DWORD,  SHLWAPI_160,
              DWORD,  arg0,
              DWORD,  arg1)
{
  dprintf(("not implemented.\n"));
  return 0;
}





/*****************************************************************************
 * Name      : SHLWAPI_185
 * Purpose   : some M$ nag screen ?
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.185
 * Status    : COMPLETELY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION6(DWORD,   SHLWAPI_185,
              DWORD, arg0,
              LPSTR, lpStr1,
              LPSTR, lpStr2,
              DWORD, arg3,
              DWORD, dwDefault,
              LPSTR, lpstrValueName)
{
  BOOL  fDontShow;
  WCHAR szValueNameW[256];

  fDontShow = SHRegGetBoolUSValueA("Software\\Microsoft\\Windows\\CurrentVersion\\"
                                   "Explorer\\DontShowMeThisDialogAgain",
                                   lpstrValueName,
                                   0,
                                   1);
  if (fDontShow == FALSE)
    return dwDefault;

  int iLength1 = lstrlenA(lpStr1)+1;
  HLOCAL hLocal1 = LocalAlloc(LMEM_ZEROINIT,
                              iLength1 << 1);
  if (hLocal1 == NULL)
    return dwDefault;

  int iLength2 = lstrlenA(lpStr2)+1;
  HLOCAL hLocal2 = LocalAlloc(LMEM_ZEROINIT,
                              iLength2 << 1);
  if (hLocal2 == NULL)
  {
    LocalFree(hLocal1);
    return dwDefault;
  }

#if 0
  // convert all ascii values to Unicode
  SHLWAPI_215(lpStr1, (LPWSTR)hLocal1, iLength1);
  SHLWAPI_215(lpStr2, (LPWSTR)hLocal2, iLength2);
  SHLWAPI_215(lpstrValueName,  szValueNameW, 256);

  // do something
  dwDefault = SHLWAPI_191(arg0,
              (LPWSTR)hLocal1,
              arg3,
              dwDefault,
              szValueNameW);
#endif

  if (hLocal1)
    LocalFree(hLocal1);

  if (hLocal2)
    LocalFree(hLocal2);

  return dwDefault;
}


/*****************************************************************************
 * Name      : SHLWAPI_191
 * Purpose   : display some M$ nag screen if enabled
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.191
 * Status    : COMPLETELY IMPLEMENTED ? UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION6(DWORD,  SHLWAPI_191,
              HWND,   hwndParent,
              DWORD,  arg1,
              DWORD,  arg2,
              DWORD,  arg3,
              DWORD,  dwDefault,
              LPWSTR, lpstrDialog)
{
  BOOL rc = SHRegGetBoolUSValueW((LPCWSTR)L"Software\\Microsoft\\Windows\\CurrentVersion"
                                 L"\\Explorer\\DontShowMeThisDialogAgain",
                                 lpstrDialog,
                                 0,
                                 1);
  if (rc == FALSE)
    return dwDefault;

  static HINSTANCE hShellInstance; // @@@PH where to put / initialize?

#if 0
  struct
  {
    DWORD s1;
    DWORD s2;
    DWORD s3;
    DWORD s4;
  } sInit;

  sInit.s1 = "software...";
  sInit.s2 = arg1;
  sInit.s3 = arg2;
  sInit.s4 = arg3;

  return DialogBoxParamW(hShellInstance,
                         0x1200,          // some template
                         hwndParent,
                         i_DialogProc,
                         &sInit);
#endif

  return dwDefault;
}




/*****************************************************************************
 * Name      : BOOL SHLWAPI_197
 * Purpose   : Set text background?
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.197
 * Status    : COMPLETELY IMPLEMENTED ? UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION3(BOOL,         SHLWAPI_197,
              HDC,          hdc,
              CONST RECT*,  lprc,
              COLORREF,     crColor)
{
  COLORREF crOld;
  BOOL     res;

  crOld = SetBkColor(hdc, crColor);
  res = ExtTextOutA(hdc,
                    0,
                    0,
                    ETO_OPAQUE,
                    lprc,
                    0,
                    0,
                    0);
  SetBkColor(hdc, crOld);

  return res;
}



/*****************************************************************************
 * Name      : SHLWAPI_243
 * Purpose   : does something critical, even with performance counters
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.243
 * Status    : STUB UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION5(DWORD, SHLWAPI_243,
              DWORD, arg0,
              DWORD, arg1,
              DWORD, arg2,
              DWORD, arg3,
              DWORD, arg4)
{
  dprintf(("not implementes.\n"));
  return 0;
}

/*****************************************************************************
 * Name      : DWORD SHLWAPI_437
 * Purpose   : Determine product version and options
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.437
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

#define REG_PRODUCTOPTIONS "System\\CurrentControlSet\\Control\\ProductOptions"
#define REG_OPTIONS_PRODUCTTYPE "ProductType"
#define REG_OPTIONS_ENTERPRISE  "Enterprise"
#define REG_OPTIONS_DATACENTER  "DataCenter"

ODINFUNCTION1(DWORD,  SHLWAPI_437,
              DWORD,  nFunction)
{
  static BOOL           flagProductOptionsInitialized = FALSE;
  static BOOL           flagIsProductEnterprise       = FALSE;
  static BOOL           flagIsProductDatacenter       = FALSE;
  static OSVERSIONINFOA osVersionInfo;

  HKEY          hKeyOptions;
  DWORD         dwKeyType;
  char          szBuffer[260]; // MAX_PATH_LEN
  DWORD         dwDataLength;

  dprintf(("not (properly) implemented.\n"));

  if (flagProductOptionsInitialized == FALSE)
  {
    // set to TRUE regardless of subsequent errors
    flagProductOptionsInitialized = TRUE;

    // initialize required structures
    osVersionInfo.dwOSVersionInfoSize = 0x9c;
    if (GetVersionExA(&osVersionInfo) == FALSE)
    {
      osVersionInfo.dwOSVersionInfoSize = 0x94;
      GetVersionExA(&osVersionInfo);
    }

    LONG rc = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                           REG_PRODUCTOPTIONS,
                           0,
                           KEY_READ,
                           &hKeyOptions);
    if (rc == ERROR_SUCCESS)
    {
      dwDataLength = sizeof(szBuffer);
      rc = RegQueryValueExA(hKeyOptions,
                            REG_OPTIONS_PRODUCTTYPE,
                            0,
                            &dwKeyType,
                            (LPBYTE)szBuffer,
                            &dwDataLength);
      if (StrStrIA(szBuffer, REG_OPTIONS_ENTERPRISE) != 0)
        flagIsProductEnterprise = TRUE;
      else
      if (StrStrIA(szBuffer, REG_OPTIONS_DATACENTER) != 0)
        flagIsProductDatacenter = TRUE;

      RegCloseKey(hKeyOptions);
    }
  }

  // OK, now to the usual work ...
  switch (nFunction)
  {
    // is platform WINDOWS
    case 0:
      if (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
        return 1;
      else
        return 0;

    // is platform NT
    case 1:
      if (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
        return 1;
      else
        return 0;

    // is  platform Windows 95/98/xx ?
    case 2:
      if (osVersionInfo.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
        return 0;
      if (osVersionInfo.dwMajorVersion >= 4)
        return 1;
      else
        return 0;

    // is platform NT4 or better?
    case 3:
      if (osVersionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
        return 0;
      if (osVersionInfo.dwMajorVersion >= 4)
        return 1;
      else
        return 0;

    // is platform Win2000 or better?
    case 4:
      if (osVersionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
        return 0;
      if (osVersionInfo.dwMajorVersion >= 5)
        return 1;
      else
        return 0;

    // at least Windows 4.10 ?
    case 5:
      if (osVersionInfo.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
        return 0;
      if (osVersionInfo.dwMajorVersion > 4)
        return 1;
      else
        if (osVersionInfo.dwMajorVersion == 4)
          if (osVersionInfo.dwMinorVersion >= 10)
            return 1;

      return 0;

    // is platform Windows98 GA ?
    case 6:
      if ( (osVersionInfo.dwPlatformId   == VER_PLATFORM_WIN32_WINDOWS) &&
           (osVersionInfo.dwMajorVersion == 4) &&
           (osVersionInfo.dwMinorVersion == 10) &&
           (osVersionInfo.dwBuildNumber  == 1998) ) // 0x7ce
        return 1;
      else
        return 0;

    // is platform some specific CSD ?
    case 7:
    case 8:
      //@@@PH incorrect
      if (osVersionInfo.dwMajorVersion >= 5)
        return 1;
      else
        return 0;

    case 9:
      //@@@PH incorrect
      if (osVersionInfo.dwMajorVersion >= 5 ||
          flagIsProductEnterprise ||
          flagIsProductDatacenter)
        return 1;
      else
        return 0;

    case 10:
      //@@@PH incorrect
      if (osVersionInfo.dwMajorVersion >= 5)
        return flagIsProductEnterprise;
      else
        return 0;

    case 11:
      //@@@PH incorrect
      if (osVersionInfo.dwMajorVersion >= 5)
        return flagIsProductDatacenter;
      else
        return 0;

    // @@@PH: Oops, wazzup there ?
    case 12:
      return GetSystemMetrics(4096);
  }

  return 0;
}


/*****************************************************************************
 * Name      : SHIsLowMemoryMachine
 * Purpose   :
 * Parameters: BOOL fRetest - TRUE if testing shall be repeated
 *                          - FALSE if cached result is to be used
 * Variables :
 * Result    : 0 - machine is not memory-constrained
 *             1 - machine is memory-constrained
 * Remark    : SHLWAPI.584
 * Status    : COMPLETELY IMPLEMENTED ? UNTESTED
 *
 * Author    : Patrick Haller [Mon, 2000/06/11 02:02]
 *****************************************************************************/

ODINFUNCTION1(int,     SHIsLowMemoryMachine,
              BOOL,    fRetest)
{
         MEMORYSTATUS memBuffer;
  static int          flagIsLowMemoryMachine = -1;
         ULONG        ulMem;

  // use cached result?
  if ( (fRetest == TRUE) ||
       (flagIsLowMemoryMachine == -1) )
  {
    // determine and store result
    GlobalMemoryStatus(&memBuffer);
    if (VERSION_OsIsUnicode())
      ulMem = 0x1000000; // unicode operation mode
    else
      ulMem = 0x0800000; // ascii operation mode

    // enough memory?
    if (memBuffer.dwTotalPhys <= ulMem)
      flagIsLowMemoryMachine = 1;
    else
      flagIsLowMemoryMachine = 0;
  }

  return flagIsLowMemoryMachine;
}

