/* $Id: metafile.cpp,v 1.4 2001-02-01 18:01:52 sandervl Exp $ */

/*
 * GDI32 metafile code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "misc.h"

#define DBG_LOCALLOG	DBG_metafile
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CloseEnhMetaFile( HDC arg1)
{
    dprintf(("GDI32: CloseEnhMetaFile NOT IMPLEMENTED"));
////    return O32_CloseEnhMetaFile(arg1);
    return 0;
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CloseMetaFile( HDC arg1)
{
    dprintf(("GDI32: CloseMetaFile"));
    return O32_CloseMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CopyEnhMetaFileA( HENHMETAFILE arg1, LPCSTR  arg2)
{
    dprintf(("GDI32: CopyEnhMetaFileA NOT IMPLEMENTED"));
////    return O32_CopyEnhMetaFile(arg1, arg2);
    return 0;
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CopyEnhMetaFileW( HENHMETAFILE arg1, LPCWSTR  arg2)
{
 char        *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HENHMETAFILE rc;

    dprintf(("GDI32: CopyEnhMetaFileW"));
    rc = CopyEnhMetaFileA(arg1, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CreateEnhMetaFileA( HDC arg1, LPCSTR arg2, const RECT * arg3, LPCSTR  arg4)
{
    dprintf(("GDI32: CreateEnhMetaFileA NOT IMPLEMENTED"));
////    return O32_CreateEnhMetaFile(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API CreateEnhMetaFileW( HDC arg1, LPCWSTR arg2, const RECT * arg3, LPCWSTR  arg4)
{
    char *astring1 = UnicodeToAsciiString((LPWSTR)arg2);
    char *astring2 = UnicodeToAsciiString((LPWSTR)arg4);
    HENHMETAFILE   rc;

    dprintf(("GDI32: CreateMetaFileW"));
    rc = CreateEnhMetaFileA(arg1,astring1,arg3,astring2);
    FreeAsciiString(astring1);
    FreeAsciiString(astring2);
    return rc;
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: CreateMetaFileA"));
    return O32_CreateMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
HDC WIN32API CreateMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HDC   rc;

    dprintf(("GDI32: CreateMetaFileW"));
    rc = O32_CreateMetaFile(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteEnhMetaFile( HENHMETAFILE arg1)
{
    dprintf(("GDI32: DeleteEnhMetaFile NOT IMPLEMENTED"));
////    return O32_DeleteEnhMetaFile(arg1);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DeleteMetaFile( HMETAFILE arg1)
{
    dprintf(("GDI32: DeleteMetaFile"));
    return O32_DeleteMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CopyMetaFileA( HMETAFILE arg1, LPCSTR  arg2)
{
    dprintf(("GDI32: CopyMetaFileA"));
    return O32_CopyMetaFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API CopyMetaFileW( HMETAFILE arg1, LPCWSTR  arg2)
{
 char        *astring = UnicodeToAsciiString((LPWSTR)arg2);
 HMETAFILE rc;

    dprintf(("GDI32: CopyMetaFileW"));
    rc = O32_CopyMetaFile(arg1, astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//TODO: Callback
//******************************************************************************
BOOL WIN32API EnumMetaFile( HDC hdc, HMETAFILE hMetafile, MFENUMPROC metaProc, LPARAM lParam)
{
    dprintf(("GDI32: EnumMetaFile NOT IMPLEMENTED"));
    //calling convention differences
//    return O32_EnumMetaFile(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EnumEnhMetaFile( HDC arg1, HENHMETAFILE arg2, ENHMFENUMPROC arg3, PVOID arg4, const RECT *  arg5)
{
    dprintf(("GDI32: EnumEnhMetaFile NOT IMPLEMENTED!"));
//    return O32_EnumEnhMetaFile(arg1, arg2, arg3, arg4, arg5);
    return 0;
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API GetMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: GetMetaFileA"));
    return O32_GetMetaFile(arg1);
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API GetMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HENHMETAFILE rc;

    dprintf(("GDI32: GetMetaFileW"));
    rc = O32_GetMetaFile(astring);
    FreeAsciiString(astring);
    return rc;

}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API GetEnhMetaFileA( LPCSTR arg1)
{
    dprintf(("GDI32: GetEnhMetaFileA NOT IMPLEMENTED"));
////    return O32_GetEnhMetaFile(arg1);
    return 0;
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API GetEnhMetaFileW( LPCWSTR arg1)
{
 char *astring = UnicodeToAsciiString((LPWSTR)arg1);
 HENHMETAFILE rc;

    dprintf(("GDI32: GetEnhMetaFileW"));
    // NOTE: This will not work as is (needs UNICODE support)
    rc = GetEnhMetaFileA(astring);
    FreeAsciiString(astring);
    return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFileBits( HENHMETAFILE arg1, UINT arg2, PBYTE  arg3)
{
    dprintf(("GDI32: GetEnhMetaFileBits NOT IMPLEMENTED"));
////    return O32_GetEnhMetaFileBits(arg1, arg2, arg3);
    return 0;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetMetaFileBitsEx( HMETAFILE arg1, UINT arg2, LPVOID  arg3)
{
    dprintf(("GDI32: GetMetaFileBitsEx"));
    return O32_GetMetaFileBitsEx(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API SetEnhMetaFileBits( UINT arg1, const BYTE *  arg2)
{
    dprintf(("GDI32: SetEnhMetaFileBits NOT IMPLEMENTED"));
////    return O32_SetEnhMetaFileBits(arg1, arg2);
    return 0;
}
//******************************************************************************
//******************************************************************************
HMETAFILE WIN32API SetMetaFileBitsEx( UINT arg1, const BYTE * arg2)
{
    dprintf(("GDI32: SetMetaFileBitsEx"));
    return O32_SetMetaFileBitsEx(arg1, (PBYTE)arg2);
}
//******************************************************************************
//******************************************************************************
HENHMETAFILE WIN32API SetWinMetaFileBits( UINT arg1, const BYTE * arg2, HDC arg3, const METAFILEPICT *  arg4)
{
    dprintf(("GDI32: SetWinMetaFileBits NOT IMPLEMENTED"));
////    return O32_SetWinMetaFileBits(arg1, arg2, arg3, arg4);
    return 0;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetWinMetaFileBits( HENHMETAFILE arg1, UINT arg2, PBYTE arg3, int arg4, HDC arg5)
{
    dprintf(("GDI32: GetWinMetaFileBits NOT IMPLEMENTED"));
////    return O32_GetWinMetaFileBits(arg1, arg2, arg3, arg4, arg5);
    return 0;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFileHeader( HENHMETAFILE arg1, UINT arg2, LPENHMETAHEADER  arg3)
{
    dprintf(("GDI32: GetEnhMetaFileHeader NOT IMPLEMENTED"));
////    return O32_GetEnhMetaFileHeader(arg1, arg2, arg3);
    return 0;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFilePaletteEntries( HENHMETAFILE arg1, UINT arg2, PPALETTEENTRY  arg3)
{
    dprintf(("GDI32: GetEnhMetaFilePaletteEntries NOT IMPLEMENTED"));
////    return O32_GetEnhMetaFilePaletteEntries(arg1, arg2, arg3);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayEnhMetaFile( HDC arg1, HENHMETAFILE arg2, const RECT *  arg3)
{
    dprintf(("GDI32: PlayEnhMetaFile NOT IMPLEMENTED"));
////    return O32_PlayEnhMetaFile(arg1, arg2, arg3);
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayMetaFile( HDC arg1, HMETAFILE  arg2)
{
    dprintf(("GDI32: PlayMetaFile"));
    return O32_PlayMetaFile(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayMetaFileRecord( HDC arg1, LPHANDLETABLE arg2, LPMETARECORD arg3, UINT  arg4)
{
    dprintf(("GDI32: PlayMetaFileRecord"));
    return O32_PlayMetaFileRecord(arg1, arg2, arg3, (int)arg4);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PlayEnhMetaFileRecord(                                 /*KSO Thu 21.05.1998*/
        HDC                                     arg1,
        LPHANDLETABLE           arg2,
        CONST ENHMETARECORD *arg3,
        UINT                            arg4
        )
{
        dprintf(("GDI32: PlayEnhMetaFileRecord - NOT IMPLEMENTED"));
        return FALSE;
}
//******************************************************************************
UINT WIN32API GetEnhMetaFileDescriptionA(HENHMETAFILE    arg1,
                                         UINT            arg2,
                                         LPSTR           arg3)
{
        dprintf(("GDI32: GetEnhMetaFileDescriptionA - NOT IMPLEMENTED"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetEnhMetaFileDescriptionW(HENHMETAFILE    arg1,
                                         UINT            arg2,
                                         LPWSTR          arg3)
{
        dprintf(("GDI32: GetEnhMetaFileDescriptionW - stub\n"));
        return FALSE;
}
//******************************************************************************
//******************************************************************************
