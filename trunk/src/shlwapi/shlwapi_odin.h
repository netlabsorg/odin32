/* $Id: shlwapi_odin.h,v 1.2 2001-08-30 19:19:58 phaller Exp $ */

/*
 * Win32 Lightweight SHELL32 for OS/2
 *
 * Copyright 2000 Patrick Haller
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*
 *      internal Shell32 Library definitions
 */

#ifndef _SHLWAPI_H_
#define _SHLWAPI_H_


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/


#ifdef __cplusplus
  extern "C" {
#endif /* defined(__cplusplus) */
    
    

typedef HKEY HUSKEY;
typedef HUSKEY *PHUSKEY;

typedef DWORD SHREGDEL_FLAGS;
#define SHREGDEL_BOTH

typedef DWORD SHREGENUM_FLAGS;
#define SHREGENUM_HKCU    
#define SHREGENUM_DEFAULT 
#define SHREGENUM_HKLM

typedef DWORD SHREGSET_FLAGS;
#define SHREGSET_HKLM
#define SHREGSET_DEFAULT
    
    
    
/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/
    
// REG.CPP
BOOL WIN32API SHRegGetBoolUSValueA(LPCSTR pszSubKey,
                                   LPCSTR pszValue,
                                   BOOL   fIgnoreHKCU,
                                   BOOL   fDefault);
    
BOOL WIN32API SHRegGetBoolUSValueW(LPCWSTR pszSubKey,
                                   LPCWSTR pszValue,
                                   BOOL    fIgnoreHKCU,
                                   BOOL    fDefault);
    
LONG WIN32API SHRegGetUSValueA(LPCSTR   pSubKey,
                               LPCSTR   pValue,
                               LPDWORD  pwType,
                               LPVOID   pvData,
                               LPDWORD  pbData,
                               BOOL     fIgnoreHKCU,
                               LPVOID   pDefaultData,
                               DWORD    wDefaultDataSize);
    
   
// STRING.CPP
LPSTR WIN32API StrStrIA(LPCSTR lpFirst, LPCSTR lpSrch);

int WINAPI StrCmpNIA ( LPCSTR str1, LPCSTR str2, int len);
int WINAPI StrCmpNIW ( LPCWSTR wstr1, LPCWSTR wstr2, int len);
    

#ifdef __cplusplus
  }
#endif /* defined(__cplusplus) */


#endif
