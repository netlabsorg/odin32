/* $Id: version.h,v 1.5 2001-05-19 13:26:11 sandervl Exp $ */

/* Definitions for the VERsion infolibrary (VER.DLL)
 *
 * Copyright 1996 Marcus Meissner
 */

#ifndef __ODIN_VERSION_H
#define __ODIN_VERSION_H

#include <windef.h>
#include <wctype.h>

/* resource ids for different version infos */
/* These are internal resources for win16, but simple numbers in win32 */
/* All win32 defines should be moved to winver.h */
#define  VS_FILE_INFO16                  MAKEINTRESOURCE16(16)
#define  VS_VERSION_INFO16               MAKEINTRESOURCE16(1)
#define  VS_USER_INFO16                  MAKEINTRESOURCE16(100)

#define  VS_FILE_INFO                    16
#define  VS_VERSION_INFO                 1
#define  VS_USER_INFO                    100

#define  VS_FFI_SIGNATURE                0xfeef04bdL /* FileInfo Magic */
#define  VS_FFI_STRUCVERSION             0x00010000L /* struc version 1.0 */
#define  VS_FFI_FILEFLAGSMASK            0x0000003fL /* valid flags */

/* VS_VERSION.dwFileFlags */
#define  VS_FF_DEBUG                     0x01L
#define  VS_FF_PRERELEASE                0x02L
#define  VS_FF_PATCHED                   0x04L
#define  VS_FF_PRIVATEBUILD              0x08L
#define  VS_FF_INFOINFERRED              0x10L
#define  VS_FF_SPECIALBUILD              0x20L

/* VS_VERSION.dwFileOS */

/* major os version */
#define  VOS_UNKNOWN                        0x00000000L
#define  VOS_DOS                            0x00010000L
#define  VOS_OS216                          0x00020000L
#define  VOS_OS232                          0x00030000L
#define  VOS_NT                             0x00040000L

/* minor os version */
#define  VOS__BASE                          0x00000000L
#define  VOS__WINDOWS16                     0x00000001L
#define  VOS__PM16                          0x00000002L
#define  VOS__PM32                          0x00000003L
#define  VOS__WINDOWS32                     0x00000004L

/* possible versions */
#define  VOS_DOS_WINDOWS16               (VOS_DOS|VOS__WINDOWS16)
#define  VOS_DOS_WINDOWS32               (VOS_DOS|VOS__WINDOWS32)
#define  VOS_OS216_PM16                     (VOS_OS216|VOS__PM16)
#define  VOS_OS232_PM32                     (VOS_OS232|VOS__PM32)
#define  VOS_NT_WINDOWS32                (VOS_NT|VOS__WINDOWS32)

/* VS_VERSION.dwFileType */
#define  VFT_UNKNOWN                        0x00000000L
#define  VFT_APP                            0x00000001L
#define  VFT_DLL                            0x00000002L
#define  VFT_DRV                            0x00000003L
#define  VFT_FONT                           0x00000004L
#define  VFT_VXD                            0x00000005L
/* ??one type missing??                     0x00000006L -Marcus */
#define  VFT_STATIC_LIB                     0x00000007L

/* VS_VERSION.dwFileSubtype for VFT_DRV */
#define  VFT2_UNKNOWN                    0x00000000L
#define  VFT2_DRV_PRINTER                0x00000001L
#define  VFT2_DRV_KEYBOARD               0x00000002L
#define  VFT2_DRV_LANGUAGE               0x00000003L
#define  VFT2_DRV_DISPLAY                0x00000004L
#define  VFT2_DRV_MOUSE                  0x00000005L
#define  VFT2_DRV_NETWORK                0x00000006L
#define  VFT2_DRV_SYSTEM                 0x00000007L
#define  VFT2_DRV_INSTALLABLE            0x00000008L
#define  VFT2_DRV_SOUND                  0x00000009L
#define  VFT2_DRV_COMM                   0x0000000aL
#define  VFT2_DRV_INPUTMETHOD            0x0000000bL

/* VS_VERSION.dwFileSubtype for VFT_FONT */
#define  VFT2_FONT_RASTER                0x00000001L
#define  VFT2_FONT_VECTOR                0x00000002L
#define  VFT2_FONT_TRUETYPE              0x00000003L

/* VerFindFile Flags */
   /* input */
#define  VFFF_ISSHAREDFILE               0x0001

   /* output (returned) */
#define  VFF_CURNEDEST                   0x0001
#define  VFF_FILEINUSE                   0x0002
#define  VFF_BUFFTOOSMALL                0x0004

/* VerInstallFile Flags */
   /* input */
#define  VIFF_FORCEINSTALL               0x0001
#define  VIFF_DONTDELETEOLD              0x0002

   /* output (return) */
#define  VIF_TEMPFILE                    0x00000001L
#define  VIF_MISMATCH                    0x00000002L
#define  VIF_SRCOLD                      0x00000004L
#define  VIF_DIFFLANG                    0x00000008L
#define  VIF_DIFFCODEPG                  0x00000010L
#define  VIF_DIFFTYPE                    0x00000020L
#define  VIF_WRITEPROT                   0x00000040L
#define  VIF_FILEINUSE                   0x00000080L
#define  VIF_OUTOFSPACE                  0x00000100L
#define  VIF_ACCESSVIOLATION             0x00000200L
#define  VIF_SHARINGVIOLATION            0x00000400L
#define  VIF_CANNOTCREATE                0x00000800L
#define  VIF_CANNOTDELETE                0x00001000L
#define  VIF_CANNOTRENAME                0x00002000L
#define  VIF_CANNOTDELETECUR             0x00004000L
#define  VIF_OUTOFMEMORY                 0x00008000L
#define  VIF_CANNOTREADSRC               0x00010000L
#define  VIF_CANNOTREADDST               0x00020000L
#define  VIF_BUFFTOOSMALL                0x00040000L

typedef struct tagVS_FIXEDFILEINFO {
   DWORD   dwSignature;
   DWORD   dwStrucVersion;
   DWORD   dwFileVersionMS;
   DWORD   dwFileVersionLS;
   DWORD   dwProductVersionMS;
   DWORD   dwProductVersionLS;
   DWORD   dwFileFlagsMask;
   DWORD   dwFileFlags;
   DWORD   dwFileOS;
   DWORD   dwFileType;
   DWORD   dwFileSubtype;
   DWORD   dwFileDateMS;
   DWORD   dwFileDateLS;
} VS_FIXEDFILEINFO;

/***********************************************************************
 * Version Info Structure
 */

typedef struct
{
    WORD  wLength;
    WORD  wValueLength;
    CHAR  szKey[1];
#if 0   /* variable length structure */
    /* DWORD aligned */
    BYTE  Value[];
    /* DWORD aligned */
    VS_VERSION_INFO_STRUCT16 Children[];
#endif
} VS_VERSION_INFO_STRUCT16;

typedef struct
{
    WORD  wLength;
    WORD  wValueLength;
    WORD  bText;
    WCHAR szKey[1];
#if 0   /* variable length structure */
    /* DWORD aligned */
    BYTE  Value[];
    /* DWORD aligned */
    VS_VERSION_INFO_STRUCT32 Children[];
#endif
} VS_VERSION_INFO_STRUCT32;

#define VersionInfoIs16( ver ) \
    ( ((VS_VERSION_INFO_STRUCT16 *)ver)->szKey[0] >= ' ' )

#define VersionInfo16_Value( ver )  \
    (LPBYTE)( ((DWORD)((ver)->szKey) + (lstrlenA((ver)->szKey)+1) + 3) & ~3 )
#define VersionInfo32_Value( ver )  \
    (LPBYTE)( ((DWORD)((ver)->szKey) + 2*(lstrlenW((ver)->szKey)+1) + 3) & ~3 )

#define VersionInfo16_Children( ver )  \
    (VS_VERSION_INFO_STRUCT16 *)( VersionInfo16_Value( ver ) + \
                           ( ( (ver)->wValueLength + 3 ) & ~3 ) )
#define VersionInfo32_Children( ver )  \
    (VS_VERSION_INFO_STRUCT32 *)( VersionInfo32_Value( ver ) + \
                           ( ( (ver)->wValueLength * \
                               ((ver)->bText? 2 : 1) + 3 ) & ~3 ) )
#define VersionInfo16_Next( ver ) \
    (VS_VERSION_INFO_STRUCT16 *)( (LPBYTE)ver + (((ver)->wLength + 3) & ~3) )
#define VersionInfo32_Next( ver ) \
    (VS_VERSION_INFO_STRUCT32 *)( (LPBYTE)ver + (((ver)->wLength + 3) & ~3) )

#ifndef RC_INVOKED

/* function prototypes */

DWORD       WINAPI VerFindFileA(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,PUINT,LPSTR,PUINT);
DWORD       WINAPI VerFindFileW(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT,LPWSTR,PUINT);
DWORD       WINAPI VerInstallFileA(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,PUINT);
DWORD       WINAPI VerInstallFileW(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT);
BOOL        WINAPI VerQueryValueA(LPVOID,LPSTR,LPVOID*,PUINT);
BOOL        WINAPI VerQueryValueW(LPVOID,LPWSTR,LPVOID*,PUINT);
DWORD       WINAPI GetFileVersionInfoSizeA(LPSTR,LPDWORD);
DWORD       WINAPI GetFileVersionInfoSizeW(LPWSTR,LPDWORD);
BOOL        WINAPI GetFileVersionInfoA(LPSTR,DWORD,DWORD,LPVOID);
BOOL        WINAPI GetFileVersionInfoW(LPWSTR,DWORD,DWORD,LPVOID);
DWORD       WINAPI VerLanguageNameA( UINT wLang, LPSTR szLang, UINT nSize );
DWORD       WINAPI VerLanguageNameW( UINT wLang, LPWSTR szLang, UINT nSize );

/* 20 GETFILEVERSIONINFORAW */

#ifdef __WINE__

DWORD WINAPI GetFileResourceSize( LPCSTR lpszFileName,
                                    LPCSTR lpszResType, LPCSTR lpszResId,
                                    LPDWORD lpdwFileOffset );
DWORD WINAPI GetFileResource( LPCSTR lpszFileName,
                                LPCSTR lpszResType, LPCSTR lpszResId,
                                DWORD dwFileOffset,
                                DWORD dwResLen, LPVOID lpvData );

#endif /* __WINE__ */

#endif /* RC_INVOKED */

#endif /* __ODIN_VERSION_H */
