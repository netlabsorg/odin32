/* $Id: palette.cpp,v 1.4 2000-05-22 19:11:28 sandervl Exp $ */

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
#include "dibsect.h"

#define DBG_LOCALLOG	DBG_palette
#include "dbglocal.h"

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
//******************************************************************************
//******************************************************************************
UINT WIN32API RealizePalette( HDC hdc)
{
    dprintf(("GDI32: RealizePalette %x", hdc));
    return O32_RealizePalette(hdc);
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API CreatePalette( const LOGPALETTE * arg1)
{
  HPALETTE rc;

   for(int i=0; i<arg1->palNumEntries;i++)
   {
     	dprintf2(("Index %d : 0x%08X\n",i, *((DWORD*)(&arg1->palPalEntry[i])) ));
   }
   rc = O32_CreatePalette(arg1);
   dprintf(("GDI32: CreatePalette returns 0x%08X\n",rc));

   return rc;
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API SelectPalette(HDC arg1, HPALETTE arg2, BOOL arg3)
{
  dprintf(("GDI32: SelectPalette (0x%08X, 0x%08X, 0x%08X)\n", arg1, arg2, arg3));
  if(DIBSection::getSection() != NULL)
  {
    DIBSection *dsect = DIBSection::findHDC(arg1);
    if(dsect)
    {
      PALETTEENTRY Pal[256];
      char PalSize = dsect->GetBitCount();
      dprintf(("       - Set Palette Values in DIBSection\n"));
      if(PalSize<=8)
      {
        GetPaletteEntries( arg2, 0, 1<<PalSize, (LPPALETTEENTRY)&Pal);
        dsect->SetDIBColorTable(0, 1<< PalSize, (RGBQUAD*)&Pal);
      }

    }
  }
  return O32_SelectPalette(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API AnimatePalette( HPALETTE arg1, UINT arg2, UINT arg3, const PALETTEENTRY * arg4)
{
    dprintf(("GDI32: AnimatePalette"));
    return O32_AnimatePalette(arg1, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetNearestPaletteIndex( HPALETTE arg1, COLORREF  arg2)
{
    UINT rc;
    dprintf(("GDI32: GetNearestPaletteIndex (0x%08X ,0x%08X) ",arg1,arg2));
    rc = O32_GetNearestPaletteIndex(arg1, arg2);
    dprintf(("Returns %d\n",rc));
    return rc;
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetPaletteEntries( HPALETTE hPalette, UINT arg2, UINT arg3, PPALETTEENTRY  arg4)
{
    dprintf(("GDI32: GetPaletteEntries %x %d-%d %x", hPalette, arg2, arg3, arg4));
    return O32_GetPaletteEntries(hPalette, arg2, arg3, arg4);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetSystemPaletteEntries( HDC arg1, UINT arg2, UINT arg3, PPALETTEENTRY  arg4)
{
 UINT rc;

    dprintf(("GDI32: GetSystemPaletteEntries start %d nr %d pal ptr %X", arg2, arg3, arg4));
    rc = O32_GetSystemPaletteEntries(arg1, arg2, arg3, arg4);
    dprintf((" GetSystemPaletteEntries returned %d", rc));
    return(rc);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API ResizePalette( HPALETTE arg1, UINT  arg2)
{
    dprintf(("GDI32: ResizePalette\n"));
    return O32_ResizePalette(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API SetPaletteEntries( HPALETTE hPalette, UINT arg2, UINT arg3, PALETTEENTRY * arg4)
{
    dprintf(("GDI32: SetPaletteEntries %x %d-%d %x", hPalette, arg2, arg3, arg4));
    return O32_SetPaletteEntries(hPalette, arg2, arg3, (const PALETTEENTRY *)arg4);
}
//******************************************************************************
//******************************************************************************
HPALETTE WIN32API CreateHalftonePalette(HDC hdc)
{
    dprintf(("GDI32: CreateHalftonePalette, not implemented\n"));
    return(NULL);
}
//******************************************************************************
//******************************************************************************
