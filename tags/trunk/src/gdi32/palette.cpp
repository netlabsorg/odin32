/* $Id: palette.cpp,v 1.1 1999-12-01 23:30:30 sandervl Exp $ */

/*
 * GDI32 palette apis
 *
 * Based on Wine code (991031) (objects\palette.c)
 *
 * Copyright 1993,1994 Alexandre Julliard
 * Copyright 1996 Alex Korobka
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdlib.h>
#include <misc.h>
#include <string.h>

static UINT SystemPaletteUse = SYSPAL_STATIC;  /* currently not considered */

/***********************************************************************
 * SetSystemPaletteUse32 [GDI32.335]
 *
 * RETURNS
 *    Success: Previous system palette
 *    Failure: SYSPAL_ERROR
 */
UINT WINAPI SetSystemPaletteUse(
    HDC hdc,  /* [in] Handle of device context */
    UINT use) /* [in] Palette-usage flag */
{
    UINT old = SystemPaletteUse;
    dprintf(("SetSystemPaletteUse: (%04x,%04x): stub\n", hdc, use ));
    SystemPaletteUse = use;
    return old;
}
/***********************************************************************
 * GetSystemPaletteUse32 [GDI32.223]  Gets state of system palette
 *
 * RETURNS
 *    Current state of system palette
 */
UINT WINAPI GetSystemPaletteUse(HDC hdc) /* [in] Handle of device context */
{
    dprintf(("GetSystemPaletteUse %x", hdc));
    return SystemPaletteUse;
}
