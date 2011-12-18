/* $Id: oslibprf.h,v 1.3 2003-02-25 11:57:49 sandervl Exp $ */
/*
 * Profile API wrappers for OS/2
 *
 *
 * Copyright 2001 Achim Hasenmueller <achimha@innotek.de>
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __OSLIBPRF_H__
#define __OSLIBPRF_H__

typedef ULONG                    OSLIB_HINI;
#define OSLIB_HINI_PROFILE       NULL
#define OSLIB_HINI_USERPROFILE   -1L
#define OSLIB_HINI_SYSTEMPROFILE -2L
#define OSLIB_HINI_USER          OSLIB_HINI_USERPROFILE
#define OSLIB_HINI_SYSTEM        OSLIB_HINI_SYSTEMPROFILE

extern "C" {

LONG OSLibPrfQueryProfileInt(OSLIB_HINI hini, const char *pszApp, const char *pszKey, LONG sDefault);
LONG OSLibPrfQueryProfileString(OSLIB_HINI hini, const char *pszApp, const char *pszKey, const char *pszDefault, char *buffer, ULONG buflen);

}

#endif
