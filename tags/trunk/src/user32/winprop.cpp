/* $Id: winprop.cpp,v 1.6 2000-01-18 20:11:22 sandervl Exp $ */
/*
 * Win32 Property apis for OS/2
 *
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <user32.h>
#include <misc.h>
#include "win32wbase.h"

#undef DEBUG

//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsA(HWND hwnd, PROPENUMPROCA arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsA DOES NOT WORK\n");
#endif
    //calling convention problems
    return 0;
//    return O32_EnumProps(hwnd, (PROPENUMPROC_O32)arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsExA( HWND hwnd, PROPENUMPROCEXA arg2, LPARAM  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsExA DOES NOT WORK\n");
#endif
    //calling convention problems
    return 0;
//    return O32_EnumPropsEx(hwnd, arg2, (PROPENUMPROCEX_O32)arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsExW( HWND hwnd, PROPENUMPROCEXW arg2, LPARAM  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsExW DOES NOT WORK\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    //calling convention problems
    return 0;
//    return O32_EnumPropsEx(hwnd, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsW( HWND hwnd, PROPENUMPROCW  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsW DOES NOT WORK\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    //calling convention problems
    return 0;
//    return O32_EnumProps(hwnd, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetPropA(HWND hwnd, LPCSTR arg2)
{
  Win32BaseWindow *window;
  HANDLE rc;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("GetPropA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    rc = O32_GetProp(window->getOS2WindowHandle(), arg2);
#ifdef DEBUG
    if(HIWORD(arg2) != 0)
         dprintf(("USER32: GetPropA %s returned %x\n", arg2, rc));
    else dprintf(("USER32: GetPropA %X returned %x\n", arg2, rc));
#endif
    return rc;
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetPropW(HWND hwnd, LPCWSTR arg2)
{
 BOOL  handle;
 char *astring;

    if(HIWORD(arg2) != 0)
         astring = UnicodeToAsciiString((LPWSTR)arg2);
    else astring = (char *)arg2;

    handle = GetPropA(hwnd, (LPCSTR)astring);
    if(HIWORD(astring) != 0)
        FreeAsciiString(astring);

    return(handle);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API RemovePropA( HWND hwnd, LPCSTR  arg2)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("RemovePropA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
#ifdef DEBUG
    if(HIWORD(arg2) != 0)
         dprintf(("USER32: RemovePropA %s\n", arg2));
    else dprintf(("USER32: RemovePropA %X\n", arg2));
#endif
    return O32_RemoveProp(window->getOS2WindowHandle(), arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API RemovePropW( HWND hwnd, LPCWSTR arg2)
{
 char  *astring;
 HANDLE rc;

    if(HIWORD(arg2) != 0)
         astring = UnicodeToAsciiString((LPWSTR)arg2);
    else astring = (char *)arg2;

    rc = RemovePropA(hwnd, astring);

    if(HIWORD(astring) != 0)
        FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPropA( HWND hwnd, LPCSTR arg2, HANDLE  arg3)
{
 Win32BaseWindow *window;

    window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("SetPropA, window %x not found", hwnd));
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
#ifdef DEBUG
    if(HIWORD(arg2) != 0)
         dprintf(("USER32: SetPropA %s to %x\n", arg2, arg3));
    else dprintf(("USER32: SetPropA %X to %x\n", arg2, arg3));
#endif
    return O32_SetProp(window->getOS2WindowHandle(), arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPropW(HWND hwnd, LPCWSTR arg2, HANDLE arg3)
{
 BOOL  rc;
 char *astring;

    if(HIWORD(arg2) != 0)
         astring = UnicodeToAsciiString((LPWSTR)arg2);
    else astring = (char *)arg2;

    rc = SetPropA(hwnd, astring, arg3);
    if(HIWORD(astring) != 0)
        FreeAsciiString(astring);

    return(rc);
}
//******************************************************************************
//******************************************************************************
