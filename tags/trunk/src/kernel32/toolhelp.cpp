/* $Id: toolhelp.cpp,v 1.2 2000-02-16 14:23:12 sandervl Exp $ */

/*
 * Misc Toolhelp functions
 *
 * Copyright 1996 Marcus Meissner
 * Copyright 1999 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <os2win.h>
#include <odinwrap.h>

#include "winbase.h"
#include "winerror.h"
#include "tlhelp32.h"
#include "toolhelp.h"

#define DBG_LOCALLOG	DBG_toolhelp
#include "dbglocal.h"

ODINDEBUGCHANNEL(KERNEL32-TOOLHELP)

/***********************************************************************
 *           CreateToolHelp32Snapshot			(KERNEL32.179)
 */

ODINFUNCTION2(HANDLE,CreateToolhelp32Snapshot,DWORD,dwFlags,
                                              DWORD,dwProcess)
{
  dprintf(("KERNEL32: CreateToolhelp32Snapshot not implemented.\n"));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return INVALID_HANDLE_VALUE;
}


/***********************************************************************
 *		Process32First    (KERNEL32.555)
 *
 * Return info about the first process in a toolhelp32 snapshot
 */
ODINFUNCTION2(BOOL,Process32First,HANDLE,        hSnapshot,
                                  LPPROCESSENTRY,lppe)
{
  dprintf(("KERNEL32: Process32First not implemented.\n"));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}

/***********************************************************************
 *		Process32Next   (KERNEL32.556)
 *
 * Return info about the "next" process in a toolhelp32 snapshot
 */
ODINFUNCTION2(BOOL,Process32Next,HANDLE,        hSnapshot,
                                 LPPROCESSENTRY,lppe)
{
  dprintf(("KERNEL32: Process32Next not implemented.\n"));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}

/***********************************************************************
 *		Module32First   (KERNEL32.527)
 *
 * Return info about the "first" module in a toolhelp32 snapshot
 */
ODINFUNCTION2(BOOL,Module32First,HANDLE,hSnapshot,
                                 LPVOID,lpme)
{
  //                               LPMODULEENTY,lpme)
  dprintf(("KERNEL32: Module32First not implemented.\n"));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}

/***********************************************************************
 *		Module32Next   (KERNEL32.528)
 *
 * Return info about the "next" module in a toolhelp32 snapshot
 */
ODINFUNCTION2(BOOL,Module32Next,HANDLE,hSnapshot,
                                LPVOID,lpme)
{
  //                              LPMODULEENTRY,lpme)
  dprintf(("KERNEL32: Module32Next not implemented.\n"));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}

