/* $Id: she.cpp,v 1.3 1999-07-05 13:49:23 phaller Exp $ */

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
}
