/* $Id: cursoricon.h,v 1.3 2000-11-09 18:23:22 sandervl Exp $ */

/*
 * Cursor and icon definitions
 *
 * Copyright 1995 Alexandre Julliard
 */

#ifndef __WINE_CURSORICON_H
#define __WINE_CURSORICON_H

#include "windef.h"

#include "pshpack1.h"

typedef struct
{
    BYTE   bWidth;
    BYTE   bHeight;
    BYTE   bColorCount;
    BYTE   bReserved;
} ICONRESDIR;

typedef struct
{
    WORD   wWidth;
    WORD   wHeight;
} CURSORDIR;

typedef struct
{   union
    { ICONRESDIR icon;
      CURSORDIR  cursor;
    } ResInfo;
    WORD   wPlanes;
    WORD   wBitCount;
    DWORD  dwBytesInRes;
    WORD   wResId;
} CURSORICONDIRENTRY;

typedef struct
{
    WORD                idReserved;
    WORD                idType;
    WORD                idCount;
    CURSORICONDIRENTRY  idEntries[1] WINE_PACKED;
} CURSORICONDIR;

typedef struct {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD xHotspot;
    WORD yHotspot;
    DWORD dwDIBSize;
    DWORD dwDIBOffset;
} CURSORICONFILEDIRENTRY;

typedef struct
{
    WORD                idReserved;
    WORD                idType;
    WORD                idCount;
    CURSORICONFILEDIRENTRY  idEntries[1];
} CURSORICONFILEDIR;

typedef struct tagCURSORICONINFO
{
    POINT16 ptHotSpot;
    WORD    nWidth;
    WORD    nHeight;
    WORD    nWidthBytes;
    BYTE    bPlanes;
    BYTE    bBitsPerPixel;
    HBITMAP hColorBmp;
} CURSORICONINFO;


#include "poppack.h"

#define CID_RESOURCE  0x0001
#define CID_WIN32     0x0004
#define CID_NONSHARED 0x0008

extern void CURSORICON_Init( void );

extern HCURSOR16 CURSORICON_IconToCursor( HICON16 hIcon,
                                          BOOL bSemiTransparent );

extern HGLOBAL CURSORICON_Load( HINSTANCE hInstance, LPCWSTR name,
                                int width, int height, int colors,
                                BOOL fCursor, UINT loadflags);

extern WORD WIN32API CURSORICON_Destroy( HGLOBAL handle, UINT flags );

extern void CURSORICON_FreeModuleIcons( HMODULE hModule );

HGLOBAL CopyCursorIcon(HGLOBAL Handle, UINT nType, 
  	 	       INT iDesiredCX, INT iDesiredCY, 
		       UINT nFlags);

HGLOBAL WIN32API CreateCursorIconIndirect( HINSTANCE hInstance,
                                           CURSORICONINFO *info,
                                           LPCVOID lpANDbits,
                                           LPCVOID lpXORbits );

HGLOBAL CURSORICON_ExtCopy(HGLOBAL Handle, UINT nType,
                           INT iDesiredCX, INT iDesiredCY,
                           UINT nFlags);
				    
#endif /* __WINE_CURSORICON_H */
