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
