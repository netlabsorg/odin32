/* $Id: profile.h,v 1.4 1999-11-09 14:19:46 sandervl Exp $ */
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

void WINAPI WriteOutProfiles(void);
int WINAPI PROFILE_LoadOdinIni(void);

INT ODIN_EXTERN(GetPrivateProfileStringA)(LPCSTR section, LPCSTR entry,
                                          LPCSTR def_val, LPSTR buffer,
                                          UINT len, LPCSTR filename);

int ODIN_EXTERN(PROFILE_GetOdinIniString)(LPCSTR section, LPCSTR entry,
                                          LPCSTR def_val, LPSTR buffer,
                                          UINT len);
#endif
