/* $Id: oslibprf.cpp,v 1.2 2001-07-14 08:36:17 sandervl Exp $ */
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
#define  INCL_WIN
#define  INCL_PM
#define  INCL_WINSWITCHLIST
#include <os2wrap.h>

#include <win32type.h>
#include <winconst.h>
#include "oslibprf.h"

LONG OSLibPrfQueryProfileInt(OSLIB_HINI hini, char *pszApp, char *pszKey, LONG sDefault)
{
    return PrfQueryProfileInt(hini, pszApp, pszKey, sDefault);
}

LONG OSLibPrfQueryProfileString(OSLIB_HINI hini, char *pszApp, char *pszKey, char *pszDefault, char *buffer, ULONG buflen)
{
    return PrfQueryProfileString(hini, pszApp, pszKey, pszDefault, buffer, buflen);
}
