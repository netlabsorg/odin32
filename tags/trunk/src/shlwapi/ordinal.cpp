/* $Id: ordinal.cpp,v 1.5 2000-08-24 09:32:42 sandervl Exp $ */

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


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

HANDLE WIN32API SHLWAPI_11(HANDLE, DWORD, DWORD, DWORD, DWORD);


#define debugstr_guid(a) a


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

ODINFUNCTION2(DWORD,SHLWAPI_1,
              DWORD,arg0,
              DWORD,arg1)
{
  dprintf(("not implemented"));

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
 * Author    : Christoph Bratschi [Wed, 2000/03/29 19:47]
 *****************************************************************************/

ODINFUNCTION2(DWORD,SHLWAPI_2,
              DWORD,arg0,
              DWORD,arg1)
{
  dprintf(("not implemented"));

  return 0;
}


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
 * Purpose   : Unknown (used by explorer.exe)
 * Parameters: Unknown (wrong)
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION3(DWORD,SHLWAPI_7,
              DWORD, arg0,
              DWORD, arg1,
              DWORD, arg2)
{
  HANDLE hMap;
  LPVOID lpMap;
  DWORD  dwResult;

  dprintf(("not implemented"));

  // create mapping
  hMap = CreateFileMappingA(-1,
                            NULL,
                            PAGE_READWRITE,
                            0,
                            arg1,
                            0);
  if (hMap == NULL)
    return FALSE;

  // now map the thing
  lpMap = MapViewOfFile(hMap,
                        FILE_MAP_READ | FILE_MAP_WRITE,
                        0,
                        0,
                        0);
  if (lpMap == NULL)
  {
    CloseHandle(hMap);
    return FALSE;
  }

  /*  @@@PH copy someting into the shared segment */

  UnmapViewOfFile(lpMap);
  dwResult = SHLWAPI_11(hMap,
                        GetCurrentProcessId(),
                        arg2,
                        FILE_MAP_ALL_ACCESS,
                        2);
  CloseHandle(hMap);

  return dwResult;
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

ODINFUNCTION2(DWORD,  SHLWAPI_8,
              HANDLE, hMap,
              DWORD,  arg1)
{
  HANDLE hHandle;
  LPVOID lpBase;

  hHandle = SHLWAPI_11(hMap,
                       arg1,
                       GetCurrentProcessId(),
                       FILE_MAP_ALL_ACCESS,
                       0);
  lpBase = MapViewOfFile(hHandle,
                         FILE_MAP_READ | FILE_MAP_WRITE,
                         0,
                         0,
                         0);
  CloseHandle(hHandle);

  if (lpBase == NULL)
    return NULL;
  else
    return (DWORD)lpBase+4; // lea eax, [ebx + 4]
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : COMPLETELY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION1(DWORD,  SHLWAPI_9,
              LPBYTE, lpBase)
{
  return (UnmapViewOfFile( (LPVOID) ((DWORD)lpBase & 0xFFFFFFFC)) );
}


/*****************************************************************************
 * Name      : ???
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : COMPLETELY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION2(DWORD,  SHLWAPI_10,
              HANDLE, hMap,
              DWORD,  arg1)
{
  HANDLE hHandle;

  hHandle = SHLWAPI_11(hMap,
                       arg1,
                       GetCurrentProcessId(),
                       FILE_MAP_ALL_ACCESS,
                       1);
  return CloseHandle(hHandle);
}


/*****************************************************************************
 * Name      :
 * Purpose   : Duplicate the handle to the shell shared segment
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.11 - wrong implementation, check again
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION5(HANDLE, SHLWAPI_11,
              HANDLE, hMap,
              DWORD,  dwProcessDest,
              DWORD,  dwCurrentPID,
              DWORD,  dwAccess,
              DWORD,  dwFlag)
{
  dprintf(("not implemented, explorer.exe will trap now"));

  DWORD  dwMyPID = GetCurrentProcessId();
  HANDLE hProcess;
  HANDLE hTargetHandle;
  HANDLE hTargetProcessHandle = dwProcessDest;
  BOOL   fResult;

  // open desired process
  if (dwMyPID == dwProcessDest)
    hProcess == GetCurrentProcess();
  else
    hProcess == OpenProcess(PROCESS_DUP_HANDLE,
                            FALSE,
                            dwProcessDest);

  // verify result
  if (hProcess == NULL)
    return 0;

  // duplicate handle
  fResult = DuplicateHandle(hProcess,
                            hMap,
                            hTargetProcessHandle,
                            &hTargetHandle,
                            dwAccess,
                            0,
                            dwFlag | 0x02);
  //CloseHandle()

  return fResult;
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
 * Parameters:
 * Variables :
 * Result    : Unknown
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION2(DWORD,SHLWAPI_13,
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

ODINFUNCTION2(DWORD,SHLWAPI_14,
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

ODINFUNCTION2(DWORD,SHLWAPI_15,
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
 * Remark    : SHLWAPI.16
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINFUNCTION4(HANDLE,SHCreateThread,
              DWORD, arg0,
              DWORD, arg1,
              DWORD, arg2,
              DWORD, arg3)
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

ODINFUNCTION2(DWORD,SHLWAPI_18,
              DWORD, arg0,
              DWORD, arg1)
{
  dprintf(("not implemented, explorer.exe will trap now"));

  return 0;
}


/*****************************************************************************
 * Name      : SHLWAPI_19
 * Purpose   : Free memory
 * Parameters: LPVOID lpMem
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.19
 * Status    : COMPLETELY IMPLEMENTED ? UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/09 04:47]
 *****************************************************************************/

ODINPROCEDURE1(SHLWAPI_19,
               HLOCAL, hMem)
{
  if (hMem != NULL)
    LocalFree(hMem);
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

ODINFUNCTION2(DWORD,SHLWAPI_21,
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

ODINFUNCTION2(DWORD,SHLWAPI_22,
              DWORD, arg0,
              DWORD, arg1)
{
  dprintf(("not implemented, explorer.exe will trap now"));

  return 0;
}


/*****************************************************************************
 * Name      : SHLWAPI_23
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    : returns strlen(str)
 * Remark    : converts a guid to a string
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION3(DWORD,  SHLWAPI_23,
              REFGUID,guid,  /* [in]  clsid */
              LPSTR,  str,     /* [out] buffer */
              INT,    cmax) /* [in]  size of buffer */
{
    char xguid[80];

    TRACE("(%s %p 0x%08x)stub\n", debugstr_guid(guid), str, cmax);

    if (WINE_StringFromCLSID(guid,xguid)) return 0;
    if (strlen(xguid)>=cmax) return 0;
    strcpy(str,xguid);
    return strlen(xguid) + 1;
}


/*************************************************************************
*      SHLWAPI_24  [SHLWAPI.24]
*
* NOTES
*   converts a guid to a string
*   returns strlen(str)
*/
ODINFUNCTION3(DWORD,   SHLWAPI_24,
              REFGUID, guid,   /* [in]  clsid */
              LPWSTR,  str,    /* [out] buffer */
              INT,     cmax)   /* [in]  size of buffer */
{
  TRACE("(%s %p 0x%08x)stub\n", debugstr_guid(guid), str, cmax);
  return StringFromGUID2(guid, str, cmax);
}


/*****************************************************************************
 * Name      : SHLWAPI_28
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(DWORD,SHLWAPI_28,
              DWORD,arg0)
{
  dprintf(("not yet implemented"));

  return 0;
}


/*****************************************************************************
 * Name      : SHLWAPI_30
 * Purpose   : Check for whitespace in ascii and unicode ?
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : COMPLETELY IMPLEMENTED ? UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(DWORD,SHLWAPI_30,
              WORD, ch)
{
  if ( (ch == 0x09)   || // tab
       (ch == 0x20)   || // space
       (ch == 0x3000) ||
       (ch == 0xFEFF) )
    return 1;
  else
    return 0;
}



/*****************************************************************************
 * Name      : DWORD SHLWAPI_156
 * Purpose   : Case-sensitive wide string compare
 * Parameters: LPSTR lpStr1
 *             LPSTR lpStr2
 * Variables :
 * Result    : case-sensitive comparsion result between the two strings
 * Remark    : SHLWAPI.158
 * Status    : COMPLETELY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION2(DWORD,  SHLWAPI_156,
              LPWSTR, str1,   /* "shell32.dll" */
              LPWSTR, str2)   /* "shell32.dll" */
{
  return lstrcmpW(str1,str2);
}


/*****************************************************************************
 * Name      : DWORD SHLWAPI_158
 * Purpose   : Case-ignoring wide string compare
 * Parameters: LPSTR lpStr1
 *             LPSTR lpStr2
 * Variables :
 * Result    : case-insensitive comparsion result between the two strings
 * Remark    : SHLWAPI.158
 * Status    : COMPLETELY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION2(int,     SHLWAPI_158,
              LPWSTR,  lpStr1,
              LPWSTR,  lpStr2)
{
  WCHAR ch1;
  WCHAR ch2;
  int   res;

  for (;

       *lpStr1 != 0;

       lpStr1++,
       lpStr2++)
  {
    ch1 = *lpStr1;
    ch2 = *lpStr2;

    /* ignore case of characters */
    if ( (ch1 >= 'A') && (ch1 <= 'Z')) ch1 += 0x20;
    if ( (ch2 >= 'A') && (ch2 <= 'Z')) ch1 += 0x20;

    /* compare characters */
    res = ch1 - ch2;
    if (res)
      return res;
  }

  /* OK, strings compared til end of string1 */
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



/*************************************************************************
*      SHLWAPI_169 [SHLWAPI]
*/
ODINFUNCTION1(DWORD,       SHLWAPI_169,
              IUnknown **, lplpUnknown)
{
  IUnknown *lpUnknown;

  if (lplpUnknown == NULL)
    return 0;

  lpUnknown = *lplpUnknown;
  if (lpUnknown == NULL)
    return 0;

  *lplpUnknown = NULL; // kill object pointer

  // and still call the object's release method
  return IUnknown_Release(lpUnknown);
}


/*****************************************************************************
 * Name      : LPSTR SHLWAPI_170
 * Purpose   : verify if current string position is "//" or not
 *             skip "//" ...
 * Parameters: LPSTR lpStr
 * Variables :
 * Result    : lpStr + 2 or NULL
 * Remark    : SHLWAPI.170
 * Status    : COMPLETELY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(LPSTR, SHLWAPI_170,
              LPSTR, lpStr)
{
  if ( (lpStr      != NULL) &&
       (*lpStr     == '/' ) &&
       (*(lpStr+1) == '/' ) )
    return lpStr+2;
  else
    return lpStr;
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



/*************************************************************************
*      SHLWAPI_193 [SHLWAPI]
* query current color depth
*/
ODINFUNCTION0(DWORD, SHLWAPI_193)
{
  HDC hdc;
  DWORD ret;

  hdc = GetDC(0);
  ret = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
  ReleaseDC(0, hdc);
  return ret;
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


/*************************************************************************
*      SHLWAPI_219 [SHLWAPI]
*/
ODINFUNCTION4(HRESULT, SHLWAPI_219,
              LPVOID,  w, /* returned by LocalAlloc */
              LPVOID,  x,
              LPVOID,  y,
              LPWSTR,  z) /* OUT: path */
{
  FIXME("(%p %p %p %p)stub\n",w,x,y,z);
  return 0xabba1252;
}


/*************************************************************************
*      SHLWAPI_222 [SHLWAPI]
*
* NOTES
*  securityattributes missing
*/
ODINFUNCTION1(HANDLE,  SHLWAPI_222,
              LPCLSID, guid)
{
  char lpstrName[80];
  strcpy( lpstrName,"shell.");
  WINE_StringFromCLSID(guid, lpstrName + strlen(lpstrName));

  FIXME("(%s) stub\n", lpstrName);
  return CreateSemaphoreA(NULL,0, 0x7fffffff, lpstrName);
}


/*************************************************************************
*      SHLWAPI_223 [SHLWAPI]
*
* NOTES
*  function guessed
*/
ODINFUNCTION1(DWORD,  SHLWAPI_223,
              HANDLE, handle)
{
  LONG oldCount;

  FIXME("(0x%08x) stub\n",handle);

  ReleaseSemaphore( handle, 1, &oldCount);
  WaitForSingleObject( handle, 0 );
  return 0;
}


/*************************************************************************
*      SHLWAPI_237 [SHLWAPI]
* Purpose: verify a WNDCLASSW structure
*/
ODINFUNCTION1(DWORD,       SHLWAPI_237,
              LPWNDCLASSW, lpWndClass)
{
  WNDCLASSW wndClass;

  if (GetClassInfoW(lpWndClass->hInstance,
                    (LPWSTR)lpWndClass->lpszClassName,
                    &wndClass) == TRUE)
    return 1;
  else
    return 0;
}


/*****************************************************************************
 * Name      : SHLWAPI_240
 * Purpose   : universal window procedure
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.240
 * Status    : COMPLETELY IMPLEMENTED ? UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION4(DWORD,  SHLWAPI_240,
              HWND,   hwnd,
              UINT,   Msg,
              WPARAM, wParam,
              LPARAM, lParam)
{
  if (IsWindowUnicode(hwnd))
    return DefWindowProcW(hwnd, Msg, wParam, lParam);
  else
    return DefWindowProcA(hwnd, Msg, wParam, lParam);
}


/*************************************************************************
*      SHLWAPI_241 [SHLWAPI]
*
*/
ODINFUNCTION0(DWORD, SHLWAPI_241)
{
  FIXME("()stub\n");
  return 0xabba1243;
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


/*************************************************************************
*      SHLWAPI_266 [SHLWAPI]
*/
ODINFUNCTION4(DWORD,  SHLWAPI_266,
              LPVOID, w,
              LPVOID, x,
              LPVOID, y,
              LPVOID, z)
{
  FIXME("(%p %p %p %p)stub\n",w,x,y,z);
  return 0xabba1248;
}

/*************************************************************************
*      SHLWAPI_267 [SHLWAPI]
*/
ODINFUNCTION4(HRESULT, SHLWAPI_267,
              LPVOID,  w, /* same as 1th parameter of SHLWAPI_219 */
              LPVOID,  x, /* same as 2nd parameter of SHLWAPI_219 */
              LPVOID,  y,
              LPVOID,  z)
{
  FIXME("(%p %p %p %p)stub\n",w,x,y,z);
  *((LPDWORD)z) = 0xabba1200;
  return 0xabba1254;
}

/*************************************************************************
*      SHLWAPI_268 [SHLWAPI]
*/
ODINFUNCTION2(DWORD,  SHLWAPI_268,
              LPVOID, w,
              LPVOID, x)
{
  FIXME("(%p %p)\n",w,x);
  return 0xabba1251; /* 0 = failure */
}

/*************************************************************************
*      SHLWAPI_276 [SHLWAPI]

* dynamically load SHELL32.DllGetVersion
*
*/
ODINFUNCTION0(DWORD, SHLWAPI_276)
{
  FIXME("()stub\n");
  return 0xabba1244;
}


/*************************************************************************
*      SHLWAPI_309 [SHLWAPI]
*
*/
ODINFUNCTION1(DWORD, SHLWAPI_309,
              LPVOID, x)
{
  FIXME("(%p)stub\n",x);
  return 0xabba1245;
}

/*************************************************************************
*      SHLWAPI_342 [SHLWAPI]
*
*/
ODINFUNCTION4(DWORD,  SHLWAPI_342,
              LPVOID, w,
              LPVOID, x,
              LPVOID, y,
              LPVOID, z)
{
  FIXME("(%p %p %p %p)stub\n",w,x,y,z);
  return 0xabba1249;
}

/*************************************************************************
*      SHLWAPI_346 [SHLWAPI]
*/
ODINFUNCTION3(DWORD,   SHLWAPI_346,
              LPCWSTR, src,
              LPWSTR,  dest,
              int,     len)
{
  FIXME("(%s %p 0x%08x)stub\n",debugstr_w(src),dest,len);
  lstrcpynW(dest, src, len);
  return lstrlenW(dest)+1;
}


/*****************************************************************************
 * Name      : SHLWAPI_364
 * Purpose   : call lstrcpynA
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.364
 * Status    : COMPLETELY IMPLEMENTED ? UNTESTED
 *
 * Author    :
 *****************************************************************************/

ODINFUNCTION3(INT,   SHLWAPI_364,
              LPSTR, lpStr1,
              LPSTR, lpStr2,
              INT,   nLength)
{
  // @@@PH is there some parameter twisting ?
  lstrcpynA(lpStr2,
            lpStr1,
            nLength);

  return 1;
}


/*****************************************************************************
 * Name      : DWORD SHLWAPI_376
 * Purpose   : Try to determine user interface language
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.366
 * Status    : PARTIALLY IMPLEMENTED UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION0(DWORD, SHLWAPI_376)
{
  /* Microsoft does a hell of a job here: looks for
   * - Internet Explorer
   * - Office
   * - Lpk ?
   * - Resource Locales
   * - etc.
   */

  return 0x0409; // @@@PH maybe this is 'nuf for now.
}



/*************************************************************************
*      SHLWAPI_377 [SHLWAPI]
*/
ODINFUNCTION3(DWORD,   SHLWAPI_377,
              LPSTR,   lpstrModule,
              HMODULE, hModule,
              LPVOID,  z)
{
  static BOOL flagSHLWAPI_377Initialized       = FALSE;
  static BOOL flagInternetExplorerCheckVersion = FALSE;

  dprintf(("not (properly) implemented.\n"));

  char szModuleName[260]; // MAXPATHLEN
  HMODULE hModLanguage;

  // initialize this subsystem
  if (flagSHLWAPI_377Initialized == FALSE)
  {
    flagSHLWAPI_377Initialized = TRUE;

    flagInternetExplorerCheckVersion =
    SHRegGetBoolUSValueA("Software\\Microsoft\\Internet Explorer\\International",
                         "CheckVersion",
                         1,
                         1);
  }

  if (lpstrModule == NULL) // Garbage in - garbage out
    return 0;

  if (0 == GetModuleFileNameA(hModule,
                              szModuleName,
                              sizeof(szModuleName)))
  {
    // treat error
  }
  else
  {
    PathRemoveFileSpecA(szModuleName);
    PathAppendA(szModuleName,
                lpstrModule);
    // @@@PH twiddle around with the current IE codepage
    hModLanguage = LoadLibraryA(szModuleName);
  }


  return hModLanguage;
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

/*************************************************************************
 *      SHCreateShellPalette	[SHLWAPI.@]
 */
HPALETTE WINAPI SHCreateShellPalette(HDC hdc)
{
	FIXME("stub\n");
	return CreateHalftonePalette(hdc);
}
