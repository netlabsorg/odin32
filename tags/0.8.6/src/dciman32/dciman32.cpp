/* $Id: dciman32.cpp,v 1.2 2001-05-29 20:25:27 sandervl Exp $ */
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

#include <dciman.h>

//******************************************************************************
//******************************************************************************
HDC WIN32API DCIOpenProvider()
{
    dprintf(("DCIOpenProvider"));
    return CreateDCA("DISPLAY", 0, 0, 0);
}
//******************************************************************************
//******************************************************************************
void WIN32API DCICloseProvider(HDC hdc)
{
    dprintf(("DCICloseProvide %x", hdc));
    DeleteDC(hdc);
}
//******************************************************************************
//******************************************************************************
int WINAPI DCIEnum(HDC hdc, LPRECT lprDst, LPRECT lprSrc, LPVOID lpFnCallback,
                   LPVOID lpContext)
{
    dprintf(("DCIEnum %x %x %x %x %x NOT IMPLEMENTED", hdc, lprDst, lprSrc, lpFnCallback, lpContext));
    return DCI_FAIL_UNSUPPORTED;  //NT returns -4
}
//******************************************************************************
//******************************************************************************
int WINAPI DCICreatePrimary(HDC hdc, LPDCISURFACEINFO *lplpSurface)
{
    dprintf(("DCICreatePrimary %x %x NOT IMPLEMENTED", hdc, lplpSurface));
    return DCI_FAIL_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
int WINAPI DCICreateOffscreen(HDC hdc, DWORD dwCompression, DWORD dwRedMask,
                              DWORD dwGreenMask, DWORD dwBlueMask, DWORD dwWidth, 
                              DWORD dwHeight, DWORD dwDCICaps, DWORD dwBitCount, 
                              LPDCIOFFSCREEN  *lplpSurface)
{
    dprintf(("DCICreateOffScreen %x %x %x %x %x %x %x %x %x %x NOT IMPLEMENTED", hdc, dwCompression, dwRedMask, dwGreenMask, dwBlueMask, dwWidth, dwHeight, dwDCICaps, dwBitCount, lplpSurface));
    return DCI_FAIL_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
int WINAPI DCICreateOverlay(HDC hdc, LPVOID lpOffscreenSurf, 
                            LPDCIOVERLAY *lplpSurface)
{
    dprintf(("DCICreateOverlay %x %x %x NOT IMPLEMENTED", hdc, lpOffscreenSurf, lplpSurface));
    return DCI_FAIL_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
DCIRVAL WINAPI DCIBeginAccess(LPDCISURFACEINFO pdci, int x, int y, int dx, int dy)
{
    dprintf(("DCIBeginAccess %x %x %x %x %x NOT IMPLEMENTED", pdci, x, y, dx, dy));
    return DCI_FAIL_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
void WINAPI DCIEndAccess(LPDCISURFACEINFO pdci)
{
    dprintf(("DCIEndAccess %x NOT IMPLEMENTED", pdci));
    return;
}
//******************************************************************************
//******************************************************************************
void WINAPI DCIDestroy(LPDCISURFACEINFO pdci)
{
    dprintf(("DCIDestroy %x NOT IMPLEMENTED", pdci));
    return;
}
//******************************************************************************
//******************************************************************************
DCIRVAL WINAPI DCIDraw(LPDCIOFFSCREEN pdci)
{
    dprintf(("DCIDraw %x NOT IMPLEMENTED", pdci));
    return DCI_FAIL_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
DCIRVAL WINAPI DCISetClipList(LPDCIOFFSCREEN pdci, LPRGNDATA prd)
{
    dprintf(("DCISetClipList %x %x NOT IMPLEMENTED", pdci, prd));
    return DCI_FAIL_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
DCIRVAL WINAPI DCISetDestination(LPDCIOFFSCREEN pdci, LPRECT dst, LPRECT src)
{
    dprintf(("DCISetDestination %x %x %x NOT IMPLEMENTED", pdci, dst, src));
    return DCI_FAIL_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
DCIRVAL WINAPI DCISetSrcDestClip(LPDCIOFFSCREEN pdci, LPRECT srcrc, LPRECT destrc, LPRGNDATA prd )
{
    dprintf(("DCISetSrcDestClip %x %x %x %x NOT IMPLEMENTED", pdci, srcrc, destrc, prd));
    return DCI_FAIL_UNSUPPORTED;
}
//******************************************************************************
//******************************************************************************
HWINWATCH WINAPI WinWatchOpen(HWND hwnd)
{
    dprintf(("WinWatchOpen %x NOT IMPLEMENTED", hwnd));
    return 0;
}
//******************************************************************************
//******************************************************************************
void WINAPI WinWatchClose(HWINWATCH hWW)
{
    dprintf(("WinWatchClose %x NOT IMPLEMENTED", hWW));
    return;
}
//******************************************************************************
//******************************************************************************
UINT WINAPI WinWatchGetClipList(HWINWATCH hWW, LPRECT prc, UINT size,  LPRGNDATA prd)
{
    dprintf(("WinWatchGetClipList %x %x %x %xNOT IMPLEMENTED", hWW, prc, size, prd));
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI WinWatchDidStatusChange(HWINWATCH hWW)
{
    dprintf(("WinWatchDidStatusChange %x NOT IMPLEMENTED", hWW));
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WINAPI WinWatchNotify(HWINWATCH hWW, WINWATCHNOTIFYPROC NotifyCallback,
                           LPARAM NotifyParam )
{
    dprintf(("WinWatchNotify %x NOT IMPLEMENTED", hWW, NotifyCallback, NotifyParam));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI GetWindowRegionData(HWND hwnd, DWORD size, LPRGNDATA prd)
{
    dprintf(("GetWindowRegionData %x %x %x NOT IMPLEMENTED", hwnd, size, prd));
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD WINAPI GetDCRegionData(HDC hdc, DWORD size, LPRGNDATA prd)
{
    dprintf(("GetDCRegionData %x %x %x NOT IMPLEMENTED", hdc, size, prd));
    return 0;
}
//******************************************************************************
//******************************************************************************

