/*
 * MCI stubs
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <mmsystem.h>

#include <string.h>
#include <stdio.h>

#include <misc.h>
#include <unicode.h>

#include "winmm.h"

BOOL WIN32API mciDriverNotify(HWND hwndCallback, UINT uDeviceID, UINT uStatus)
{
  dprintf(("WINMM:mciDriverNotify - stub\n"));
  return FALSE;
}

UINT WIN32API mciDriverYield(UINT uDeviceID)
{
  dprintf(("WINMM:mciDriverYield - stub\n"));
  return 0;
}

BOOL WIN32API mciExecute(LPCSTR pszCommand)
{
  dprintf(("WINMM:mciExecute(%s) - stub\n", pszCommand));
  return FALSE;
}

BOOL WIN32API mciFreeCommandResource(UINT uTable)
{
  dprintf(("WINMM:mciFreeCommandResource - stub\n"));
  return FALSE;
}

HTASK WIN32API mciGetCreatorTask(MCIDEVICEID mciId)
{
  dprintf(("WINMM:mciGetCreatorTask - stub\n"));
  return 0;
}

MCIDEVICEID WIN32API mciGetDeviceIDA(LPCSTR pszDevice)
{
  dprintf(("WINMM:mciGetDeviceIDA - stub\n"));
  return 0;
}

MCIDEVICEID WIN32API mciGetDeviceIDW(LPCWSTR pszDevice)
{
  dprintf(("WINMM:mciGetDeviceIDW - stub\n"));
  return 0;
}

MCIDEVICEID WIN32API mciGetDeviceIDFromElementIDA(DWORD dwElementID, LPCSTR lpstrType )
{
  dprintf(("WINMM:mciGetDeviceIDFromElementIDA - stub\n"));
  return 0;
}

MCIDEVICEID WIN32API mciGetDeviceIDFromElementIDW(DWORD dwElementID, LPCWSTR lpstrType )
{
  dprintf(("WINMM:mciGetDeviceIDFromElementIDW - stub\n"));
  return 0;
}

DWORD WIN32API mciGetDriverData(UINT uDeviceID)
{
  dprintf(("WINMM:mciGetDriverData - stub\n"));
  return 0;
}

BOOL WIN32API mciGetErrorStringA( MCIERROR mcierr, LPSTR pszText, UINT cchText )
{
  dprintf(("WINMM:mciGetErrorStringA(%d)\n", mcierr ));
  char * theMsg = getWinmmMsg( mcierr );
  if ( theMsg )
    strncpy( pszText, theMsg, cchText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", mcierr );
    strncpy( pszText, errMsg, cchText );
  }
  return TRUE;
}

BOOL WIN32API mciGetErrorStringW( MCIERROR mcierr, LPWSTR pszText, UINT cchText )
{
  dprintf(("WINMM:mciGetErrorStringW(%d)\n", mcierr ));
  char * theMsg = getWinmmMsg( mcierr );
  if ( theMsg )
    AsciiToUnicode( theMsg, pszText );
  else
  {
    char errMsg[100];
    sprintf( errMsg, "Unknown error number %d", mcierr );
    AsciiToUnicode( errMsg, pszText );
  }
  return TRUE;
}

YIELDPROC WIN32API mciGetYieldProc(MCIDEVICEID mciId, LPDWORD pdwYieldData)
{
  dprintf(("WINMM:mciGetYieldProc - stub\n"));
  return 0;
}

UINT WIN32API mciLoadCommandResource(HINSTANCE hInstance, LPCWSTR lpResName, UINT uType)
{
  dprintf(("WINMM:mciLoadCOmmandResource - stub\n"));
  return 0;
}

MCIERROR WIN32API mciSendCommandA(MCIDEVICEID mciId, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
  dprintf(("WINMM:mciSendCommandA - stub %X %X %X %X\n", mciId, uMsg, dwParam1, dwParam2));
  return(MMSYSERR_NODRIVER);
}

MCIERROR WIN32API mciSendCommandW(MCIDEVICEID mciId, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
  dprintf(("WINMM:mciSendCommandW - stub %X %X %X %X\n", mciId, uMsg, dwParam1, dwParam2));
  return(MMSYSERR_NODRIVER);
}

MCIERROR WIN32API mciSendStringA(LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback)
{
  dprintf(("WINMM:mciSendStringA - stub\n"));
  return(MMSYSERR_NODRIVER);
}

MCIERROR WIN32API mciSendStringW(LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback)
{
  dprintf(("WINMM:mciSendStringW - stub\n"));
  return(MMSYSERR_NODRIVER);
}

BOOL WIN32API mciSetDriverData(UINT uDeviceID, DWORD dwData)
{
  dprintf(("WINMM:mciSetDriverData - stub\n"));
  return FALSE;
}

BOOL WIN32API mciSetYieldProc(MCIDEVICEID mciId, YIELDPROC fpYieldProc, DWORD dwYieldData)
{
  dprintf(("WINMM:mciSetYieldProc - stub\n"));
  return FALSE;
}

