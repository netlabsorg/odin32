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


/*****************************************************************************
 * Name      : HRESULT SHGetMalloc
 * Purpose   :
 * Parameters: LPMALLOC *ppMalloc
 * Variables :
 * Result    :
 * Remark    :
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
 * Name      : DWORD SHGetFileInfoA
 * Purpose   :
 * Parameters: LPCSTR      pszPath
 *             DWORD       dwFileAttributes
 *             SHFILEINFOA *psfi
 *             UINT        cbFileInfo
 *             UINT        uFlags
 * Variables :
 * Result    :
 * Remark    :
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
 * Remark    :
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
 * Name      : HRESULT SHGetDesktopFolder
 * Purpose   :
 * Parameters: LPSHELLFOLDER *ppshf
 * Variables :
 * Result    :
 * Remark    :
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
 * Name      : VOID SHAddToRecentDocs
 * Purpose   :
 * Parameters: UINT    uFlags
 *             LPCVOID pv
 * Variables :
 * Result    :
 * Remark    :
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
 * Name      : VOID SHChangeNotify
 * Purpose   :
 * Parameters: LONG    wEventId
 *             UINT    uFlags
 *             LPCVOID dwItem1
 *             LPCVOID dwItem2
 * Variables :
 * Result    :
 * Remark    :
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
 * Name      : DWORD SHFileOperationA
 * Purpose   :
 * Parameters: LPSHFILEOPSTRUCTA lpFileOp
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHFileOperationA(LPSHFILEOPSTRUCTA lpFileOp)
{
  dprintf(("SHELL32: SHFileOperationA(%08xh) not implemented.\n",
           lpFileOp));

  return(0);
}


/*****************************************************************************
 * Name      : DWORD SHFileOperationW
 * Purpose   :
 * Parameters: LPSHFILEOPSTRUCTW lpFileOp
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

DWORD WIN32API SHFileOperationW(LPSHFILEOPSTRUCTW lpFileOp)
{
  dprintf(("SHELL32: SHFileOperationW(%08xh) not implemented.\n",
           lpFileOp));

  return(0);
}


/*****************************************************************************
 * Name      : BOOL SHGetPathFromIDListA
 * Purpose   :
 * Parameters: LPCITEMIDLIST pidl
 *             LPSTR         pszPath
 * Variables :
 * Result    :
 * Remark    :
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
 * Name      : HRESULT SHGetSpecialFolderLocation
 * Purpose   :
 * Parameters: HWND         hwndOwner
 *             int          nFolder
 *             LPITEMIDLIST *ppidl
 * Variables :
 * Result    :
 * Remark    :
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
 * Name      : LPITEMIDLIST SHBrowseForFolder
 * Purpose   :
 * Parameters: LPBROWSEINFOA lpbi
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/09 20:02]
 *****************************************************************************/

LPITEMIDLIST WIN32API SHBrowseForFolder(LPBROWSEINFOA lpbi)
{
  dprintf(("SHELL32: SHBrowseForFolder(%08xh) not implemented.\n",
           lpbi));

  return(NULL);
}

