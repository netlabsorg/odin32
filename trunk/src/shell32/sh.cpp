/* $Id: sh.cpp,v 1.5 1999-06-25 08:07:12 phaller Exp $ */

/*
 * Win32 SHELL32 for OS/2
 * Copyright 1999 Patrick Haller
 * Project Odin Software License can be found in LICENSE.TXT
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
#include <shellapi.h>
#include <winreg.h>
//#include <shlobj.h> @@@PH shlobj is yet to be ported
#include "shell32.h"

#include <stdarg.h>
//#include <builtin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <nameid.h>
#include <unicode.h>


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

#define LPSHELLVIEWDATA  LPVOID
#define LPUNKNOWN        LPVOID
#define IDSTRUCT         VOID
#define LPSHELLFLAGSTATE LPVOID
#define IDropTarget      VOID

static LPUNKNOWN SHELL32_IExplorerInterface=0;


/*****************************************************************************
 * Name      : DWORD WIN32API SHChangeNotifyRegister
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.2
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *   Idlist is an array of structures and Count specifies how many items in the array
 *   (usually just one I think).
 */

DWORD WIN32API SHChangeNotifyRegister(HWND hwnd,
                                      LONG events1,
                                      LONG events2,
                                      DWORD msg,
                                      int count,
                                      IDSTRUCT *idlist)
{
  dprintf(("SHELL32: SHChangeNotifyRegister(%08xh,  %08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hwnd,
           events1,
           events2,
           msg,
           count,
           idlist));

  return 0;
}


/*****************************************************************************
 * Name      : DWORD WIN32API SHChangeNotifyDeregister
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.4
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 */

DWORD WIN32API SHChangeNotifyDeregister(LONG x1)
{
  dprintf(("SHELL32: SHChangeNotifyDeregister(%08xh) not implemented.\n",
           x1));

  return 0;
}


/*****************************************************************************
 * Name      : void WIN32API SHGetSettings
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.68
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *  the registry path are for win98 (tested)
 *  and possibly are the same in nt40
 */

void WIN32API SHGetSettings(LPSHELLFLAGSTATE lpsfs,
                            DWORD            dwMask,
                            DWORD            dwx)
{
  HKEY  hKey;
  DWORD dwData;
  DWORD dwDataSize = sizeof (DWORD);

  dprintf(("SHELL32: SHGetSettings(%08xh, %08xh, %08xh)\n",
           lpsfs,
           dwMask,
           dwx));

  if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
            0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, 0))
    return;

//@@@PH: todo
#if 0
   if ( (SSF_SHOWEXTENSIONS & dwMask) && !RegQueryValueExA(hKey, "HideFileExt", 0, 0, (LPBYTE)&dwData, &dwDataSize))
     lpsfs->fShowExtensions  = ((dwData == 0) ?  0 : 1);

   if ( (SSF_SHOWINFOTIP & dwMask) && !RegQueryValueExA(hKey, "ShowInfoTip", 0, 0, (LPBYTE)&dwData, &dwDataSize))
     lpsfs->fShowInfoTip  = ((dwData == 0) ?  0 : 1);

   if ( (SSF_DONTPRETTYPATH & dwMask) && !RegQueryValueExA(hKey, "DontPrettyPath", 0, 0, (LPBYTE)&dwData, &dwDataSize))
     lpsfs->fDontPrettyPath  = ((dwData == 0) ?  0 : 1);

   if ( (SSF_HIDEICONS & dwMask) && !RegQueryValueExA(hKey, "HideIcons", 0, 0, (LPBYTE)&dwData, &dwDataSize))
     lpsfs->fHideIcons  = ((dwData == 0) ?  0 : 1);

   if ( (SSF_MAPNETDRVBUTTON & dwMask) && !RegQueryValueExA(hKey, "MapNetDrvBtn", 0, 0, (LPBYTE)&dwData, &dwDataSize))
     lpsfs->fMapNetDrvBtn  = ((dwData == 0) ?  0 : 1);

   if ( (SSF_SHOWATTRIBCOL & dwMask) && !RegQueryValueExA(hKey, "ShowAttribCol", 0, 0, (LPBYTE)&dwData, &dwDataSize))
     lpsfs->fShowAttribCol  = ((dwData == 0) ?  0 : 1);

   if (((SSF_SHOWALLOBJECTS | SSF_SHOWSYSFILES) & dwMask) && !RegQueryValueExA(hKey, "Hidden", 0, 0, (LPBYTE)&dwData, &dwDataSize))
   { if (dwData == 0)
     { if (SSF_SHOWALLOBJECTS & dwMask)  lpsfs->fShowAllObjects  = 0;
       if (SSF_SHOWSYSFILES & dwMask)    lpsfs->fShowSysFiles  = 0;
     }
     else if (dwData == 1)
     { if (SSF_SHOWALLOBJECTS & dwMask)  lpsfs->fShowAllObjects  = 1;
       if (SSF_SHOWSYSFILES & dwMask)    lpsfs->fShowSysFiles  = 0;
     }
     else if (dwData == 2)
     { if (SSF_SHOWALLOBJECTS & dwMask)  lpsfs->fShowAllObjects  = 0;
       if (SSF_SHOWSYSFILES & dwMask)    lpsfs->fShowSysFiles  = 1;
     }
   }
#endif

  RegCloseKey (hKey);
}


/*****************************************************************************
 * Name      : int WIN32API SHShellFolderView_Message
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.73
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * SHShellFolderView_Message                   [SHELL32.73]
 *
 * PARAMETERS
 *  hwndCabinet defines the explorer cabinet window that contains the
 *              shellview you need to communicate with
 *  uMsg        identifying the SFVM enum to perform
 *  lParam
 *
 * NOTES
 *  Message SFVM_REARRANGE = 1
 *    This message gets sent when a column gets clicked to instruct the
 *    shell view to re-sort the item list. lParam identifies the column
 *    that was clicked.
 */

int WIN32API SHShellFolderView_Message(HWND   hwndCabinet,
                                       UINT   uMsg,
                                       LPARAM lParam)
{
  dprintf(("SHELL32: SHShellFolderView_Message(%08xh,%08xh,%08xh) not implemented.\n",
           hwndCabinet,
           uMsg,
           lParam));

  return 0;
}




/*****************************************************************************
 * Name      : DWORD WIN32API SHRegisterDragDrop
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.86
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *     exported by ordinal
 */

DWORD WIN32API SHRegisterDragDrop(HWND        hWnd,
                                  IDropTarget *pDropTarget)
{
  dprintf(("SHELL32: SHRegisterDragDrop(%08xh,%08xh)\n",
           hWnd,
           pDropTarget));

  // return RegisterDragDrop(hWnd,
  //                         pDropTarget);
  return 0;
}

/*****************************************************************************
 * Name      : DWORD WIN32API SHRevokeDragDrop
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.87
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *     exported by ordinal
 */

DWORD WIN32API SHRevokeDragDrop(DWORD x)
{
  dprintf(("SHELL32: SHRevokeDragDrop(%08xh) not implemented.\n",
           x));

  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHWaitForFileToOpen
 * Purpose   :
 * Parameters: DWORD x
 *             DWORD y
 *             DWORD z
 * Variables :
 * Result    :
 * Remark    : SHELL32.97
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHWaitForFileToOpen(DWORD u,
                                     DWORD v,
                                     DWORD w)
{
  dprintf(("SHELL32: SHWaitForFileToOpen(%08xh,%08xh,%08xh) not implemented.\n",
           u,
           v,
           w));

  return 0;
}


/*****************************************************************************
 * Name      : DWORD WIN32API SHRestricted
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.100
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * walks through policy table, queries <app> key, <type> value, returns
 * queried (DWORD) value.
 * {0x00001,Explorer,NoRun}
 * {0x00002,Explorer,NoClose}
 * {0x00004,Explorer,NoSaveSettings}
 * {0x00008,Explorer,NoFileMenu}
 * {0x00010,Explorer,NoSetFolders}
 * {0x00020,Explorer,NoSetTaskbar}
 * {0x00040,Explorer,NoDesktop}
 * {0x00080,Explorer,NoFind}
 * {0x00100,Explorer,NoDrives}
 * {0x00200,Explorer,NoDriveAutoRun}
 * {0x00400,Explorer,NoDriveTypeAutoRun}
 * {0x00800,Explorer,NoNetHood}
 * {0x01000,Explorer,NoStartBanner}
 * {0x02000,Explorer,RestrictRun}
 * {0x04000,Explorer,NoPrinterTabs}
 * {0x08000,Explorer,NoDeletePrinter}
 * {0x10000,Explorer,NoAddPrinter}
 * {0x20000,Explorer,NoStartMenuSubFolders}
 * {0x40000,Explorer,MyDocsOnNet}
 * {0x80000,WinOldApp,NoRealMode}
 *
 * NOTES
 *     exported by ordinal
 */

DWORD WIN32API SHRestricted (DWORD pol)
{
  HKEY  xhkey;

  dprintf(("SHELL32: SHRestricted(%08xh) not implemented.\n",
           pol));

  if (RegOpenKeyA(HKEY_CURRENT_USER,
                  "Software\\Microsoft\\Windows\\CurrentVersion\\Policies",
                  &xhkey))
    return 0;

  /* FIXME: do nothing for now, just return 0 (== "allowed") */
  RegCloseKey(xhkey);
  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHFlushClipboard
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.121
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHFlushClipboard(void)
{
  dprintf(("SHELL32: SHFlushClipboard() not implemented.\n"));

  return 1;
}


/*****************************************************************************
 * Name      : HRESULT SHFreeUnusedLibraries
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.123
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHFreeUnusedLibraries (void)
{
  dprintf(("SHELL32: SHFreeUnusedLibraries() not implemented.\n"));
  return TRUE;
}


/*****************************************************************************
 * Name      : HRESULT SHWinHelp
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.127
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHWinHelp (DWORD v,
                            DWORD w,
                            DWORD x,
                            DWORD z)
{
  dprintf(("SHELL32: SHWinHelp(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           v,
           w,
           x,
           z));

  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHOutOfMemoryMessageBox
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.126
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHOutOfMemoryMessageBox(DWORD u,
                                         DWORD v,
                                         DWORD w)
{
  dprintf(("SHELL32: SHOutOfmemoryMessageBox(%08xh,%08xh,%08xh) not implemented.\n",
           u,
           v,
           w));

  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHLoadOLE
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.151
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHLoadOLE(DWORD u)
{
  dprintf(("SHELL32: SHLoadOle(%08xh) not implemented.\n",
           u));

  return S_OK;
}


/*****************************************************************************
 * Name      : HRESULT SHRunControlPanel
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.161
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRunControlPanel (DWORD x,
                                    DWORD z)
{
  dprintf(("SHELL32: SHRunControlPanel(%08xh, %08xh) not implemented.\n",
           x,
           z));

  return 0;
}


/*****************************************************************************
 * Name      : DWORD WIN32API SHCreateDirectory
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.165
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *  exported by ordinal
 *  not sure about LPSECURITY_ATTRIBUTES
 */

DWORD WIN32API SHCreateDirectory(LPSECURITY_ATTRIBUTES sec,
                                 LPCSTR                path)
{
  dprintf(("SHELL32: SHCreateDirectory(%08xh, %08xh) not implemented.\n",
           sec,
           path));

 if (CreateDirectoryA(path,sec))
   return TRUE;

 /* SHChangeNotify(8,1,path,0); */
 return FALSE;

//   if (SHELL32_79(path,(LPVOID)x))
//      return 0;
//   FIXME("(%08lx,%s):stub.\n",x,path);
//   return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHValidateUNC
 * Purpose   : validating a UNC path ? so the parameters are not correct !
 * Parameters: DWORD x
 *             DWORD y
 *             DWORD z
 * Variables :
 * Result    :
 * Remark    : SHELL32.173
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHValidateUNC (DWORD x,
                                DWORD y,
                                DWORD z)
{
  dprintf(("SHELL32: SHValidateUnc(%08x,%08xh,%08xh) not implemented.\n",
           x,
           y,
           z));

 return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHCreateShellFolderViewEx
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.174
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/


HRESULT WIN32API SHCreateShellFolderViewEx(LPSHELLVIEWDATA psvcbi,
                                           LPVOID* ppv)
{
  dprintf(("SHELL32: SHCreateShellFolderViewEx(%08xh,%08xh) not implemented.\n",
           psvcbi,
           ppv));

  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHSetInstanceExplorer
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.176
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHSetInstanceExplorer (LPUNKNOWN lpUnknown)
{
  dprintf(("SHELL32: SHSetInstanceExplorer(%08xh)\n",
           lpUnknown));

  SHELL32_IExplorerInterface = lpUnknown;
  return (HRESULT) lpUnknown;
}


/*****************************************************************************
 * Name      : DWORD WIN32API SHFree
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.195
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *     free_ptr() - frees memory using IMalloc
 *     exported by ordinal
 */

DWORD WIN32API SHFree(LPVOID x)
{
  dprintf(("SHELL32: SHFree(%08xh)\n",
           x));

  if (!HIWORD(x))
  {
    *(LPDWORD)0xdeaf0000 = 0;
  }

  return HeapFree(GetProcessHeap(),0,x);
}


/*****************************************************************************
 * Name      : LPVOID WIN32API SHAlloc
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.196
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *     void *task_alloc(DWORD len), uses SHMalloc allocator
 *     exported by ordinal
 */

LPVOID WIN32API SHAlloc(DWORD len)
{
  dprintf(("SHELL32: SHAlloc(%08xh)\n",
           len));

  return (LPVOID)HeapAlloc(GetProcessHeap(),0,len);
}


/*****************************************************************************
 * Name      : HRESULT SHAbortInvokeCommand
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.198
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHAbortInvokeCommand(void)
{
  dprintf(("SHELL32: SHAbortInvokeCommand() not implemented.\n"));

  return 1;
}


/*****************************************************************************
 * Name      : VOID SHAddToRecentDocs
 * Purpose   :
 * Parameters: UINT    uFlags
 *             LPCVOID pv
 * Variables :
 * Result    :
 * Remark    : SHELL32.234
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

VOID WIN32API SHAddToRecentDocs(UINT    uFlags,
                                LPCVOID pv)
{
  dprintf(("SHELL32: SHAddToRecentDocs(%08xh,%08xh) not implemented.\n",
           uFlags,
           pv));
}


/*****************************************************************************
 * Name      : DWORD SHAppBarMessage
 * Purpose   : ... be the janitor
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.235
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHAppBarMessage(LONG   x1,
                               LPVOID x2)
{
  dprintf(("SHELL32: SHAppBarMessage(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return 0;
}


/*****************************************************************************
 * Name      : LPITEMIDLIST SHBrowseForFolder
 * Purpose   :
 * Parameters: LPBROWSEINFOA lpbi
 * Variables :
 * Result    :
 * Remark    : SHELL32.236
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

LPITEMIDLIST WIN32API SHBrowseForFolder(LPBROWSEINFOA lpbi)
{
  dprintf(("SHELL32: SHBrowseForFolder(%08xh) not implemented.\n",
           lpbi));

  //@@@PH automatically decide between A and W

  return(NULL);
}


/*****************************************************************************
 * Name      : LPITEMIDLIST SHBrowseForFolderA
 * Purpose   :
 * Parameters: LPBROWSEINFOA lpbi
 * Variables :
 * Result    :
 * Remark    : SHELL32.237
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

LPITEMIDLIST WIN32API SHBrowseForFolderA(LPBROWSEINFOA lpbi)
{
  dprintf(("SHELL32: SHBrowseForFolderA(%08xh) not implemented.\n",
           lpbi));

  return(NULL);
}


/*****************************************************************************
 * Name      : LPITEMIDLIST SHBrowseForFolderW
 * Purpose   :
 * Parameters: LPBROWSEINFOW lpbi
 * Variables :
 * Result    :
 * Remark    : SHELL32.238
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

LPITEMIDLIST WIN32API SHBrowseForFolderW(LPBROWSEINFOA lpbi)
{
  dprintf(("SHELL32: SHBrowseForFolderW(%08xh) not implemented.\n",
           lpbi));

  return(NULL);
}


/*****************************************************************************
 * Name      : VOID SHChangeNotify
 * Purpose   :
 * Parameters: LONG    wEventId
 *             UINT    uFlags
 *             LPCVOID dwItem1
 *             LPCVOID dwItem2
 * Variables :
 * Result    :
 * Remark    : SHELL32.239
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

VOID WIN32API SHChangeNotify(LONG    wEventId,
                             UINT    uFlags,
                             LPCVOID dwItem1,
                             LPCVOID dwItem2)
{
  dprintf(("SHELL32: SHChangeNotify(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           wEventId,
           uFlags,
           dwItem1,
           dwItem2));

}


/*****************************************************************************
 * Name      : DWORD SHEmptyRecycleBinA
 * Purpose   : ... be the janitor
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.240
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHEmptyRecycleBinA(DWORD x1,
                                  DWORD x2,
                                  DWORD x3)
{
  dprintf(("SHELL32: SHEmptyRecycleBinA(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return 0;
}


/*****************************************************************************
 * Name      : DWORD SHEmptyRecycleBinW
 * Purpose   : ... be the janitor
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.241
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHEmptyRecycleBinW(DWORD x1,
                                  DWORD x2,
                                  DWORD x3)
{
  dprintf(("SHELL32: SHEmptyRecycleBinW(%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3));

  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHFileOperation
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.242
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHFileOperationAW(LPVOID lpFileOp)
{
  dprintf(("SHELL32: SHFileOperationAW(%08xh) not implemented.\n",
           lpFileOp));

  // if OsIsUnicode()
  //   SHFileOperationA(lpFileOp);
  // else
  //   SHFileOperationW(lpFileOp);

  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHFileOperationA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.243
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHFileOperationA(LPSHFILEOPSTRUCTA lpFileOp)
{
  dprintf(("SHELL32: SHFileOperationA(%08xh) not implemented.\n",
           lpFileOp));

  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHFileOperationW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.244
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHFileOperationW(LPSHFILEOPSTRUCTW lpFileOp)
{
  dprintf(("SHELL32: SHFileOperationW(%08xh) not implemented.\n",
           lpFileOp));

  return 0;
}


/*****************************************************************************
 * Name      : DWORD SHFormatDrive
 * Purpose   : format a drive ? ;-)
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.245
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHFormatDrive(DWORD x1,
                             DWORD x2,
                             DWORD x3,
                             DWORD x4)
{
  dprintf(("SHELL32: SHFormatDrive(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return 0;
}


/*****************************************************************************
 * Name      : DWORD SHFreeNameMappings
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.246
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHFreeNameMappings(DWORD x1)
{
  dprintf(("SHELL32: SHFreeNameMappings(%08xh) not implemented.\n",
           x1));

  return 0;
}


/*****************************************************************************
 * Name      : DWORD SHGetDataFromIDListA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.247
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHGetDataFromIDListA(LPVOID x1,
                                    LPVOID x2,
                                    DWORD  x3,
                                    LPVOID x4,
                                    DWORD  x5)
{
  dprintf(("SHELL32: SHGetDataFromIDListA(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return 0;
}


/*****************************************************************************
 * Name      : DWORD SHGetDataFromIDListW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.248
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHGetDataFromIDListW(LPVOID x1,
                                    LPVOID x2,
                                    DWORD  x3,
                                    LPVOID x4,
                                    DWORD  x5)
{
  dprintf(("SHELL32: SHGetDataFromIDListW(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return 0;
}


/*****************************************************************************
 * Name      : HRESULT SHGetDesktopFolder
 * Purpose   :
 * Parameters: LPSHELLFOLDER *ppshf
 * Variables :
 * Result    :
 * Remark    : SHELL32.252
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHGetDesktopFolder(LPSHELLFOLDER *ppshf)
{
  dprintf(("SHELL32: SHGetDesktopFolder(%08xh) not implemented.\n",
           ppshf));

  return(1);
}



/*****************************************************************************
 * Name      : DWORD SHGetFileInfoA
 * Purpose   :
 * Parameters: LPCSTR      pszPath
 *             DWORD       dwFileAttributes
 *             SHFILEINFOA *psfi
 *             UINT        cbFileInfo
 *             UINT        uFlags
 * Variables :
 * Result    :
 * Remark    : SHELL32.253 .254
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHGetFileInfoA(LPCSTR      pszPath,
                              DWORD       dwFileAttributes,
                              SHFILEINFOA *psfi,
                              UINT        cbFileInfo,
                              UINT        uFlags)
{
  dprintf(("SHELL32: SHGetFileInfoA (%s,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           pszPath,
           dwFileAttributes,
           psfi,
           cbFileInfo,
           uFlags));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHGetFileInfoW
 * Purpose   :
 * Parameters: LPWSTR      pszPath
 *             DWORD       dwFileAttributes
 *             SHFILEINFOA *psfi
 *             UINT        cbFileInfo
 *             UINT        uFlags
 * Variables :
 * Result    :
 * Remark    : SHELL32.255
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHGetFileInfoW(LPCWSTR         pszPath,
                              DWORD           dwFileAttributes,
                              SHFILEINFOW     *psfi,
                              UINT            cbFileInfo,
                              UINT            uFlags)
{
  char *astring = UnicodeToAsciiString((LPWSTR)pszPath);

  dprintf(("SHELL32: SHGetFileInfoW (%s,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           astring,
           dwFileAttributes,
           psfi,
           cbFileInfo,
           uFlags));

  FreeAsciiString(astring);
  return(0);
}


/*****************************************************************************
 * Name      : HRESULT SHGetInstanceExplorer
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.256
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHGetInstanceExplorer (LPUNKNOWN * lpUnknown)
{
  dprintf(("SHELL32: SHGetInstanceExplorer(%08xh) not implemented.\n",
           lpUnknown));

  *lpUnknown = SHELL32_IExplorerInterface;

  if (!SHELL32_IExplorerInterface)
    return E_FAIL;

  //@@@PH to do:
  //SHELL32_IExplorerInterface->lpvtbl->fnAddRef(SHELL32_IExplorerInterface);
  return NOERROR;
}


/*****************************************************************************
 * Name      : HRESULT SHGetMalloc
 * Purpose   :
 * Parameters: LPMALLOC *ppMalloc
 * Variables :
 * Result    :
 * Remark    : SHELL32.257
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHGetMalloc(LPMALLOC *ppMalloc)
{
  dprintf(("SHELL32: SHGetMalloc(%08xh) not implemented.\n",
           ppMalloc));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHGetNewLinkInfo
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.258
 * Status    : UNTESTED STUB UNKNOWN
 *
 * Author    : Patrick Haller [Tue, 1998/06/15 03:00]
 *****************************************************************************/

DWORD WIN32API SHGetNewLinkInfo(DWORD x1,
                                DWORD x2,
                                DWORD x3,
                                DWORD x4,
                                DWORD x5)
{
  dprintf(("SHELL32: SHGetNewLinkInfo(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4,
           x5));

  return 0;
}


/*****************************************************************************
 * Name      : BOOL SHGetPathFromIDListA
 * Purpose   :
 * Parameters: LPCITEMIDLIST pidl
 *             LPSTR         pszPath
 * Variables :
 * Result    :
 * Remark    : SHELL32.259 .260
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

BOOL WIN32API SHGetPathFromIDListA(LPCITEMIDLIST pidl,
                                   LPSTR         pszPath)
{
  dprintf(("SHELL32: SHGetPathFromIDListA(%08xh,%s) not implemented.\n",
           pidl,
           pszPath));

  return(FALSE);
}


/*****************************************************************************
 * Name      : BOOL SHGetPathFromIDListW
 * Purpose   :
 * Parameters: LPCITEMIDLIST pidl
 *             LPSTR         pszPath
 * Variables :
 * Result    :
 * Remark    : SHELL32.261
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

BOOL WIN32API SHGetPathFromIDListW(LPCITEMIDLIST pidl,
                                   LPWSTR        pszPath)
{
  dprintf(("SHELL32: SHGetPathFromIDListW(%08xh,%08xh) not implemented.\n",
           pidl,
           pszPath));

  return(FALSE);
}


/*****************************************************************************
 * Name      : HRESULT SHGetSpecialFolderLocation
 * Purpose   :
 * Parameters: HWND         hwndOwner
 *             int          nFolder
 *             LPITEMIDLIST *ppidl
 * Variables :
 * Result    :
 * Remark    : SHELL32.262
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHGetSpecialFolderLocation(HWND         hwndOwner,
                                            int          nFolder,
                                            LPITEMIDLIST * ppidl)
{
  dprintf(("SHELL32: SHGetSpecialFolderLocation(%08xh,%08xh,%08xh) not implemented.\n",
           hwndOwner,
           nFolder,
           ppidl));

  return(1);
}


/*****************************************************************************
 * Name      : DWORD SHHelpShortcuts_RunDLL
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.263
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHHelpShortcuts_RunDLL(DWORD x1,
                                      DWORD x2,
                                      DWORD x3,
                                      DWORD x4)
{
  dprintf(("SHELL32: SHHelpShortcuts_RunDLL(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHHelpShortcuts_RunDLLA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.264
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHHelpShortcuts_RunDLLA(DWORD x1,
                                       DWORD x2,
                                       DWORD x3,
                                       DWORD x4)
{
  dprintf(("SHELL32: SHHelpShortcuts_RunDLLA(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHHelpShortcuts_RunDLLW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.265
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHHelpShortcuts_RunDLLW(DWORD x1,
                                       DWORD x2,
                                       DWORD x3,
                                       DWORD x4)
{
  dprintf(("SHELL32: SHHelpShortcuts_RunDLLW(%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           x1,
           x2,
           x3,
           x4));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHLoadInProc
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.267
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHLoadInProc(DWORD x1)
{
  dprintf(("SHELL32: SHLoadInProc(%08xh) not implemented.\n",
           x1));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHQueryRecycleBinA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.268
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHQueryRecycleBinA(DWORD x1,
                                  DWORD x2)
{
  dprintf(("SHELL32: SHQueryRecycleBinA(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHQueryRecycleBinW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.269
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHQueryRecycleBinW(DWORD x1,
                                  DWORD x2)
{
  dprintf(("SHELL32: SHQueryRecycleBinW(%08xh,%08xh) not implemented.\n",
           x1,
           x2));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHUpdateRecycleBinIcon
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.269
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHUpdateRecycleBinIcon(void)
{
  dprintf(("SHELL32: SHUpdateRecycleBinIcon() not implemented.\n"));

  return(0);
}


/*****************************************************************************
 * Name      : HRESULT SHRegCloseKey
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.505
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegCloseKey (HKEY hkey)
{
  dprintf(("SHELL32: SHRegCloseKey(%08xh)\n",
           hkey));

  return RegCloseKey( hkey );
}


/*****************************************************************************
 * Name      : HRESULT SHRegOpenKeyA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.506
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegOpenKeyA(HKEY  hKey,
                               LPSTR lpSubKey,
                               LPHKEY phkResult)
{
  dprintf(("SHELL32: SHRegOpenKeyA(%08xh,%s,%08xh)\n",
           hKey,
           lpSubKey,
           phkResult));

 return RegOpenKeyA(hKey,
                    lpSubKey,
                    phkResult);
}


/*****************************************************************************
 * Name      : HRESULT SHRegOpenKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.507
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegOpenKeyW (HKEY    hKey,
                                LPCWSTR lpszSubKey,
                                LPHKEY  phkResult)
{
  dprintf(("SHELL32: SHRegOpenKeyW(%08xh,%08xh,%08xh)\n",
           hKey,
           lpszSubKey,
           phkResult));

 return RegOpenKeyW(hKey,
                    lpszSubKey,
                    phkResult);
}


/*****************************************************************************
 * Name      : HRESULT SHRegQueryValueA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.508
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegQueryValueA(HKEY   hKey,
                                  LPSTR  lpszSubKey,
                                  LPSTR  lpszData,
                                  LPLONG lpcbData)
{
  dprintf(("SHELL32: SHRegQueryValueW(%08xh,%s,%08xh,%08xh)\n",
           hKey,
           lpszSubKey,
           lpszData,
           lpcbData));

  return (HRESULT)RegQueryValueA(hKey,
                                 lpszSubKey,
                                 lpszData,
                                 lpcbData);
}


/*****************************************************************************
 * Name      : HRESULT SHRegQueryValueExA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.509
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegQueryValueExA(DWORD   u,
                                    LPSTR   v,
                                    LPDWORD w,
                                    LPDWORD x,
                                    LPBYTE  y,
                                    LPDWORD z)
{
  dprintf(("SHELL32: SHRegQueryValueExA(%08xh,%s,%08xh,%08xh,%08xh,%08xh)\n",
           u,
           v,
           w,
           x,
           y,
           z));

  return (HRESULT)RegQueryValueExA(u,
                                   v,
                                   w,
                                   x,
                                   y,
                                   z);
}


/*****************************************************************************
 * Name      : HRESULT SHRegQueryValueExW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.510
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegQueryValueExW(DWORD    u,
                                    LPWSTR   v,
                                    LPDWORD  w,
                                    LPDWORD  x,
                                    LPBYTE   y,
                                    LPDWORD  z)
{
  dprintf(("SHELL32: SHRegQueryValueExW(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh)\n",
           u,
           v,
           w,
           x,
           y,
           z));

  return (HRESULT)RegQueryValueExW(u,
                                   v,
                                   w,
                                   x,
                                   y,
                                   z);
}


/*****************************************************************************
 * Name      : HRESULT SHRegQueryValueW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.511
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegQueryValueW(HKEY   hKey,
                                  LPWSTR lpszSubKey,
                                  LPWSTR lpszData,
                                  LPLONG lpcbData)
{
  dprintf(("SHELL32: SHRegQueryValueW(%08xh,%08xh,%08xh,%08xh)\n",
           hKey,
           lpszSubKey,
           lpszData,
           lpcbData));

  return (HRESULT)RegQueryValueW(hKey,
                                 lpszSubKey,
                                 lpszData,
                                 lpcbData);
}


/*****************************************************************************
 * Name      : HRESULT SHRegDeleteKeyW
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.512
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegDeleteKeyW (HKEY   hkey,
                                  LPWSTR lpszKey)
{
  dprintf(("SHELL32: SHRegDeleteKeyW(%08xh,%08xh)\n",
           hkey,
           lpszKey));

  return RegDeleteKeyW(hkey,
                       lpszKey);
}


/*****************************************************************************
 * Name      : HRESULT SHRegDeleteKeyA
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.513
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

HRESULT WIN32API SHRegDeleteKeyA (HKEY  hkey,
                                  LPSTR lpszKey)
{
  dprintf(("SHELL32: SHRegDeleteKeyA(%08xh,%s)\n",
           hkey,
           lpszKey));

  return RegDeleteKeyA(hkey,
                       lpszKey);
}


/*****************************************************************************
 * Name      : HGLOBAL SHAllocShared
 * Purpose   : allocate shared memory ?
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.520
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *  parameter1 is return value from HeapAlloc
 *  parameter2 is equal to the size allocated with HeapAlloc
 *  parameter3 is return value from GetCurrentProcessId
 *
 *  the return value is posted as lParam with 0x402 (WM_USER+2) to somewhere
 *  WM_USER+2 could be the undocumented CWM_SETPATH
 *  the allocated memory contains a pidl
 */

HGLOBAL WIN32API SHAllocShared(LPVOID psrc,
                               DWORD  size,
                               DWORD  procID)
{
  HGLOBAL hmem;
  LPVOID pmem;

  dprintf(("SHELL32: SHAllocShared(%08xh,%08xh,%08xh)\n",
           psrc,
           size,
           procID));

  hmem = GlobalAlloc(GMEM_FIXED,
                     size);
  if (!hmem)
    return 0;

  pmem = GlobalLock (hmem);
  if (! pmem)
    return 0;

  memcpy (pmem, psrc, size);
  GlobalUnlock(hmem);
  return hmem;
}


/*****************************************************************************
 * Name      : LPVOID SHLockShared
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.521
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *  parameter1 is return value from SHAllocShared
 *  parameter2 is return value from GetCurrentProcessId
 *  the receiver of (WM_USER+2) trys to lock the HANDLE (?)
 *  the returnvalue seems to be a memoryadress
 */
LPVOID WIN32API SHLockShared(HANDLE hmem,
                             DWORD  procID)
{
  dprintf(("SHELL32: SHLockShared(%08xh,%08xh).\n",
           hmem,
           procID));

  return GlobalLock(hmem);
}


/*****************************************************************************
 * Name      : LPVOID SHUnlockShared
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.522
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *  parameter1 is return value from SHLockShared
 */

BOOL WIN32API SHUnlockShared(HANDLE pmem)
{
  dprintf(("SHELL32: SHUnlockShared(%08xh)\n",
           pmem));

  return GlobalUnlock(pmem);
}


/*****************************************************************************
 * Name      : LPVOID SHFreeShared
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.523
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *  parameter1 is return value from SHAllocShared
 *  parameter2 is return value from GetCurrentProcessId
 */

HANDLE WIN32API SHFreeShared(HANDLE hmem,
                             DWORD  procID)
{
  dprintf(("SHELL32: SHFreeShared(%08xh,%08xh)\n",
           hmem,
           procID));

  return GlobalFree(hmem);
}


/*****************************************************************************
 * Name      : DWORD WIN32API NTSHChangeNotifyRegister
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.640
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *
 * NOTES
 *   Idlist is an array of structures and Count specifies how many items in the array
 *   (usually just one I think).
 */

DWORD WIN32API NTSHChangeNotifyRegister(HWND hwnd,
                                        LONG events1,
                                        LONG events2,
                                        DWORD msg,
                                        int count,
                                        IDSTRUCT *idlist)
{
  dprintf(("SHELL32: NTSHChangeNotifyRegister(%08xh,  %08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hwnd,
           events1,
           events2,
           msg,
           count,
           idlist));

   return SHChangeNotifyRegister(hwnd,
                                 events1,
                                 events2,
                                 msg,
                                 count,
                                 idlist);
}


/*****************************************************************************
 * Name      : DWORD WIN32API NTSHChangeNotifyDeregister
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : SHELL32.641
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 */
DWORD WIN32API NTSHChangeNotifyDeregister(LONG x1)
{
  dprintf(("SHELL32: NTSHChangeNotifyDeregister(%08xh) not implemented.\n",
           x1));

  return SHChangeNotifyDeregister(x1);
}
