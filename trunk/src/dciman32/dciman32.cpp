/* $Id: dciman32.cpp,v 1.1 2000-10-23 11:24:38 sandervl Exp $ */
/*
 * DCIMAN32 Stub code
 *
 * Copyright 2000 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>


//******************************************************************************
//******************************************************************************
HDC WIN32API DCIOpenProvider()
{
    dprintf(("DCIOpenProvider"));
    return CreateDCA("DISPLAY", 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API DCICloseProvider(HDC hdc)
{
    dprintf(("DCICloseProvide %x", hdc));
    return DeleteDC(hdc);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCIEnum(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5)
{
    dprintf(("DCIEnum %x %x %x %x %x NOT IMPLEMENTED", arg1, arg2, arg3, arg4, arg5));
    return -4;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCICreatePrimary(DWORD arg1, DWORD arg2)
{
    dprintf(("DCICreatePrimary %x %x NOT IMPLEMENTED", arg1, arg2));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCICreateOffscreen(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5,
                                  DWORD arg6, DWORD arg7, DWORD arg8, DWORD arg9, DWORD arg10)
{
    dprintf(("DCICreateOffScreen %x %x %x %x %x %x %x %x %x %x NOT IMPLEMENTED", arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));
    return -4;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCICreateOverlay(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("DCICreateOverlay %x %x %x NOT IMPLEMENTED", arg1, arg2, arg3));
    return -4;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCIBeginAccess(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5)
{
    dprintf(("DCIBeginAccess %x %x %x %x %x NOT IMPLEMENTED", arg1, arg2, arg3, arg4, arg5));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCIEndAccess(DWORD arg1)
{
    dprintf(("DCIEndAccess %x NOT IMPLEMENTED", arg1));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCIDestroy(DWORD arg1)
{
    dprintf(("DCIDestroy %x NOT IMPLEMENTED", arg1));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCIDraw(DWORD arg1)
{
    dprintf(("DCIDraw %x NOT IMPLEMENTED", arg1));
    return -4;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCISetClipList(DWORD arg1, DWORD arg2)
{
    dprintf(("DCISetClipList %x %x NOT IMPLEMENTED", arg1, arg2));
    return -4;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCISetDestination(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("DCISetDestination %x %x %x NOT IMPLEMENTED", arg1, arg2, arg3));
    return -4;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API DCISetSrcDestClip(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4)
{
    dprintf(("DCISetSrcDestClip %x %x %x %x NOT IMPLEMENTED", arg1, arg2, arg3, arg4));
    return -4;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API WinWatchOpen(DWORD arg1)
{
    dprintf(("WinWatchOpen %x NOT IMPLEMENTED", arg1));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API WinWatchClose(DWORD arg1)
{
    dprintf(("WinWatchClose %x NOT IMPLEMENTED", arg1));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API WinWatchGetClipList(DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4)
{
    dprintf(("WinWatchGetClipList %x %x %x %xNOT IMPLEMENTED", arg1, arg2, arg3, arg4));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API WinWatchDidStatusChange(DWORD arg1)
{
    dprintf(("WinWatchDidStatusChange %x NOT IMPLEMENTED", arg1));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API WinWatchNotify(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("WinWatchNotify %x NOT IMPLEMENTED", arg1));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetWindowRegionData(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("GetWindowRegionData %x %x %x NOT IMPLEMENTED", arg1, arg2, arg3));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetDCRegionData(DWORD arg1, DWORD arg2, DWORD arg3)
{
    dprintf(("GetDCRegionData %x %x %x NOT IMPLEMENTED", arg1, arg2, arg3));
    return 0;
}
//******************************************************************************
//******************************************************************************

