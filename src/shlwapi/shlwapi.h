/* $Id: shlwapi.h,v 1.2 2000-06-12 08:09:44 phaller Exp $ */

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
    
LPSTR WIN32API StrStrIA(LPCSTR lpFirst, LPCSTR lpSrch);
    

#ifdef __cplusplus
  }
#endif /* defined(__cplusplus) */


#endif
