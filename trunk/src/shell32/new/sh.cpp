/* $Id: sh.cpp,v 1.2 1999-10-04 19:11:27 sandervl Exp $ */

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
#include <stdarg.h>
//#include <builtin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ICOM_CINTERFACE 1
#include "shell32_main.h"
#include <win\shlobj.h>
#include <misc.h>
//#include <nameid.h>
#include <unicode.h>


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

#define LPSHELLVIEWDATA  LPVOID
#define LPUNKNOWN        LPVOID
//#define IDSTRUCT         VOID
#define IDropTarget      VOID

static LPUNKNOWN SHELL32_IExplorerInterface=0;



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
