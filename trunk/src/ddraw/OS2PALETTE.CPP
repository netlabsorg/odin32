#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#define INITGUID
#include "os2palette.h"
#define _OS2WIN_H
#define FAR
#include "misc.h"
#include "os2palset.h"
#include <winerror.h>

//******************************************************************************
//******************************************************************************
OS2IDirectDrawPalette::OS2IDirectDrawPalette( OS2IDirectDraw *lpDirectDraw,
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
  lpDraw->Vtbl.AddRef(lpDraw);
  hDive                         = lpDirectDraw->GetDiveInstance();
  dwCaps                        = dwPalFlags;

  dwSize = palsize;
  if(256==dwSize)
    dwCaps |= DDPCAPS_ALLOW256;
  dwCaps &= ~DDPCAPS_VSYNC; // No sync change

  os2pal = (LPPALETTEENTRY)malloc(palsize*sizeof(PALETTEENTRY));
  memcpy((char *)os2pal, (char *)lpColorTable, palsize*sizeof(PALETTEENTRY));
}
//******************************************************************************
//******************************************************************************
OS2IDirectDrawPalette::~OS2IDirectDrawPalette()
{
  if(os2pal)
    free(os2pal);
  lpDraw->Vtbl.Release(lpDraw);
}
//******************************************************************************
//******************************************************************************
HRESULT __stdcall PalQueryInterface(THIS This, REFIID riid, LPVOID FAR * ppvObj)
{
  #ifdef DEBUG
    WriteLog("OS2IDirectDrawPalette::PalQueryInterface\n");
  #endif

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

  #ifdef DEBUG
    WriteLog("OS2IDirectDrawPalette::PalAddRef %d\n", me->Referenced+1);
  #endif

  return (++me->Referenced);
}
//******************************************************************************
//******************************************************************************
ULONG __stdcall PalRelease(THIS This)
{
  OS2IDirectDrawPalette *me = (OS2IDirectDrawPalette *)This;

  #ifdef DEBUG
    WriteLog("OS2IDirectDrawPalette::PalRelease %d\n", me->Referenced-1);
  #endif

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

  #ifdef DEBUG
    WriteLog("OS2IDirectDrawPalette::GetCaps\n");
  #endif

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

  #ifdef DEBUG
    WriteLog("OS2IDirectDrawPalette::PalGetEntries\n");
  #endif

  if( (NULL== lpEntries) ||(0!=dwFlags) ||(dwBase<0) ||((dwBase + dwNumEntries)>me->dwSize) )
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
  #ifdef DEBUG
    WriteLog("OS2IDirectDrawPalette::PalInitialize\n");
  #endif
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

  #ifdef DEBUG
    WriteLog("OS2IDirectDrawPalette::PalSetEntries\n");
  #endif
  if( (NULL== lpNewEntries) ||(0!=dwFlags) ||(dwBase<0) ||((dwBase + dwNumEntries)>me->dwSize) )
    return(DDERR_INVALIDPARAMS);

  memcpy((char *)(me->os2pal + dwBase),
         (char *)lpNewEntries,
         dwNumEntries*sizeof(PALETTEENTRY));

  if(me->fAttachedToPrimary)
    me->SetPhysPalette();

  return(DD_OK);
}
//******************************************************************************
//******************************************************************************
void OS2IDirectDrawPalette::SetPhysPalette()
{
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
  if(fAttachedToPrimary)
    SetPhysPalette();

}
//******************************************************************************
//******************************************************************************

