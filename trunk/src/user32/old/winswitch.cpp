/* $Id: winswitch.cpp,v 1.1 2001-05-11 08:36:32 sandervl Exp $ */
/*
 * WinSwitch control
 *
 * Copyright 1999 Christoph Bratschi
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Status:  stub
 * Version: stub
 */
#include <stdlib.h>
#include <os2win.h>
#include "controls.h"
#include "winswitch.h"

#define DBG_LOCALLOG	DBG_winswitch
#include "dbglocal.h"

LRESULT WINAPI WinSwitchWndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
  return DefWindowProcA(hwnd,message,wParam,lParam);
}
//******************************************************************************
//******************************************************************************
BOOL WINSWITCH_Register()
{
    WNDCLASSA wndClass;

//SvL: Don't check this now
//    if (GlobalFindAtomA(WINSWITCHCLASSNAME)) return FALSE;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS;
    wndClass.lpfnWndProc   = (WNDPROC)WinSwitchWndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = sizeof(WINSWITCH_INFO);
    wndClass.hCursor       = LoadCursorA(0,IDC_ARROWA);
    wndClass.hbrBackground = (HBRUSH)0;
    wndClass.lpszClassName = WINSWITCHCLASSNAME;

    return RegisterClassA(&wndClass);
}
//******************************************************************************
//******************************************************************************
BOOL WINSWITCH_Unregister()
{
    if (GlobalFindAtomA(WINSWITCHCLASSNAME))
        return UnregisterClassA(WINSWITCHCLASSNAME,(HINSTANCE)NULL);
    else return FALSE;
}
//******************************************************************************
//******************************************************************************

