/* $Id: OS2PALETTE.CPP,v 1.3 1999-06-10 17:10:56 phaller Exp $ */

/*
 * DirectDraw Palette class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_GUID
#define WIN32SDK_NOPOSTWRAPPER

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2win.h>
#include <dive.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "no.h"
#include <w_windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <Win32SDKPostWrapper.h>

#include "os2ddraw.h"
#include "os2palette.h"
#include "misc.h"
#include "os2palset.h"

extern DIVE_CAPS dcaps;

//******************************************************************************
//******************************************************************************
OS2IDirectDrawPalette::OS2IDirectDrawPalette(OS2IDirectDraw *lpDirectDraw,
                         int palsize,
                         W32_LPPALETTEENTRY lpColorTable) :
                         Referenced(0), os2pal(NULL),
                         lastError(DD_OK), lpDraw(NULL)
{
  lpVtbl                        = &Vtbl;
  Vtbl.AddRef           = PalAddRef;
  Vtbl.Release              = PalRelease;
  Vtbl.QueryInterface       = PalQueryInterface;
  Vtbl.GetCaps          = PalGetCaps;
  Vtbl.GetEntries       = PalGetEntries;
  Vtbl.Initialize       = PalInitialize;
  Vtbl.SetEntries       = PalSetEntries;

  lpDraw                        = lpDirectDraw;
  lpDraw->Vtbl.AddRef((IDirectDraw2*)lpDraw);
  hDive                         = lpDirectDraw->GetDiveInstance();
  nrColors                      = palsize;

  dprintf(("OS2IDirectDrawPalette::OS2IDirectDrawPalette nr colors %d", palsize));
  os2pal = (W32_LPPALETTEENTRY)malloc(palsize*sizeof(PALETTEENTRY));
  memcpy((char *)os2pal, (char *)lpColorTable, palsize*sizeof(PALETTEENTRY));
}
//******************************************************************************
//******************************************************************************
OS2IDirectDrawPalette::~OS2IDirectDrawPalette()
{
  if(os2pal)    free(os2pal);
  lpDraw->Vtbl.Release((IDirectDraw2*)lpDraw);
  RestorePhysPalette();
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj)
{
  dprintf(("OS2IDirectDrawPalette::PalQueryInterface\n"));
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
ULONG __stdcall PalAddRef(THIS)
{
 OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  dprintf(("OS2IDirectDrawPalette::PalAddRef %d\n", me->Referenced+1));
  return ++me->Referenced;
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall PalRelease(THIS)
{
 OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  dprintf(("OS2IDirectDrawPalette::PalRelease %d\n", me->Referenced-1));
  if(me->Referenced) {
    me->Referenced--;
    if(me->Referenced == 0) {
        delete me;
        return(0);
    }
    else    return me->Referenced;
  }
  else  return(0);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalGetCaps(THIS_ LPDWORD)
{
  dprintf(("OS2IDirectDrawPalette::GetCaps\n"));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalGetEntries(THIS_ DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries,
                        W32_LPPALETTEENTRY lpEntries)
{
 OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  dprintf(("OS2IDirectDrawPalette::PalGetEntries %d %d\n", dwBase, dwNumEntries));

  if(dwBase + dwNumEntries > 256)
    return(DDERR_UNSUPPORTED);

  memcpy((char *)lpEntries, (char *)(me->os2pal + dwBase), dwNumEntries*sizeof(PALETTEENTRY));
  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalInitialize(THIS_ LPDIRECTDRAW, DWORD, W32_LPPALETTEENTRY)
{
  dprintf(("OS2IDirectDrawPalette::PalInitialize\n"));
  return(DD_OK);
}
//******************************************************************************
   #pragma pack(1)          /* pack on wordboundary */

   typedef struct _RGB2         /* rgb2 */
   {
      BYTE bBlue;              /* Blue component of the color definition */
      BYTE bGreen;             /* Green component of the color definition*/
      BYTE bRed;               /* Red component of the color definition  */
      BYTE fcOptions;          /* Reserved, must be zero                 */
   } RGB2;
   typedef RGB2 *PRGB2;
 #pragma pack()
//******************************************************************************
HRESULT __stdcall PalSetEntries(THIS_ DWORD dwFlags, DWORD dwStartingEntry,
                DWORD dwCount, W32_LPPALETTEENTRY lpEntries)
{
 OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;
 int i, rc;
 RGB2 os2rgb[256];

   dprintf(("OS2IDirectDrawPalette::PalSetEntries %X %X %d %d\n", me, dwFlags, dwStartingEntry, dwCount));
   memcpy(&me->os2pal[dwStartingEntry], lpEntries, sizeof(PALETTEENTRY)*dwCount);

   for(i=0;i<256;i++) {
    os2rgb[i].bBlue     = me->os2pal[i].peBlue;
    os2rgb[i].bGreen    = me->os2pal[i].peGreen;
    os2rgb[i].bRed      = me->os2pal[i].peRed;
    os2rgb[i].fcOptions = 0;
//  dprintf(("pal (%3d,%3d,%3d) %d\n", me->os2pal[i].peBlue, me->os2pal[i].peGreen, me->os2pal[i].peRed, me->os2pal[i].peFlags));
   }
#if 0
   rc = DiveSetSourcePalette(me->hDive, dwStartingEntry, dwCount, (PBYTE)os2rgb);
   if(rc != DIVE_SUCCESS) {
    dprintf(("DiveSetSourcePalette returned %d\n", rc));
    return(DDERR_INVALIDPARAMS);
   }
   if(dcaps.ulDepth == 8) {
    rc = DiveSetDestinationPalette(me->hDive, dwStartingEntry, dwCount, (PBYTE)os2rgb);
    if(rc != DIVE_SUCCESS) {
        dprintf(("DiveSetDestinationPalette returned %d\n", rc));
        return(DDERR_INVALIDPARAMS);
    }
   }
#else
   if(dcaps.ulDepth == 8) {
    OS2SetPhysPalette((HWND)me->lpDraw->GetWindowHandle(), me->os2pal);
    rc = DiveSetDestinationPalette(me->hDive, 0, 0, 0);
    if(rc != DIVE_SUCCESS) {
        dprintf(("DiveSetDestinationPalette returned %d\n", rc));
        return(DDERR_INVALIDPARAMS);
    }
   }
   else {
    rc = DiveSetSourcePalette(me->hDive, 0, me->nrColors, (PBYTE)os2rgb);
    if(rc != DIVE_SUCCESS) {
        dprintf(("DiveSetSourcePalette returned %d\n", rc));
        return(DDERR_INVALIDPARAMS);
    }
   }
#endif
   return(DD_OK);
}
//******************************************************************************
//******************************************************************************
void OS2IDirectDrawPalette::SetPhysPalette()
{
 int i, rc;
 RGB2 os2rgb[256];

   for(i=0;i<nrColors;i++) {
    os2rgb[i].bBlue     = os2pal[i].peBlue;
    os2rgb[i].bGreen    = os2pal[i].peGreen;
    os2rgb[i].bRed      = os2pal[i].peRed;
    os2rgb[i].fcOptions = 0;
    dprintf(("pal (%3d,%3d,%3d) %d\n", os2pal[i].peBlue, os2pal[i].peGreen, os2pal[i].peRed, os2pal[i].peFlags));
   }
#if 0
   rc = DiveSetSourcePalette(hDive, 0, nrColors, (PBYTE)os2rgb);
   if(rc != DIVE_SUCCESS) {
    dprintf(("DiveSetSourcePalette returned %d\n", rc));
    return;
   }
   if(dcaps.ulDepth == 8) {
    rc = DiveSetDestinationPalette(hDive, 0, nrColors, (PBYTE)os2rgb);
    if(rc != DIVE_SUCCESS) {
        dprintf(("DiveSetDestinationPalette returned %d\n", rc));
        return;
    }
   }
#else
   if(dcaps.ulDepth == 8) {
    OS2SetPhysPalette((HWND)lpDraw->GetWindowHandle(), os2pal);
    rc = DiveSetDestinationPalette(hDive, 0, 0, 0);
    if(rc != DIVE_SUCCESS) {
        dprintf(("DiveSetDestinationPalette returned %d\n", rc));
        return;
    }
  }
  else {
    rc = DiveSetSourcePalette(hDive, 0, nrColors, (PBYTE)os2rgb);
    if(rc != DIVE_SUCCESS) {
        dprintf(("DiveSetSourcePalette returned %d\n", rc));
        return;
    }
  }
#endif
}
//******************************************************************************
//******************************************************************************
void OS2IDirectDrawPalette::RestorePhysPalette()
{
   if(dcaps.ulDepth == 8) {
    OS2ResetPhysPalette();
   }
}
//******************************************************************************
//******************************************************************************

