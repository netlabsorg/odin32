/* $Id: winver.h,v 1.3 1999-08-22 22:54:26 sandervl Exp $ */

#ifndef __WINE_WINVER_H
#define __WINE_WINVER_H

#include "windef.h"

// enable C linkage to avoid parameter mangling
#ifdef __cplusplus__
extern "C" {
#endif


DWORD       WINAPI VerFindFileA(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,PUINT,LPSTR,PUINT);
DWORD       WINAPI VerFindFileW(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT,LPWSTR,PUINT);
DWORD       WINAPI VerInstallFileA(DWORD,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,LPSTR,PUINT);
DWORD       WINAPI VerInstallFileW(DWORD,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,LPWSTR,PUINT);
DWORD       WINAPI VerQueryValueA(LPVOID,LPSTR,LPVOID*,PUINT);
DWORD       WINAPI VerQueryValueW(LPVOID,LPWSTR,LPVOID*,PUINT);
DWORD       WINAPI GetFileVersionInfoSizeA(LPSTR,LPDWORD);
DWORD       WINAPI GetFileVersionInfoSizeW(LPWSTR,LPDWORD);
BOOL        WINAPI GetFileVersionInfoA(LPSTR,DWORD,DWORD,LPVOID);
BOOL        WINAPI GetFileVersionInfoW(LPWSTR,DWORD,DWORD,LPVOID);

// enable C linkage to avoid parameter mangling
#ifdef __cplusplus__
  }
#endif


#endif /* __WINE_WINVER_H */
