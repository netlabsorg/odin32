/* $Id: profile.h,v 1.10 2004-03-16 13:34:58 sandervl Exp $ */
/*
 * Profile header for initterm
 * Copyright 1999 Christoph Bratschi
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <odinwrap.h>

/* Odin profile name in KERNEL32.DLL directory */
#define ODINININAME "ODIN.INI"

#ifdef __cplusplus
extern "C" {
#endif

int WINAPI PROFILE_Initialize (void);

#ifdef DEBUG

INT ODIN_EXTERN(GetPrivateProfileStringA)(LPCSTR section, LPCSTR entry,
                                          LPCSTR def_val, LPSTR buffer,
                                          UINT len, LPCSTR filename);

int ODIN_EXTERN(PROFILE_GetOdinIniString)(LPCSTR section, LPCSTR entry,
                                          LPCSTR def_val, LPSTR buffer,
                                          UINT len);

int ODIN_EXTERN(PROFILE_SetOdinIniString)(LPCSTR section_name, LPCSTR key_name,
                                          LPCSTR value);

int ODIN_EXTERN(PROFILE_GetOdinIniInt)(LPCSTR section_name, LPCSTR key_name,
                                          int value);

int ODIN_EXTERN(PROFILE_GetOdinIniBool)(LPCSTR section, LPCSTR key_name,
                                          int def);

UINT ODIN_EXTERN(GetPrivateProfileIntA)(LPCSTR, LPCSTR, INT, LPCSTR);
UINT ODIN_EXTERN(GetPrivateProfileIntW)(LPCWSTR, LPCWSTR, INT, LPCWSTR);
INT  ODIN_EXTERN(GetPrivateProfileStringW)(LPCWSTR, LPCWSTR, LPCWSTR, LPWSTR, UINT, LPCWSTR);
BOOL ODIN_EXTERN(WritePrivateProfileStringA)(LPCSTR, LPCSTR, LPCSTR, LPCSTR);
BOOL ODIN_EXTERN(WritePrivateProfileStringW)(LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR);

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#include <win/options.h> //for odin profile apis

#endif
