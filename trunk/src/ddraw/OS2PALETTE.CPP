/* $Id: OS2PALETTE.CPP,v 1.15 2001-10-03 13:49:40 sandervl Exp $ */

/*
 * DX palette class implementation
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#define INITGUID
#include "os2ddraw.h"
#include "os2clipper.h"
#include "os2palette.h"
#include "os2surface.h"
#define _OS2WIN_H
#define FAR
#include <misc.h>
#include "os2palset.h"
#include "os2fsdd.h"
#include <winerror.h>

#undef THIS
#define THIS IDirectDrawPalette*

//******************************************************************************
//******************************************************************************
OS2IDirectDrawPalette::OS2IDirectDrawPalette( VOID *lpDirectDraw,
                                              int palsize,
                                              LPPALETTEENTRY lpColorTable,
                                              DWORD dwPalFlags) :
                                              Referenced(0), os2pal(NULL),
                                              lastError(DD_OK), lpDraw(NULL)

{
  lpVtbl              = &Vtbl;
  Vtbl.AddRef         = PalAddRef;
  Vtbl.Release        = PalRelease;
  Vtbl.QueryInterface = PalQueryInterface;
  Vtbl.GetCaps        = PalGetCaps;
  Vtbl.GetEntries     = PalGetEntries;
  Vtbl.Initialize     = PalInitialize;
  Vtbl.SetEntries     = PalSetEntries;

  lpDraw                        = lpDirectDraw;
  ((OS2IDirectDraw*)lpDraw)->Vtbl.AddRef(lpDraw);
  hDive                         = ((OS2IDirectDraw*)lpDirectDraw)->GetDiveInstance();
  dwCaps                        = dwPalFlags;
  hDiveCC                       = ((OS2IDirectDraw*)lpDirectDraw)->GetCCDiveInstance();

  dwSize = palsize;
  if(256==dwSize)
    dwCaps |= DDPCAPS_ALLOW256;
  dwCaps &= ~DDPCAPS_VSYNC; // No sync change

  os2pal = (LPPALETTEENTRY)malloc(palsize*sizeof(PALETTEENTRY));
  memcpy((char *)os2pal, (char *)lpColorTable, palsize*sizeof(PALETTEENTRY));

  dprintf(("DDRAW: Init CC PalTable"));
  for(DWORD i=0;i<dwSize;i++)
  {
    aPal16[i] = (lpColorTable[i].peBlue>>3) +
                ((lpColorTable[i].peGreen>>2) <<5) +
                ((lpColorTable[i].peRed>>3) << 11);
    dprintf(( " # %d : RGB=%02X/%02X/%02X => %04X\n",
              i,
              lpColorTable[i].peRed,
              lpColorTable[i].peGreen,
              lpColorTable[i].peBlue,
              aPal16[i]));
    aPal24[i] = (lpColorTable[i].peBlue <<8) +
                (lpColorTable[i].peGreen<<16) +
                (lpColorTable[i].peRed<<24);
  }
}
//******************************************************************************
//******************************************************************************
OS2IDirectDrawPalette::~OS2IDirectDrawPalette()
{
  if(os2pal)
    free(os2pal);
  ((OS2IDirectDraw*)lpDraw)->Vtbl.Release((OS2IDirectDraw*)lpDraw);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalQueryInterface(THIS This, REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("DDRAW: OS2IDirectDrawPalette::PalQueryInterface"));

  *ppvObj = NULL;

  if(!IsEqualGUID(riid, IID_IDirectDrawPalette))
//&& !IsEqualGUID(riid, IID_IUnknown))
  return E_NOINTERFACE;

  *ppvObj = This;

  PalAddRef(This);
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall PalAddRef(THIS This)
{
  OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  dprintf(("DDRAW: OS2IDirectDrawPalette::PalAddRef %d", me->Referenced+1));

  return (++me->Referenced);
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall PalRelease(THIS This)
{
  OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  dprintf(("DDRAW: OS2IDirectDrawPalette::PalRelease %d", me->Referenced-1));

  if(me->Referenced)
  {
    me->Referenced--;
    if(me->Referenced == 0)
    {
      delete(me);
      return(0);
    }
    else
      return (me->Referenced);
  }
  else
    return(0);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalGetCaps(THIS This, LPDWORD lpdwCaps)
{
  OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  dprintf(("DDRAW: OS2IDirectDrawPalette::GetCaps"));

  if(NULL== lpdwCaps)
    return(DDERR_INVALIDPARAMS);

  *lpdwCaps = me->dwCaps;

  if(me->fAttachedToPrimary)
    *lpdwCaps |= DDPCAPS_PRIMARYSURFACE;

  return(DD_OK);
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalGetEntries(THIS This, DWORD dwFlags,
                                      DWORD dwBase,
                                      DWORD dwNumEntries,
                                      LPPALETTEENTRY lpEntries)
{
 OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  dprintf(("DDRAW: OS2IDirectDrawPalette::PalGetEntries"));

  if ((NULL == lpEntries) || (0 != dwFlags) || (dwBase > 0xFFFF) ||
	 ((dwBase + dwNumEntries) > me->dwSize))
	return(DDERR_INVALIDPARAMS);

  memcpy( (char *)lpEntries,
          (char *)(me->os2pal + dwBase),
          dwNumEntries*sizeof(PALETTEENTRY));

  return(DD_OK);
}

//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalInitialize(THIS, LPDIRECTDRAW, DWORD, LPPALETTEENTRY)
{
  dprintf(("DDRAW: OS2IDirectDrawPalette::PalInitialize"));
  return(DDERR_ALREADYINITIALIZED);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalSetEntries(THIS This, DWORD dwFlags,
                                      DWORD dwBase,
                                      DWORD dwNumEntries,
                                      LPPALETTEENTRY lpNewEntries)
{
 OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  dprintf(("DDRAW: OS2IDirectDrawPalette::PalSetEntries\n"));
  if ((NULL == lpNewEntries) || (0 != dwFlags) || (dwBase > 0xFFFF) ||
      ((dwBase + dwNumEntries) > me->dwSize))
    return(DDERR_INVALIDPARAMS);

  memcpy((char *)(me->os2pal + dwBase),
         (char *)lpNewEntries,
         dwNumEntries*sizeof(PALETTEENTRY));

  dprintf(("DDRAW: Setting up CC Palentries:\n"));
  for(DWORD i=0;i<dwNumEntries;i++)
  {
    me->aPal16[dwBase+i] = (lpNewEntries[i].peBlue>>3) +
                           ((lpNewEntries[i].peGreen>>2) <<5) +
                           ((lpNewEntries[i].peRed>>3) << 11);
    dprintf(( " # %d : RGB=%02X/%02X/%02X => %04X\n",
              dwBase+i,
              lpNewEntries[i].peRed,
              lpNewEntries[i].peGreen,
              lpNewEntries[i].peBlue,
              me->aPal16[dwBase+i]));
    me->aPal24[dwBase+i] = (lpNewEntries[i].peBlue <<8) +
                           (lpNewEntries[i].peGreen<<16) +
                           (lpNewEntries[i].peRed<<24);
  }

  if (8==((OS2IDirectDraw*)me->lpDraw)->dCaps.ulDepth)
  {
    if(me->fAttachedToPrimary)
      me->SetPhysPalette();
  }
  else
  {
    if(NULL!=((OS2IDirectDraw*)me->lpDraw)->pPrimSurf)
      ((OS2IDirectDrawSurface*)((OS2IDirectDraw*)me->lpDraw)->pPrimSurf)->ColorConversion(NULL);
  }
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
void OS2IDirectDrawPalette::SetPhysPalette()
{
	// Run appropriate code depening on whether FS DDraw is enabled
	if (bUseFSDD)
		SetSVGAPalette(os2pal);	
	else	
		OS2SetPhysPalette(os2pal);
}
//******************************************************************************
//******************************************************************************
void OS2IDirectDrawPalette::RestorePhysPalette()
{
  OS2ResetPhysPalette();
}
//******************************************************************************
//******************************************************************************
void OS2IDirectDrawPalette::SetIsPrimary(BOOL fNewVal)
{
  if(fNewVal==fAttachedToPrimary)
    return;
  fAttachedToPrimary = fNewVal;

  /*
  if(fAttachedToPrimary)
  {
    if(NULL!=hDiveCC)
      DiveSetSourcePalette( hDiveCC,
                            0,
                            255,
                            (PBYTE)os2pal);
    else
      SetPhysPalette();
  }
  */
}
//******************************************************************************
//******************************************************************************

