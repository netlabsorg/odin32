/* $Id: clipboard.cpp,v 1.1 1999-09-15 23:18:48 sandervl Exp $ */

/*
 * Win32 Clipboard API functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>

BOOL WIN32API ChangeClipboardChain( HWND arg1, HWND  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  ChangeClipboardChain\n");
#endif
    return O32_ChangeClipboardChain(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CloseClipboard(void)
{
#ifdef DEBUG
    WriteLog("USER32:  CloseClipboard\n");
#endif
    return O32_CloseClipboard();
}
//******************************************************************************
//******************************************************************************
int WIN32API CountClipboardFormats(void)
{
#ifdef DEBUG
    WriteLog("USER32:  CountClipboardFormats\n");
#endif
    return O32_CountClipboardFormats();
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EmptyClipboard(void)
{
#ifdef DEBUG
    WriteLog("USER32:  EmptyClipboard\n");
#endif
    return O32_EmptyClipboard();
}
//******************************************************************************
//******************************************************************************
UINT WIN32API EnumClipboardFormats(UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  EnumClipboardFormats\n");
#endif
    return O32_EnumClipboardFormats(arg1);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API GetClipboardData( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipboardData\n");
#endif
    return O32_GetClipboardData(arg1);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipboardFormatNameA( UINT arg1, LPSTR arg2, int  arg3)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipboardFormatNameA %s\n", arg2);
#endif
    return O32_GetClipboardFormatName(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipboardFormatNameW(UINT arg1, LPWSTR arg2, int arg3)
{
 int   rc;
 char *astring = UnicodeToAsciiString(arg2);

#ifdef DEBUG
    WriteLog("USER32:  GetClipboardFormatNameW %s\n", astring);
#endif
    rc = O32_GetClipboardFormatName(arg1, astring, arg3);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetClipboardOwner(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipboardOwner\n");
#endif
    return O32_GetClipboardOwner();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetClipboardViewer(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetClipboardViewer\n");
#endif
    return O32_GetClipboardViewer();
}
//******************************************************************************
//******************************************************************************
HWND WIN32API GetOpenClipboardWindow(void)
{
#ifdef DEBUG
    WriteLog("USER32:  GetOpenClipboardWindow\n");
#endif
    return O32_GetOpenClipboardWindow();
}
//******************************************************************************
//******************************************************************************
int WIN32API GetPriorityClipboardFormat( PUINT arg1, int  arg2)
{
#ifdef DEBUG
    WriteLog("USER32:  GetPriorityClipboardFormat\n");
#endif
    return O32_GetPriorityClipboardFormat(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsClipboardFormatAvailable( UINT arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  IsClipboardFormatAvailable\n");
#endif
    return O32_IsClipboardFormatAvailable(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API OpenClipboard( HWND arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  OpenClipboard\n");
#endif
    return O32_OpenClipboard(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterClipboardFormatA( LPCSTR arg1)
{
#ifdef DEBUG
    WriteLog("USER32:  RegisterClipboardFormatA\n");
#endif
    return O32_RegisterClipboardFormat(arg1);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API RegisterClipboardFormatW(LPCWSTR arg1)
{
 UINT  rc;
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);

#ifdef DEBUG
    WriteLog("USER32:  RegisterClipboardFormatW %s\n", astring);
#endif
    rc = O32_RegisterClipboardFormat(astring);
    FreeAsciiString(astring);
#ifdef DEBUG
    WriteLog("USER32:  RegisterClipboardFormatW returned %d\n", rc);
#endif
    return(rc);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API SetClipboardData( UINT arg1, HANDLE  arg2)
{
    dprintf(("USER32:  SetClipboardData\n"));
    return O32_SetClipboardData(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HWND WIN32API SetClipboardViewer( HWND arg1)
{
    dprintf(("USER32:  SetClipboardViewer\n"));
    return O32_SetClipboardViewer(arg1);
}
//******************************************************************************
//******************************************************************************
