/* $Id: codepage.cpp,v 1.9 2001-04-01 12:33:10 sandervl Exp $
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

static ULONG windowCodePage = -1;
static ULONG displayCodePage = -1;

ULONG GetDisplayCodepage()
{
    if(displayCodePage == -1) {
        //default codepage is 1252 (same as default Windows codepage)
    	displayCodePage = PROFILE_GetOdinIniInt(CODEPAGE_SECTION, "DISPLAY", 1252);
//    	displayCodePage = PROFILE_GetOdinIniInt(CODEPAGE_SECTION, "DISPLAY", 0);
    }
    return displayCodePage;
}

ULONG GetWindowsCodepage()
{
    if(windowCodePage == -1) {
        //default codepage is 1252 (same as default Windows codepage)
    	windowCodePage = PROFILE_GetOdinIniInt(CODEPAGE_SECTION, "WINDOWS", 1252);
//    	windowCodePage = PROFILE_GetOdinIniInt(CODEPAGE_SECTION, "WINDOWS", 0);
    }
    return windowCodePage;
}

UINT WIN32API GetACP()
{
    UINT codepage = GetDisplayCodepage();
    dprintf(("GetACP -> %d", codepage));

    return(codepage);
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

