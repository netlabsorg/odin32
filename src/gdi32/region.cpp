/* $Id: region.cpp,v 1.2 2000-02-16 14:18:12 sandervl Exp $ */

/*
 * GDI32 region code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "misc.h"

#define DBG_LOCALLOG	DBG_region
#include "dbglocal.h"

//******************************************************************************
//******************************************************************************
HRGN WIN32API CreatePolyPolygonRgn( const POINT * arg1, const INT * arg2, int arg3, int  arg4)
{
    dprintf(("GDI32: CreatePolyPolygonRgn\n"));
    return O32_CreatePolyPolygonRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreatePolygonRgn(const POINT * arg1, int arg2, int arg3)
{
    dprintf(("GDI32: CreatePolygonRgn"));
    return O32_CreatePolygonRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API CombineRgn( HRGN arg1, HRGN  arg2, HRGN  arg3, int  arg4)
{
    dprintf(("GDI32: CombineRgn"));
    return O32_CombineRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateEllipticRgn( int arg1, int arg2, int arg3, int  arg4)
{
    dprintf(("GDI32: CreateEllipticRgn"));
    return O32_CreateEllipticRgn(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateEllipticRgnIndirect( const RECT * arg1)
{
    dprintf(("GDI32: CreateEllipticRgnIndirect"));
    return O32_CreateEllipticRgnIndirect(arg1);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRectRgn( int nLeftRect, int nTopRect, int nRightRect, int nBottomRect)
{
    dprintf(("GDI32: CreateRectRgn (%d,%d)(%d,%d)", nLeftRect, nTopRect, nRightRect, nBottomRect));
    return O32_CreateRectRgn(nLeftRect, nTopRect, nRightRect, nBottomRect);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRectRgnIndirect( const RECT * lpRect)
{
    if(lpRect == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    dprintf(("GDI32: CreateRectRgnIndirect (%d,%d)(%d,%d)", lpRect->left, lpRect->top, lpRect->right, lpRect->bottom));
    return O32_CreateRectRgnIndirect(lpRect);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API CreateRoundRectRgn(int nLeftRect, int nTopRect, int nRightRect, int nBottomRect,
                                 int nWidthEllipse, int nHeightEllipse)
{
    dprintf(("GDI32: CreateRoundRectRgn (%d,%d)(%d,%d) (%d,%d)", nLeftRect, nTopRect, nRightRect, nBottomRect, nWidthEllipse, nHeightEllipse));
    return O32_CreateRoundRectRgn(nLeftRect, nTopRect, nRightRect, nBottomRect, nWidthEllipse, nHeightEllipse);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API EqualRgn( HRGN arg1, HRGN  arg2)
{
    dprintf(("GDI32: EqualRgn"));
    return O32_EqualRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API ExtCreateRegion( const XFORM * arg1, DWORD arg2, const RGNDATA *  arg3)
{
    dprintf(("GDI32: ExtCreateRegion"));
    return O32_ExtCreateRegion(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API ExtSelectClipRgn( HDC arg1, HRGN arg2, int  arg3)
{
    dprintf(("GDI32: ExtSelectClipRgn"));
    return O32_ExtSelectClipRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FillRgn( HDC arg1, HRGN arg2, HBRUSH  arg3)
{
    dprintf(("GDI32: FillRgn"));
    return O32_FillRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FrameRgn( HDC arg1, HRGN arg2, HBRUSH arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: FrameRgn"));
    return O32_FrameRgn(arg1, arg2, arg3, arg4, arg5);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetClipRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: GetClipRgn"));
    return O32_GetClipRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetRegionData( HRGN arg1, DWORD arg2, PRGNDATA  arg3)
{
    dprintf(("GDI32: GetRegionData"));
    return O32_GetRegionData(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API GetRgnBox( HRGN arg1, PRECT  arg2)
{
    dprintf(("GDI32: GetRgnBox"));
    return O32_GetRgnBox(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API InvertRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: InvertRgn"));
    return O32_InvertRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetClipRgn( HDC arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OffsetClipRgn"));
    return O32_OffsetClipRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
int WIN32API OffsetRgn( HRGN arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: OffsetRgn"));
    return O32_OffsetRgn(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PaintRgn( HDC arg1, HRGN  arg2)
{
    dprintf(("GDI32: PaintRgn"));
    return O32_PaintRgn(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
HRGN WIN32API PathToRegion( HDC arg1)
{
    dprintf(("GDI32: PathToRegion"));
    return O32_PathToRegion(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API PtInRegion( HRGN arg1, int arg2, int  arg3)
{
    dprintf(("GDI32: PtInRegion"));
    return O32_PtInRegion(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API RectInRegion( HRGN arg1, const RECT *  arg2)
{
    dprintf(("GDI32: RectInRegion"));
    return O32_RectInRegion(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API SelectClipRgn( HDC hdc, HRGN hRgn)
{
    dprintf(("GDI32: SelectClipRgn %x %x", hdc, hRgn));
    return O32_SelectClipRgn(hdc, hRgn);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetRectRgn( HRGN arg1, int arg2, int arg3, int arg4, int  arg5)
{
    dprintf(("GDI32: SetRectRgn"));
    return O32_SetRectRgn(arg1, arg2, arg3, arg4, arg5);
}
/*****************************************************************************
 * Name      : int GetMetaRgn
 * Purpose   : The GetMetaRgn function retrieves the current metaregion for
 *             the specified device context.
 * Parameters: HDC  hdc   handle of device context
 *             HRGN hrgn  handle of region
 * Variables :
 * Result    : 0 / 1
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

int WIN32API GetMetaRgn(HDC  hdc,
                           HRGN hrgn)
{
  dprintf(("GDI32: GetMetaRgn(%08xh, %08xh) not implemented.\n",
           hdc,
           hrgn));

  return (0);
}
/*****************************************************************************
 * Name      : int SetMetaRgn
 * Purpose   : The SetMetaRgn function intersects the current clipping region
 *             for the specified device context with the current metaregion
 *             and saves the combined region as the new metaregion for the
 *             specified device context. The clipping region is reset to a null region.
 * Parameters: HDC    hdc   handle of device context
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API SetMetaRgn(HDC hdc)
{
  dprintf(("GDI32: SetMetaRgn(%08xh) not implemented.\n",
           hdc));

  return (NULLREGION);
}
//******************************************************************************
//******************************************************************************
