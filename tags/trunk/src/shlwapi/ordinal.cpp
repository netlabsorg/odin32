/* $Id: ordinal.cpp,v 1.1 2000-06-11 08:44:53 phaller Exp $ */

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

#include "shellapi.h"
#include "shlobj.h"
#include "wine/undocshell.h"

#include "shlwapi.h"


/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHLWAPI-ORDINAL)


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

HANDLE WIN32API SHLWAPI_11(HANDLE, DWORD, DWORD, DWORD, DWORD);
DWORD  WIN32API SHLWAPI_95(HWND, LPWSTR, int);


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
 * Status    : UNTESTED
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
 * Status    : UNTESTED
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
 * Status    : UNTESTED
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
 * Status    : UNTESTED
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
 * Name      : SHLWAPI_23
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION3(DWORD,SHLWAPI_23,
              DWORD,arg0,
              DWORD,arg1,
              DWORD,arg2)
{
  dprintf(("not yet implemented"));

  return 0;
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
 * Name      : LRESULT CallWindowProcAW
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.37
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION5(LRESULT,      CallWindowProcAW,
              WNDPROC,      lpPrevWndFunc,
              HWND,         hWnd,
              UINT,         Msg,
              WPARAM,       wParam,
              LPARAM,       lParam)
{
  if (VERSION_OsIsUnicode())
    return CallWindowProcW(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
  else
    return CallWindowProcA(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}


/*****************************************************************************
 * Name      : SHLWAPI_38
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(DWORD,SHLWAPI_38,
              DWORD,arg0)
{
  dprintf(("not yet implemented"));

  return 0;
}


/*****************************************************************************
 * Name      : SHLWAPI_40
 * Purpose   : looks like CharNextW
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(LPWSTR, SHLWAPI_40,
              LPWSTR, lpChar)
{
   if (*lpChar == 0)
     return lpChar;
   else
     return lpChar++;
}


/*****************************************************************************
 * Name      : SHLWAPI_43
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(DWORD,SHLWAPI_43,
              DWORD,arg0)
{
  dprintf(("not yet implemented"));

  return 0;
}


/*****************************************************************************
 * Name      : SHLWAPI_53
 * Purpose   : Character base independend CreateFontIndirect
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(DWORD,            SHLWAPI_53,
              CONST LOGFONTW *, lplf)
{
  if (VERSION_OsIsUnicode())
    return CreateFontIndirectW(lplf);
  else
  {
    // Original:
    // convert lplf to LOGFONTA
    // call CreateFontIndirectA
    return CreateFontIndirectW(lplf);
  }
}


/*****************************************************************************
 * Name      : LRESULT sDefWindowProcAW
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.56 - this procedure has static unicode behaviour
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION4(LRESULT,      sDefWindowProcAW,
              HWND,         hWnd,
              UINT,         Msg,
              WPARAM,       wParam,
              LPARAM,       lParam)
{
  if (VERSION_OsIsUnicode())
    return DefWindowProcW(hWnd, Msg, wParam, lParam);
  else
    return DefWindowProcA(hWnd, Msg, wParam, lParam);
}


/*****************************************************************************
 * Name      : SHLWAPI_59
 * Purpose   : DialogBoxParamAW
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION5(DWORD,     SHLWAPI_59,
              HINSTANCE, hInstance,
              LPCSTR,    lpTemplateName,
              HWND,      hWndParent,
              DLGPROC,   lpDialogFunc,
              LPARAM,    dwInitParam)
{
  // PH Note: originally, here are some heavy hooks into Internet Explorer
  // and tons of language support stuff.
  
  if (VERSION_OsIsUnicode())
    return DialogBoxParamW(hInstance,
                           (LPCWSTR)lpTemplateName,
                           hWndParent,
                           lpDialogFunc,
                           dwInitParam);
  else
    return DialogBoxParamA(hInstance,
                           lpTemplateName,
                           hWndParent,
                           lpDialogFunc,
                           dwInitParam);
}


/*****************************************************************************
 * Name      : LONG DispatchMessage
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.60
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(LONG,       DispatchMessageAW,
              const MSG*, lpmsg)
{
  if (VERSION_OsIsUnicode())
    return DispatchMessageW(lpmsg);
  else
    return DispatchMessageA(lpmsg);
}


/*****************************************************************************
 * Name      : SHLWAPI_74
 * Purpose   : Sort of GetDlgItemText
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION4(DWORD,     SHLWAPI_74,
              HWND,      hDlg,
              int,       nIDDlgItem,
              LPWSTR,    lpText,
              int,       nMaxCount)
{
  HWND hwndItem = GetDlgItem(hDlg,
                             nIDDlgItem);
  if (hwndItem == NULL)
  {
    if (nMaxCount != 0)  // PH Note: bug in original windows code
      *lpText = 0;
    
    return NULL;
  }
  
  // else retrieve the text
  return SHLWAPI_95(hwndItem,
                    lpText,
                    nMaxCount);
}


/*****************************************************************************
 * Name      : HMODULE GetModuleHandleAW
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.83 - check back with original code, result may be wrong
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION1(HMODULE, GetModuleHandleAW,
              LPWSTR,  lpModuleName)
{
  if (VERSION_OsIsUnicode())
    return GetModuleHandleW(lpModuleName);
  else
  {
    dprintf(("error: unicode to ascii conversion missing"));
    return GetModuleHandleA((LPSTR)lpModuleName);
  }
}


/*****************************************************************************
 * Name      : LONG GetWindowLongAW
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.94
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION2(LONG,    GetWindowLongAW,
              HWND,    hWnd,
              int,     nIndex)
{
  if (VERSION_OsIsUnicode())
    return GetWindowLongW(hWnd, nIndex);
  else
    return GetWindowLongA(hWnd, nIndex);
}


/*****************************************************************************
 * Name      : SHLWAPI_95
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION3(DWORD,     SHLWAPI_95,
              HWND,      hwndItem,
              LPWSTR,    lpText,
              int,       nMaxCount)
{
  dprintf(("not properly implemented.\n"));
  
  // @@@PH too simple, original deals with some properties and stuff
  if (VERSION_OsIsUnicode())
    return GetWindowTextW(hwndItem,
                          lpText,
                          nMaxCount);
  else
  {
    // @@@PH unicode conversion is missing
    return GetWindowTextA(hwndItem,
                          (LPSTR)lpText,
                          nMaxCount);
  }
}


/*****************************************************************************
 * Name      : HCURSOR LoadCursorAW
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.102
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION2(HCURSOR,   LoadCursorAW,
              HINSTANCE, hInstance,
              LPCSTR,    lpCursorName)
{
  if (VERSION_OsIsUnicode())
    return LoadCursorW(hInstance, (LPCWSTR)lpCursorName);
  else
    return LoadCursorA(hInstance, lpCursorName);
}


/*****************************************************************************
 * Name      : BOOL PeekMessageAW
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.116
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION5(BOOL,    PeekMessageAW,
              LPMSG,   lpMsg,
              HWND,    hWnd,
              UINT,    uMsgFilterMin,
              UINT,    uMsgFilterMax,
              UINT,    wRemoveMsg)
{
  if (VERSION_OsIsUnicode())
    return PeekMessageW(lpMsg, hWnd, uMsgFilterMin, uMsgFilterMax, wRemoveMsg);
  else
    return PeekMessageA(lpMsg, hWnd, uMsgFilterMin, uMsgFilterMax, wRemoveMsg);
}


/*****************************************************************************
 * Name      : BOOL PostMessageAW
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.117
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION4(BOOL,    PostMessageAW,
              HWND,    hWnd,
              UINT,    Msg,
              WPARAM,  wParam,
              LPARAM,  lParam)
{
  if (VERSION_OsIsUnicode())
    return PostMessageW(hWnd, Msg, wParam, lParam);
  else
    return PostMessageA(hWnd, Msg, wParam, lParam);
}


/*****************************************************************************
 * Name      : DWORD SHLWAPI_158
 * Purpose   : Case-ignoring wide string compare
 * Parameters: LPSTR lpStr1
 *             LPSTR lpStr2
 * Variables :
 * Result    : case-insensitive comparsion result between the two strings
 * Remark    : SHLWAPI.158
 * Status    : UNTESTED
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
 * Name      : BOOL SHLWAPI_197
 * Purpose   : Set text background?
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.197
 * Status    : UNTESTED
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
 * Name      : LRESULT DefWindowProcAW
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : SHLWAPI.240 - this version has dynamic unicode behaviour
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Sun, 2000/06/10 04:02]
 *****************************************************************************/

ODINFUNCTION4(LRESULT,      DefWindowProcAW,
              HWND,         hWnd,
              UINT,         Msg,
              WPARAM,       wParam,
              LPARAM,       lParam)
{
  if (IsWindowUnicode(hWnd))
    return DefWindowProcW(hWnd, Msg, wParam, lParam);
  else
    return DefWindowProcA(hWnd, Msg, wParam, lParam);
}
