/* $Id: windlgmsg.cpp,v 1.1 1999-07-15 18:03:03 sandervl Exp $ */
/*
 * Win32 dialog message APIs for OS/2
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
//******************************************************************************
LONG WIN32API SendDlgItemMessageA( HWND arg1, int arg2, UINT arg3, WPARAM arg4, LPARAM  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SendDlgItemMessageA\n");
#endif
    return O32_SendDlgItemMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API SendDlgItemMessageW( HWND arg1, int arg2, UINT arg3, WPARAM arg4, LPARAM  arg5)
{
#ifdef DEBUG
    WriteLog("USER32:  SendDlgItemMessageW\n");
#endif
    return O32_SendDlgItemMessage(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDialogMessageA( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
////    WriteLog("USER32:  IsDialogMessage\n");
#endif
    return O32_IsDialogMessage(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDialogMessageW( HWND arg1, LPMSG  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  IsDialogMessageW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    return O32_IsDialogMessage(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
