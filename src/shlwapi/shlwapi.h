/* $Id: shlwapi.h,v 1.3 2000-06-12 11:35:14 phaller Exp $ */

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
    
// PATH.CPP
LPSTR WIN32API PathRemoveFileSpecA(LPSTR lpstrFilename);
LPSTR WIN32API PathAppendA(LPSTR lpstrPath,
                           LPSTR lpstrAppend);
    
// STRING.CPP
LPSTR WIN32API StrStrIA(LPCSTR lpFirst, LPCSTR lpSrch);
    

#ifdef __cplusplus
  }
#endif /* defined(__cplusplus) */


#endif
