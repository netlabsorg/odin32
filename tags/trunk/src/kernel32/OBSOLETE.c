/* $Id: OBSOLETE.c,v 1.1 1999-05-24 20:19:42 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 misc APIs
 *
 * Copyright 1998 Sander van Leeuwen
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
