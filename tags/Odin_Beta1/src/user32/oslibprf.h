/* $Id: oslibprf.h,v 1.2 2001-07-14 08:36:18 sandervl Exp $ */
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

LONG OSLibPrfQueryProfileInt(OSLIB_HINI hini, char *pszApp, char *pszKey, LONG sDefault);
LONG OSLibPrfQueryProfileString(OSLIB_HINI hini, char *pszApp, char *pszKey, char *pszDefault, char *buffer, ULONG buflen);

#endif
