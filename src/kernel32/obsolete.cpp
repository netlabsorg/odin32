/* $Id: obsolete.cpp,v 1.3 2000-02-16 14:25:43 sandervl Exp $ */

/*
 * Win32 misc APIs
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>

#define DBG_LOCALLOG	DBG_obsolete
#include "dbglocal.h"

//Api's that are obsolete, but some apps might still reference them
//(the Red Alert AUTORUN.EXE does)
/******************************************************************************/
/******************************************************************************/
BOOL WIN32API FreeResource(HGLOBAL hResData)
{
  return(TRUE);
}
/******************************************************************************/
/******************************************************************************/
