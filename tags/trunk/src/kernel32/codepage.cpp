/* $Id: codepage.cpp,v 1.6 2000-10-02 21:40:57 phaller Exp $
**
** Module   :CODEPAGE.CPP
** Abstract :
**
** Copyright (C) Vit Timchishin
**
** Log: Wed  22/12/1999 Created
**
*/

#include <odin.h>
#include <odinwrap.h>
#include <os2win.h>
#include "oslibdos.h"
//#include "profile.h"
#include <options.h>
#include "codepage.h"

#define DBG_LOCALLOG	DBG_codepage
#include "dbglocal.h"

static UconvObject DisplayUconv = NULL;
static UconvObject WindowsUconv = NULL;

ULONG GetDisplayCodepage()
{
 static ULONG codepage = -1;

    if(codepage == -1) {
    	codepage = PROFILE_GetOdinIniInt(CODEPAGE_SECTION, "DISPLAY", 0);
    }
    return codepage;
}

ULONG GetWindowsCodepage()
{
 static ULONG codepage = -1;

    if(codepage == -1) {
    	codepage = PROFILE_GetOdinIniInt(CODEPAGE_SECTION, "WINDOWS", 0);
    }
    return codepage;
}

static UconvObject GetObjectByCP(ULONG codepage)
{
    UconvObject rc = 0;
    UniChar   codepage_str[12];

    BOOL ret = UniMapCpToUcsCp(codepage, codepage_str, sizeof(codepage_str));
    if ( ret == ULS_SUCCESS )
    {
	ret = UniCreateUconvObject( codepage_str, &rc );
        if ( ret != ULS_SUCCESS )
          	rc = 0;
    }
    dprintf(("UniCreateUconvObject for CP %d (%08x)\n", codepage, rc ));
    return rc;
}

UconvObject GetDisplayUconvObject()
{
    if (!DisplayUconv)
        DisplayUconv = GetObjectByCP(GetDisplayCodepage());

    return DisplayUconv;
}

UconvObject GetWindowsUconvObject()
{
    if (!WindowsUconv)
        WindowsUconv = GetObjectByCP(GetWindowsCodepage());

    return WindowsUconv;
}

