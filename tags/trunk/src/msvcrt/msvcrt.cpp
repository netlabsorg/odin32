/* $Id: msvcrt.cpp,v 1.1 1999-09-28 13:26:06 sandervl Exp $ */

/*
 * The Visual C RunTime DLL
 * 
 * Implements Visual C run-time functionality
 *
 * Copyright 1999 Jens Wiessner
 */


#include <os2win.h>
#include "debugtools.h"
#include <debugdefs.h>


DEFAULT_DEBUG_CHANNEL(msvcrt)


/*********************************************************************
 *                  _XcptFilter    (MSVCRT.21)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT__XcptFilter(DWORD ret)
{
  dprintf(("MSVCRT: _XcptFilter not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _CxxThrowException    (MSVCRT.66)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT__CxxThrowException(DWORD ret)
{
  dprintf(("MSVCRT: _CxxThrowException not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _EH_prolog    (MSVCRT.67)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT__EH_prolog(DWORD ret)
{
  dprintf(("MSVCRT: _EH_prolog not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  __CxxFrameHandler    (MSVCRT.74)
 *	FIXME - Could not find anything about it
 */
INT CDECL MSVCRT___CxxFrameHandler(DWORD ret)
{
  dprintf(("MSVCRT: __CxxFrameHandler not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
