/* $Id: winaccel.cpp,v 1.1 1999-07-18 17:12:52 sandervl Exp $ */
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

//******************************************************************************
//TODO: Do more than just return?
//******************************************************************************
int WIN32API TranslateAcceleratorA(HWND hwnd, HACCEL haccel, LPMSG lpmsg)
{
    //CB: needs more work
    //WinTranslateAccel();
    //get hab, translate
    //SvL: OS/2 automatically translates accelerator keys
    return TRUE;
}
//******************************************************************************
//******************************************************************************
int WIN32API TranslateAccelerator( HWND arg1, HACCEL arg2, LPMSG  arg3)
{
    //SvL: OS/2 automatically translates accelerator keys
    return TRUE;
}
//******************************************************************************
//******************************************************************************
int WIN32API TranslateAcceleratorW( HWND arg1, HACCEL arg2, LPMSG  arg3)
{
    //SvL: OS/2 automatically translates accelerator keys
    return TRUE;
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
BOOL WIN32API DestroyAcceleratorTable( HACCEL arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  DestroyAcceleratorTable\n");
#endif
    return O32_DestroyAcceleratorTable(arg1);
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
