/* $Id: win32dlg.cpp,v 1.2 1999-08-30 11:59:53 sandervl Exp $ */
/*
 * Win32 Dialog Code for OS/2
 *
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <win32dlg.h>

//******************************************************************************
//******************************************************************************
Win32Dialog::Win32Dialog() : Win32BaseWindow(OBJTYPE_DIALOG)
{
}
//******************************************************************************
//******************************************************************************
Win32Dialog::~Win32Dialog()
{

}
//******************************************************************************
//******************************************************************************
ULONG Win32Dialog::MsgOS2Create(HWND hwndOS2, ULONG initParam)
{
  OS2Hwnd = hwndOS2;
  return win32wndproc(Win32Hwnd, WM_CREATE, 0, initParam);
}
//******************************************************************************
//******************************************************************************
LONG Win32Dialog::SetWindowLongA(int index, ULONG value)
{
 LONG oldval;

  switch(index)
  {
    case DWL_DLGPROC:
        oldval = (LONG)Win32DlgProc;
        Win32DlgProc = (DLGPROC)index;
        return oldval;
    case DWL_MSGRESULT:
        oldval = msgResult;
        msgResult = value;
        return oldval;
    case DWL_USER:
        oldval = userDlgData;
        userDlgData = value;
        return oldval;
    default:
        return Win32BaseWindow::SetWindowLongA(index, value);
  }
}
//******************************************************************************
//******************************************************************************
ULONG Win32Dialog::GetWindowLongA(int index)
{
  switch(index)
  {
    case DWL_DLGPROC:
        return (ULONG)Win32DlgProc;
    case DWL_MSGRESULT:
        return msgResult;
    case DWL_USER:
        return userDlgData;
    default:
        return Win32BaseWindow::GetWindowLongA(index);
  }
}
//******************************************************************************
//******************************************************************************
