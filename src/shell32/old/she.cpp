/* $Id: she.cpp,v 1.1 1999-10-09 11:11:31 sandervl Exp $ */

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
#include <unicode.h>


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/


/*****************************************************************************
 * Name      : HRESULT SheSetCurDrive
 * Purpose   :
 * Parameters: unknown
 * Variables :
 * Result    :
 * Remark    : SHELL32.285
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1999/06/01 09:00]
 *****************************************************************************/

HRESULT WIN32API SheSetCurDrive(DWORD x1)
{
  dprintf(("SHELL32: SheSetCurDrive(%08xh) not implemented.\n",
           x1));

  return 0;
}

/*****************************************************************************
 * Name      : SheRemoveQuotesA
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.282 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API SheRemoveQuotesA(VOID)
{
  dprintf(("SHELL32: undoc SheRemoveQuotesA\n"));
}

/*****************************************************************************
 * Name      : SheRemoveQuotesW
 * Purpose   :
 * Parameters: ???
 * Variables :
 * Result    :
 * Remark    : SHELL32.283 - used by progman.exe
 * Status    : UNTESTED UNKNOWN STUB
 *
 * Author    : Christoph Bratschi [Fri, 1999/08/6 19:00]
 *****************************************************************************/

void WIN32API SheRemoveQuotesW(VOID)
{
  dprintf(("SHELL32: undoc SheRemoveQuotesW\n"));
}
