/* $Id: shlwapi_odin.h,v 1.1 2000-08-24 09:32:44 sandervl Exp $ */

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
