/* $Id: toolhelp.cpp,v 1.5 2006-03-25 14:03:47 sao2l02 Exp $ */

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

#define DBG_LOCALLOG	DBG_toolhelp
#include "dbglocal.h"

//ODINDEBUGCHANNEL(KERNEL32-TOOLHELP)

/***********************************************************************
 *           CreateToolHelp32Snapshot			(KERNEL32.179)
 */

HANDLE WIN32API CreateToolhelp32Snapshot(DWORD dwFlags, DWORD dwProcess)
{
  dprintf(("KERNEL32: CreateToolhelp32Snapshot %x %x not implemented, dwFlags, dwProcess"));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return INVALID_HANDLE_VALUE;
}


/***********************************************************************
 *		Process32First    (KERNEL32.555)
 *
 * Return info about the first process in a toolhelp32 snapshot
 */
BOOL WIN32API Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
  dprintf(("KERNEL32: Process32First %x %x not implemented", hSnapshot, lppe));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}

/***********************************************************************
 *		Process32Next   (KERNEL32.556)
 *
 * Return info about the "next" process in a toolhelp32 snapshot
 */
BOOL WIN32API Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
  dprintf(("KERNEL32: Process32Next %x %x not implemented", hSnapshot, lppe));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}

/***********************************************************************
 *		Module32First   (KERNEL32.527)
 *
 * Return info about the "first" module in a toolhelp32 snapshot
 */
BOOL WIN32API Module32First(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
{
  dprintf(("KERNEL32: Module32First %x %x not implemented", hSnapshot, lpme));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}

/***********************************************************************
 *		Module32Next   (KERNEL32.528)
 *
 * Return info about the "next" module in a toolhelp32 snapshot
 */
BOOL WIN32API Module32Next(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
{
  dprintf(("KERNEL32: Module32Next %x %x not implemented", hSnapshot, lpme));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}


BOOL WINAPI Thread32First(HANDLE hSnapshot,LPTHREADENTRY32 lpte)
{
  dprintf(("KERNEL32: Thread32First %x %x not implemented", hSnapshot, lpte));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}

BOOL WINAPI Thread32Next(HANDLE hSnapshot, LPTHREADENTRY32 lpte)
{
  dprintf(("KERNEL32: Thread32Next %x %x not implemented", hSnapshot, lpte));
  SetLastError( ERROR_CALL_NOT_IMPLEMENTED );
  return FALSE;
}
