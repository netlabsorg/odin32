/* $Id: comm.cpp,v 1.4 2000-02-16 14:25:30 sandervl Exp $ */

/*
 * PE2LX dialog conversion code
 *
 * Copyright 1998 Patrick Haller (?)
 * Copyright 1998 Felix Maschek	 (?)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * *Comm* stubs
 */
#include <os2win.h>
#include "unicode.h"
#include "handlemanager.h"

#define DBG_LOCALLOG	DBG_comm
#include "dbglocal.h"

//------------------------------------------------------------------------------

BOOL WIN32API BuildCommDCBA( LPCSTR lpDef, LPDCB lpDCB )
{
  dprintf(("BuildCommDCBA Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API BuildCommDCBW( LPCWSTR lpDef, LPDCB lpDCB )
{
  dprintf(("BuildCommDCBW Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API BuildCommDCBAndTimeoutsA( LPCSTR lpDef, LPDCB lpDCB, LPCOMMTIMEOUTS lpCommTimeouts )
{
  dprintf(("BuildCommDCBAndTimeoutsA Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API BuildCommDCBAndTimeoutsW( LPCWSTR lpDef, LPDCB lpDCB, LPCOMMTIMEOUTS lpCommTimeouts )
{
  dprintf(("BuildCommDCBAndTimeoutsW Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API CommConfigDialogA( LPCSTR lpszName, HWND hWnd, LPCOMMCONFIG lpCC )
{
  dprintf(("CommConfigDialogA Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API CommConfigDialogW( LPCWSTR lpszName, HWND hWnd, LPCOMMCONFIG lpCC )
{
  dprintf(("CommConfigDialogW Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetDefaultCommConfigA( LPCSTR lpszName, LPCOMMCONFIG lpCC, LPDWORD lpdwSize )
{
  dprintf(("GetDefaultCommConfigA Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetDefaultCommConfigW( LPCWSTR lpszName, LPCOMMCONFIG lpCC, LPDWORD lpdwSize )
{
  dprintf(("GetDefaultCommConfigW Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetDefaultCommConfigA( LPCSTR lpszName, LPCOMMCONFIG lpCC, DWORD dwSize )
{
  dprintf(("SetDefaultCommConfigA Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetDefaultCommConfigW( LPCWSTR lpszName, LPCOMMCONFIG lpCC, DWORD dwSize )
{
  dprintf(("SetDefaultCommConfigW Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API ClearCommBreak( HANDLE hFile )
{
  dprintf(("ClearCommBreak Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetupComm( HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue )
{
  return HMSetupComm(hFile, dwInQueue, dwOutQueue);
}

//------------------------------------------------------------------------------

BOOL WIN32API EscapeCommFunction( HANDLE hFile, UINT dwFunc )
{
  dprintf(("EscapeCommFunction Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetCommConfig( HANDLE hCommDev, LPCOMMCONFIG lpCC, LPDWORD lpdwSize )
{
  dprintf(("GetCommConfig Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetCommModemStatus( HANDLE hFile, LPDWORD lpModemStat )
{
  dprintf(("GetCommModemStatus Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetCommBreak( HANDLE hFile )
{
  dprintf(("SetCommBreak Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetCommConfig( HANDLE hCommDev, LPCOMMCONFIG lpCC, DWORD dwSize )
{
  dprintf(("SetCommConfig Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API TransmitCommChar( INT hFile, CHAR cChar )
{
  dprintf(("TransmitCommChar Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetCommTimeouts(INT hCommDev, LPCOMMTIMEOUTS lpctmo)
{
  dprintf(("OS2SetCommTimeouts Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetCommTimeouts(INT hCommDev, LPCOMMTIMEOUTS lpctmo)
{
  dprintf(("OS2GetCommTimeouts Not implemented\n"));
  return(FALSE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetCommState(INT hCommDev, LPDCB lpdcb)
{
  return HMGetCommState(hCommDev, lpdcb);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetCommState(INT hCommDev, LPDCB lpdcb)
{
  dprintf(("OS2SetCommState Not implemented\n"));
  return(FALSE);
}

//------------------------------------------------------------------------------

BOOL WIN32API ClearCommError(INT hCommDev, LPDWORD lpdwErrors, LPCOMSTAT lpcst)
{
  dprintf(("OS2ClearCommError Not implemented\n"));
  return(FALSE);
}

//------------------------------------------------------------------------------

BOOL WIN32API PurgeComm(HANDLE hCommDev, DWORD fdwAction)
{
  dprintf(("OS2PurgeComm Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API SetCommMask(INT hCommDev, DWORD fdwEvtMask)
{
  dprintf(("SetCommMask Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetCommMask(HANDLE hCommDev, LPDWORD fdwEvtMask)
{
  dprintf(("GetCommMask Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API GetCommProperties(HANDLE hCommDev, LPCOMMPROP lpcmmp)
{
  dprintf(("GetCommProperties Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

BOOL WIN32API WaitCommEvent(HANDLE hCommDev, LPDWORD lpfdwEvtMask,
                   LPOVERLAPPED lpo)
{
  dprintf(("WaitCommEvent Not implemented (TRUE)\n"));
  return(TRUE);
}

//------------------------------------------------------------------------------

