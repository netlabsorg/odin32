/* $Id: mci.cpp,v 1.5 2000-02-17 14:09:31 sandervl Exp $ */

/*
 * MCI stubs
 *
 * Copyright 1998 Joel Troster
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <string.h>
#include <stdio.h>

#include <misc.h>
#include <unicode.h>

#include "winmm.h"

#define DBG_LOCALLOG	DBG_mci
#include "dbglocal.h"

ODINDEBUGCHANNEL(WINMM-MCI)


ODINFUNCTION3(BOOL, mciDriverNotify,
              HWND, hwndCallback,
              UINT, uDeviceID,
              UINT, uStatus)
{
  dprintf(("WINMM:mciDriverNotify - stub\n"));
  return FALSE;
}

ODINFUNCTION1(UINT, mciDriverYield,
              UINT, uDeviceID)
{
  dprintf(("WINMM:mciDriverYield - stub\n"));
  return 0;
}

ODINFUNCTION1(BOOL, mciExecute,
              LPCSTR, pszCommand)
{
  dprintf(("WINMM:mciExecute(%s) - stub\n", pszCommand));
  return FALSE;
}

ODINFUNCTION1(BOOL, mciFreeCommandResource,
              UINT, uTable)
{
  dprintf(("WINMM:mciFreeCommandResource - stub\n"));
  return FALSE;
}

ODINFUNCTION1(HTASK, mciGetCreatorTask,
              MCIDEVICEID, mciId)
{
  dprintf(("WINMM:mciGetCreatorTask - stub\n"));
  return 0;
}

ODINFUNCTION1(MCIDEVICEID, mciGetDeviceIDA,
              LPCSTR, pszDevice)
{
  dprintf(("WINMM:mciGetDeviceIDA - stub\n"));
  return 0;
}

ODINFUNCTION1(MCIDEVICEID, mciGetDeviceIDW,
              LPCWSTR, pszDevice)
{
  dprintf(("WINMM:mciGetDeviceIDW - stub\n"));
  return 0;
}

ODINFUNCTION2(MCIDEVICEID, mciGetDeviceIDFromElementIDA,
              DWORD, dwElementID,
              LPCSTR, lpstrType)
{
  dprintf(("WINMM:mciGetDeviceIDFromElementIDA - stub\n"));
  return 0;
}

ODINFUNCTION2(MCIDEVICEID, mciGetDeviceIDFromElementIDW,
              DWORD, dwElementID,
              LPCWSTR, lpstrType)
{
  dprintf(("WINMM:mciGetDeviceIDFromElementIDW - stub\n"));
  return 0;
}

ODINFUNCTION1(DWORD, mciGetDriverData,
              UINT, uDeviceID)
{
  dprintf(("WINMM:mciGetDriverData - stub\n"));
  return 0;
}

ODINFUNCTION3(BOOL, mciGetErrorStringA,
              MCIERROR, mcierr,
              LPSTR, pszText,
              UINT, cchText)
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

ODINFUNCTION3(BOOL, mciGetErrorStringW,
              MCIERROR, mcierr,
              LPWSTR, pszText,
              UINT, cchText)
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

ODINFUNCTION2(YIELDPROC, mciGetYieldProc,
              MCIDEVICEID, mciId,
              LPDWORD, pdwYieldData)
{
  dprintf(("WINMM:mciGetYieldProc - stub\n"));
  return 0;
}

ODINFUNCTION3(UINT, mciLoadCommandResource,
              HINSTANCE, hInstance,
              LPCWSTR, lpResName,
              UINT, uType)
{
  dprintf(("WINMM:mciLoadCOmmandResource - stub\n"));
  return 0;
}

ODINFUNCTION4(MCIERROR, mciSendCommandA,
              MCIDEVICEID, mciId,
              UINT, uMsg,
              DWORD, dwParam1,
              DWORD, dwParam2)
{
  dprintf(("WINMM:mciSendCommandA - stub %X %X %X %X\n", mciId, uMsg, dwParam1, dwParam2));
  return(MMSYSERR_NODRIVER);
}

ODINFUNCTION4(MCIERROR, mciSendCommandW,
              MCIDEVICEID, mciId,
              UINT, uMsg,
              DWORD, dwParam1,
              DWORD, dwParam2)
{
  dprintf(("WINMM:mciSendCommandW - stub %X %X %X %X\n", mciId, uMsg, dwParam1, dwParam2));
  return(MMSYSERR_NODRIVER);
}

ODINFUNCTION4(MCIERROR, mciSendStringA,
              LPCSTR, lpstrCommand,
              LPSTR, lpstrReturnString,
              UINT, uReturnLength,
              HWND, hwndCallback)
{
  dprintf(("WINMM:mciSendStringA - stub\n"));
  return(MMSYSERR_NODRIVER);
}

ODINFUNCTION4(MCIERROR, mciSendStringW,
              LPCWSTR, lpstrCommand,
              LPWSTR, lpstrReturnString,
              UINT, uReturnLength,
              HWND, hwndCallback)
{
  dprintf(("WINMM:mciSendStringW - stub\n"));
  return(MMSYSERR_NODRIVER);
}

ODINFUNCTION2(BOOL, mciSetDriverData,
              UINT, uDeviceID,
              DWORD, dwData)
{
  dprintf(("WINMM:mciSetDriverData - stub\n"));
  return FALSE;
}

ODINFUNCTION3(BOOL, mciSetYieldProc,
              MCIDEVICEID, mciId,
              YIELDPROC, fpYieldProc,
              DWORD, dwYieldData)
{
  dprintf(("WINMM:mciSetYieldProc - stub\n"));
  return FALSE;
}


