/* $Id: msvcrt.cpp,v 1.2 1999-10-01 16:02:32 sandervl Exp $ */

/*
 * The Visual C RunTime DLL
 * 
 * Implements Visual C run-time functionality
 *
 * Copyright 1999 Jens Wiessner
 */


#include <os2win.h>
#include <wchar.h>
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


/*********************************************************************
 *                  _getws    (MSVCRT.261)
 */
wchar_t * CDECL MSVCRT__getws( wchar_t *s )
{
  dprintf(("MSVCRT: _getws not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbbkprint    (MSVCRT.284)
 */
int CDECL MSVCRT__ismbbkprint( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbbkprint not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcalnum    (MSVCRT.290)
 */
int CDECL MSVCRT__ismbcalnum( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcalnum not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcgraph    (MSVCRT.293)
 */
int CDECL MSVCRT__ismbcgraph( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcgraph not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _ismbcpunct    (MSVCRT.302)
 */
int CDECL MSVCRT__ismbcpunct( unsigned int ch )
{
  dprintf(("MSVCRT: _ismbcpunct not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _putws    (MSVCRT.407)
 */
int CDECL MSVCRT__putws( const wchar_t *s )
{
  dprintf(("MSVCRT: _putws not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _strncoll    (MSVCRT.453)
 */
int CDECL MSVCRT__strncoll( const char *s1, const char *s2, size_t n )
{
  dprintf(("MSVCRT: _strncoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  _strnicoll    (MSVCRT.455)
 */
int CDECL MSVCRT__strnicoll( const char *s1, const char *s2, size_t n )
{
  dprintf(("MSVCRT: _strnicoll not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  fgetws    (MSVCRT.597)
 */
wchar_t * CDECL MSVCRT_fgetws( wchar_t *s, int n, FILE *strm )
{
  dprintf(("MSVCRT: fgetws\n"));
  return (fgetws(s, n, strm));
}


/*********************************************************************
 *                  fputws    (MSVCRT.605)
 */
int CDECL MSVCRT_fputws( const wchar_t *s, FILE *strm )
{
  dprintf(("MSVCRT: fputws\n"));
  return (fputws(s, strm));
}


/*********************************************************************
 *                  getwc    (MSVCRT.621)
 */
wint_t CDECL MSVCRT_getwc( FILE * strm)
{
  dprintf(("MSVCRT: getwc\n"));
  return (getwc(strm));
}


/*********************************************************************
 *                  getwchar    (MSVCRT.622)
 */
wint_t CDECL MSVCRT_getwchar( void )
{
  dprintf(("MSVCRT: getwchar\n"));
  return (getwchar());
}


/*********************************************************************
 *                  putwc    (MSVCRT.675)
 */
wint_t CDECL MSVCRT_putwc( wint_t t, FILE * strm)
{
  dprintf(("MSVCRT: putwc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


/*********************************************************************
 *                  putwchar    (MSVCRT.676)
 */
wint_t CDECL MSVCRT_putwchar( wint_t t)
{
  dprintf(("MSVCRT: putwchar not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
