/* $Id: oslibkbd.cpp,v 1.1 2003-02-16 15:31:10 sandervl Exp $ */
/*
 * Keyboard helpers for OS/2
 *
 *
 * Copyright 2003 Sander van Leeuwen (sandervl@innotek.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define  INCL_WIN
#define  INCL_PM
#define  INCL_WINSWITCHLIST
#define  INCL_WININPUT
#include <os2wrap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dbglog.h>
#include <win32type.h>
#include <winconst.h>

#define DBG_LOCALLOG	DBG_oslibkbd
#include "dbglocal.h"

SHORT APIENTRY16 WinGetScanState( HWND, USHORT );
BOOL  APIENTRY16 WinSetScanStateTable(HWND,unsigned char *,BOOL);

//******************************************************************************
//******************************************************************************
SHORT OSLibWinGetScanState(USHORT scan)
{
    SHORT state;
    SHORT sel;
    
    sel = RestoreOS2FS();
    state = WinGetScanState(HWND_DESKTOP, scan);
    SetFS(sel);

    return state;    
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetScanStateTable(unsigned char *PMScanState)
{
    SHORT ret;
    SHORT sel;
    
    sel = RestoreOS2FS();
    ret = WinSetScanStateTable(HWND_DESKTOP, PMScanState, FALSE);
    SetFS(sel);

    if(ret == FALSE) {
        dprintf(("WinSetScanStateTable failed with %x", WinGetLastError(0)));
    }
    return ret;    
}
//******************************************************************************
//******************************************************************************
LONG OSLibWinGetPhysKeyState(LONG scan)
{
    return WinGetPhysKeyState(HWND_DESKTOP,scan); 	
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinGetKeyboardStateTable(unsigned char *PMKeyState)
{
  return WinSetKeyboardStateTable(HWND_DESKTOP, (PBYTE)PMKeyState, FALSE );
}
//******************************************************************************
//******************************************************************************
BOOL OSLibWinSetKeyboardStateTable(unsigned char *PMKeyState)
{
  return WinSetKeyboardStateTable(HWND_DESKTOP, (PBYTE)PMKeyState, TRUE );
}
//******************************************************************************
//******************************************************************************
USHORT APIENTRY WinTranslateChar2( USHORT  /* Codepage (currently ignored) */
                                 , PUSHORT /* Ptr to char to translate     */
                                 , PULONG  /* Ptr to deadkey save info     */
                                 , USHORT  /* Translation option (TC_xxx)  */
                                 , PUSHORT /* Ptr to shift state (TCF_xxx) */
                                 );
//******************************************************************************
//******************************************************************************
USHORT OSLibWinTranslateChar(USHORT usScanCode, ULONG type, USHORT shiftstate)
{
  USHORT usResult;
  USHORT sel = GetFS();

  usResult = WinTranslateChar2(0, &usScanCode, NULL, type, &shiftstate);
  SetFS(sel);
  return usScanCode;
}
//******************************************************************************
//******************************************************************************
