/* $Id: winaccel.cpp,v 1.4 1999-08-30 11:59:54 sandervl Exp $ */
/*
 * Win32 accelartor key functions for OS/2
 *
 * Copyright 1999 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <win32wbase.h>

//******************************************************************************
//******************************************************************************
HACCEL WIN32API LoadAcceleratorsA(HINSTANCE hinst, LPCSTR lpszAcc)
{
 HACCEL rc;

    rc = (HACCEL)FindResourceA(hinst, lpszAcc, RT_ACCELERATORA);

    dprintf(("LoadAcceleratorsA returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
HACCEL WIN32API LoadAcceleratorsW(HINSTANCE hinst, LPCWSTR lpszAccel)
{
 HACCEL rc;

    rc = (HACCEL)FindResourceW(hinst, lpszAccel, RT_ACCELERATORW);

    dprintf(("LoadAcceleratorsW returned %d\n", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DestroyAcceleratorTable( HACCEL haccel)
{
 Win32Resource *winres;

    if(HIWORD(haccel) == 0) {
    dprintf(("DestroyAcceleratorTable: invalid haccel %x", haccel));
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
    }
    dprintf(("DestroyAcceleratorTable %x\n", haccel));
    winres = (Win32Resource *)haccel;
    delete winres;
    return TRUE;
}
//******************************************************************************
//TODO: Do more than just return?
//******************************************************************************
int WIN32API TranslateAcceleratorA(HWND hwnd, HACCEL haccel, LPMSG lpmsg)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
//Msg for (non-client) child of our frame window
//      dprintf(("TranslateAcceleratorA, window %x not found", hwnd));
        return FALSE;
    }
    if(window->GetAccelTable() != haccel) {
    dprintf(("TranslateAcceleratorA %X %X %X", hwnd, haccel, lpmsg->hwnd));
    window->SetAccelTable(haccel);
    }

    //SvL: OS/2 automatically translates accelerator keys
    return FALSE;
}
//******************************************************************************
//******************************************************************************
int WIN32API TranslateAcceleratorW( HWND hwnd, HACCEL hAccel, LPMSG lpMsg)
{
    return TranslateAcceleratorA(hwnd, hAccel, lpMsg);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TranslateMDISysAccel( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
////    WriteLog("USER32:  TranslateMDISysAccel\n");
#endif
    return O32_TranslateMDISysAccel(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HACCEL WIN32API CreateAcceleratorTableA( LPACCEL arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateAcceleratorTableA\n");
#endif
    return O32_CreateAcceleratorTable(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HACCEL WIN32API CreateAcceleratorTableW( LPACCEL arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  CreateAcceleratorTableW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_CreateAcceleratorTable(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API CopyAcceleratorTableA(HACCEL hAccelSrc, LPACCEL lpAccelDest,
                      int cAccelEntries)
{
#ifdef DEBUG
  WriteLog("USER32:  CopyAcceleratorTableA, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//TODO:
//******************************************************************************
int WIN32API CopyAcceleratorTableW(HACCEL hAccelSrc, LPACCEL lpAccelDest,
                      int cAccelEntries)
{
#ifdef DEBUG
  WriteLog("USER32:  CopyAcceleratorTableW, not implemented\n");
#endif
  return(0);
}
//******************************************************************************
//******************************************************************************
