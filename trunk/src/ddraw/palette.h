/* $Id: palette.h,v 1.2 2002-12-29 14:11:02 sandervl Exp $ */

/*
 * DX palette class definition
 *
 * Copyright 1998 Sander va Leeuwen
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __OS2PALETTE_H__
#define __OS2PALETTE_H__

#include "divewrap.h"
#define FAR
#undef THIS
#define THIS IDirectDrawPalette*

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

class OS2IDirectDrawPalette
{
 public:
  //this one has to go first!
  IDirectDrawPaletteVtbl *lpVtbl;
  IDirectDrawPaletteVtbl  Vtbl;

  OS2IDirectDrawPalette( VOID *lpDirectDraw,
                         int palsize,
                         LPPALETTEENTRY lpColorTable,
                         DWORD dwPalFlags);
  ~OS2IDirectDrawPalette();

  void          SetPhysPalette();
  void          RestorePhysPalette();
  void          SetIsPrimary(BOOL newValue);
  int           Referenced;
  inline  HRESULT       GetLastError() { return lastError; };
  WORD           aPal16[256];
  DWORD          aPal24[256];
 private:

 protected:
  HDIVE          hDive;
  HDIVE          hDiveCC;
  HRESULT        lastError;
  LPPALETTEENTRY os2pal;
  VOID           *lpDraw;
  DWORD          dwCaps;
  DWORD          dwSize;
  BOOL           fAttachedToPrimary;
  friend    HRESULT WIN32API PalQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
  friend    ULONG   WIN32API PalAddRef(THIS);
  friend    ULONG   WIN32API PalRelease(THIS);
  friend    HRESULT WIN32API PalGetCaps(THIS, LPDWORD);
  friend    HRESULT WIN32API PalGetEntries(THIS, DWORD,DWORD,DWORD,LPPALETTEENTRY);
  friend    HRESULT WIN32API PalInitialize(THIS, LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
  friend    HRESULT WIN32API PalSetEntries(THIS, DWORD,DWORD,DWORD,LPPALETTEENTRY);
};


HRESULT WIN32API PalQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
ULONG   WIN32API PalAddRef(THIS);
ULONG   WIN32API PalRelease(THIS);
HRESULT WIN32API PalGetCaps(THIS, LPDWORD);
HRESULT WIN32API PalGetEntries(THIS, DWORD,DWORD,DWORD,LPPALETTEENTRY);
HRESULT WIN32API PalInitialize(THIS, LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
HRESULT WIN32API PalSetEntries(THIS, DWORD,DWORD,DWORD,LPPALETTEENTRY);


#endif
