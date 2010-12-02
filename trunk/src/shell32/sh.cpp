/* $Id: sh.cpp,v 1.16 2003-11-12 14:05:13 sandervl Exp $ */

/*
 * Win32 SHELL32 for OS/2
 * Copyright 1999 Patrick Haller
 * Project Odin Software License can be found in LICENSE.TXT
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <shellapi.h>
#include <winreg.h>
#include <stdarg.h>
//#include <builtin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CINTERFACE
#include "shell32_main.h"
#include <win\shlobj.h>
#include <misc.h>
//#include <nameid.h>
#include <unicode.h>


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHELL32-SH)


#define LPSHELLVIEWDATA  LPVOID
#define LPUNKNOWN        LPVOID
//#define IDSTRUCT         VOID
#define IDropTarget      VOID

static LPUNKNOWN SHELL32_IExplorerInterface=0;





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

LPITEMIDLIST WIN32API SHBrowseForFolderW(LPBROWSEINFOW lpbi)
{
  dprintf(("SHELL32: SHBrowseForFolderW not implemented.\n"));

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

DWORD WIN32API SHEmptyRecycleBinA(DWORD x1, DWORD x2, DWORD x3)
{
  dprintf(("SHELL32: SHEmptyRecycleBinA not implemented.\n"));

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

DWORD WIN32API SHEmptyRecycleBinW(DWORD x1, DWORD x2, DWORD x3)
{
  dprintf(("SHELL32: SHEmptyRecycleBinW not implemented.\n"));

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

DWORD WIN32API SHFormatDrive(DWORD x1, DWORD x2, DWORD x3, DWORD x4)
{
  dprintf(("SHELL32: SHFormatDrive not implemented.\n"));

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

DWORD WIN32API SHGetNewLinkInfo(DWORD x1, DWORD x2, DWORD x3, DWORD x4,
                                DWORD x5)
{
  dprintf(("SHELL32: SHGetNewLinkInfo not implemented.\n"));

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

DWORD WIN32API SHHelpShortcuts_RunDLLA(DWORD x1, DWORD x2, DWORD x3,
                                       DWORD x4)
{
  dprintf(("SHELL32: SHHelpShortcuts_RunDLLA not implemented.\n"));

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

DWORD WIN32API SHHelpShortcuts_RunDLLW(DWORD x1, DWORD x2, DWORD x3,
                                       DWORD x4)
{
  dprintf(("SHELL32: SHHelpShortcuts_RunDLLW not implemented.\n"));

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

DWORD WIN32API SHQueryRecycleBinA(DWORD x1, DWORD x2)
{
  dprintf(("SHELL32: SHQueryRecycleBinA not implemented.\n"));

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

DWORD WIN32API SHQueryRecycleBinW(DWORD x1, DWORD x2)
{
  dprintf(("SHELL32: SHQueryRecycleBinW not implemented.\n"));

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

DWORD WIN32API SHUpdateRecycleBinIcon()
{
  dprintf(("SHELL32: SHUpdateRecycleBinIcon() not implemented.\n"));

  return(0);
}

