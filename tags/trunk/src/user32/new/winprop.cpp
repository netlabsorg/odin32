/* $Id: winprop.cpp,v 1.1 1999-07-18 10:39:52 sandervl Exp $ */
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

//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsA(HWND arg1, PROPENUMPROCA arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsA DOES NOT WORK\n");
#endif
    //calling convention problems
    return 0;
//    return O32_EnumProps(arg1, (PROPENUMPROC_O32)arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsExA( HWND arg1, PROPENUMPROCEXA arg2, LPARAM  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsExA DOES NOT WORK\n");
#endif
    //calling convention problems
    return 0;
//    return O32_EnumPropsEx(arg1, arg2, (PROPENUMPROCEX_O32)arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsExW( HWND arg1, PROPENUMPROCEXW arg2, LPARAM  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsExW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    //calling convention problems
    return 0;
//    return O32_EnumPropsEx(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API EnumPropsW( HWND arg1, PROPENUMPROCW  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumPropsW\n");
#endif
    // NOTE: This will not work as is (needs UNICODE support)
    //calling convention problems
    return 0;
//    return O32_EnumProps(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetPropA( HWND arg1, LPCSTR  arg2)
{
#ifdef DEBUG
    if((int)arg2 >> 16 != 0)
     WriteLog("USER32:  GetPropA %s\n", arg2);
    else WriteLog("USER32:  GetPropA %X\n", arg2);
#endif
    return O32_GetProp(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetPropW(HWND arg1, LPCWSTR arg2)
{
 BOOL  handle;
 char *astring;

    if((int)arg2 >> 16 != 0)
         astring = UnicodeToAsciiString((LPWSTR)arg2);
    else astring = (char *)arg2;
#ifdef DEBUG
    if((int)arg2 >> 16 != 0)
         WriteLog("USER32:  GetPropW %s\n", astring);
    else WriteLog("USER32:  GetPropW %X\n", astring);
#endif
    handle = GetPropA(arg1, (LPCSTR)astring);
    if((int)arg2 >> 16 != 0)
        FreeAsciiString(astring);

    return(handle);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API RemovePropA( HWND arg1, LPCSTR  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  RemovePropA\n");
#endif
    return O32_RemoveProp(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API RemovePropW( HWND arg1, LPCWSTR  arg2)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HANDLE rc;

#ifdef DEBUG
    WriteLog("USER32:  RemovePropW\n");
#endif
    rc = O32_RemoveProp(arg1, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPropA( HWND arg1, LPCSTR arg2, HANDLE  arg3)
{
#ifdef DEBUG
    if((int)arg2 >> 16 != 0)
     WriteLog("USER32:  SetPropA %S\n", arg2);
    else WriteLog("USER32:  SetPropA %X\n", arg2);
#endif
    return O32_SetProp(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPropW(HWND arg1, LPCWSTR arg2, HANDLE arg3)
{
 BOOL  rc;
 char *astring;

    if((int)arg2 >> 16 != 0)
         astring = UnicodeToAsciiString((LPWSTR)arg2);
    else astring = (char *)arg2;

#ifdef DEBUG
    if((int)arg2 >> 16 != 0)
         WriteLog("USER32:  SetPropW %S\n", astring);
    else WriteLog("USER32:  SetPropW %X\n", astring);
#endif
    rc = O32_SetProp(arg1, astring, arg3);
    if((int)astring >> 16 != 0)
        FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
