/* $Id: directory.cpp,v 1.1 1999-09-15 23:32:54 sandervl Exp $ */

/*
 * Win32 Directory functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include "unicode.h"

//******************************************************************************
//******************************************************************************
UINT WIN32API GetCurrentDirectoryA(UINT arg1, LPSTR arg2)
{
    dprintf(("KERNEL32:  GetCurrentDirectory\n"));
    return O32_GetCurrentDirectory(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetCurrentDirectoryW(UINT nBufferLength, LPWSTR lpBuffer)
{
    char *asciidir = (char *)malloc(nBufferLength+1);
     int  rc;

    dprintf(("KERNEL32:  OS2GetCurrentDirectoryW\n"));

    rc = O32_GetCurrentDirectory(nBufferLength, asciidir);
    if(rc != 0) {
        AsciiToUnicode(asciidir, lpBuffer);
    }
    free(asciidir);
    return(rc);
}
//******************************************************************************

//******************************************************************************
BOOL WIN32API SetCurrentDirectoryA(LPCSTR arg1)
{
    dprintf(("KERNEL32:  SetCurrentDirectory to %s\n", arg1));
    return O32_SetCurrentDirectory((LPSTR)arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetCurrentDirectoryW(LPCWSTR lpPathName)
{
 char *asciipath;
 BOOL  rc;

    dprintf(("KERNEL32:  OS2SetCurrentDirectoryW\n"));
    asciipath = UnicodeToAsciiString((LPWSTR)lpPathName);
    rc = O32_SetCurrentDirectory(asciipath);
    FreeAsciiString(asciipath);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CreateDirectoryA( LPCSTR arg1, PSECURITY_ATTRIBUTES  arg2)
{
    dprintf(("KERNEL32:  OS2CreateDirectoryA %s\n", arg1));
    return O32_CreateDirectory(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API CreateDirectoryW(LPCWSTR arg1, PSECURITY_ATTRIBUTES arg2)
{
 BOOL  rc;
 char *astring;

    dprintf(("KERNEL32:  OS2CreateDirectoryW"));
    astring = UnicodeToAsciiString((LPWSTR)arg1);
    rc = O32_CreateDirectory(astring, arg2);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetSystemDirectoryA(LPSTR arg1, UINT arg2)
{
    dprintf(("KERNEL32:  GetSystemDirectory\n"));
    return O32_GetSystemDirectory(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetSystemDirectoryW(LPWSTR lpBuffer, UINT uSize)
{
 char *asciibuffer = (char *)malloc(uSize+1);
 UINT  rc;

    dprintf(("KERNEL32:  OS2GetSystemDirectoryW\n"));
    rc = O32_GetSystemDirectory(asciibuffer, uSize);
    if(rc)      AsciiToUnicode(asciibuffer, lpBuffer);
    free(asciibuffer);
    return(rc);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetWindowsDirectoryA( LPSTR arg1, UINT  arg2)
{
    dprintf(("KERNEL32:  GetWindowsDirectory\n"));
    return O32_GetWindowsDirectory(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetWindowsDirectoryW(LPWSTR lpBuffer, UINT uSize)
{
 char *asciibuffer = (char *)malloc(uSize+1);
 UINT  rc;

    dprintf(("KERNEL32:  OS2GetWindowsDirectoryW"));
    rc = O32_GetWindowsDirectory(asciibuffer, uSize);
    AsciiToUnicode(asciibuffer, lpBuffer);
    free(asciibuffer);
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveDirectoryA( LPCSTR arg1)
{
    dprintf(("KERNEL32:  OS2RemoveDirectoryA\n"));
    return O32_RemoveDirectory(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RemoveDirectoryW(LPCWSTR lpPathName)
{
 char *asciipath;
 BOOL  rc;

    dprintf(("KERNEL32:  OS2RemoveDirectoryW"));
    asciipath = UnicodeToAsciiString((LPWSTR)lpPathName);
    rc = O32_RemoveDirectory(asciipath);
    FreeAsciiString(asciipath);
    return(rc);
}
