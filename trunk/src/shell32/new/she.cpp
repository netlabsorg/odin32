/* $Id: she.cpp,v 1.3 1999-10-07 10:34:47 phaller Exp $ */

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

#include <misc.h>
//#include <nameid.h>
#include <unicode.h>


/*****************************************************************************
 * Types & Defines                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHELL32-SHE)


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

ODINFUNCTION1(HRESULT, SheSetCurDrive,
              DWORD, x1)
{
  dprintf(("SHELL32: SheSetCurDrive not implemented.\n"));

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

ODINFUNCTION1(HRESULT, SheRemoveQuotesA,
              LPCSTR,  lpszString)
{
  dprintf(("SHELL32: SheRemoveQuotesA\n not implemented"));

  return 0;
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

ODINFUNCTION1(HRESULT, SheRemoveQuotesW,
              LPCWSTR, lpszString)
{
  dprintf(("SHELL32: SheRemoveQuotesW\n not implemented"));

  return 0;
}

