/* $Id: winver.h,v 1.4 1999-09-18 15:55:45 sandervl Exp $ */

#ifndef __WINE_WINVER_H
#define __WINE_WINVER_H

#include "windef.h"

// enable C linkage to avoid parameter mangling
#ifdef __cplusplus__
extern "C" {
#endif


DWORD       WINAPI VerFindFileA(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,PUINT,LPSTR,PUINT);
DWORD       WINAPI VerFindFileW(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT,LPWSTR,PUINT);
#define     VerFindFile WINELIB_NAME_AW(VerFindFile)

DWORD       WINAPI VerInstallFileA(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,PUINT);
DWORD       WINAPI VerInstallFileW(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT);
#define     VerInstallFile WINELIB_NAME_AW(VerInstallFile)

DWORD       WINAPI VerQueryValueA(LPVOID,LPSTR,LPVOID*,PUINT);
DWORD       WINAPI VerQueryValueW(LPVOID,LPWSTR,LPVOID*,PUINT);
#define     VerQueryValue WINELIB_NAME_AW(VerQueryValue)

DWORD       WINAPI GetFileVersionInfoSizeA(LPSTR,LPDWORD);
DWORD       WINAPI GetFileVersionInfoSizeW(LPWSTR,LPDWORD);
#define     GetFileVersionInfoSize WINELIB_NAME_AW(GetFileVersionInfoSize)

BOOL        WINAPI GetFileVersionInfoA(LPSTR,DWORD,DWORD,LPVOID);
BOOL        WINAPI GetFileVersionInfoW(LPWSTR,DWORD,DWORD,LPVOID);
#define     GetFileVersionInfo WINELIB_NAME_AW(GetFileVersionInfo)

// enable C linkage to avoid parameter mangling
#ifdef __cplusplus__
  }
#endif


#endif /* __WINE_WINVER_H */
