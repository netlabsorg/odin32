/*
 * DirectDraw Palette class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2PALETTE_H__
#define __OS2PALETTE_H__

#include "os2ddraw.h"

/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectDrawPalette

class OS2IDirectDrawPalette
{
 public:
    //this one has to go first!
    IDirectDrawPaletteVtbl *lpVtbl;
    IDirectDrawPaletteVtbl  Vtbl;

    OS2IDirectDrawPalette(OS2IDirectDraw *lpDirectDraw, int palsize, W32_LPPALETTEENTRY lpColorTable);
    ~OS2IDirectDrawPalette();

    void          SetPhysPalette();
    void          RestorePhysPalette();
    int           Referenced;
    inline  HRESULT       GetLastError() { return lastError; };
 private:

 protected:
    HDIVE               hDive;
    ULONG               nrColors;
    HRESULT             lastError;
    W32_LPPALETTEENTRY  os2pal;
    OS2IDirectDraw     *lpDraw;

                              // Linked list management
              OS2IDirectDrawPalette* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDrawPalette* ddraw;                  // List of OS2IDirectDraw

    friend    HRESULT __stdcall PalQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   __stdcall PalAddRef(THIS);
    friend    ULONG   __stdcall PalRelease(THIS);
    friend    HRESULT __stdcall PalGetCaps(THIS_ LPDWORD);
    friend    HRESULT __stdcall PalGetEntries(THIS_ DWORD,DWORD,DWORD,W32_LPPALETTEENTRY);
    friend    HRESULT __stdcall PalInitialize(THIS_ LPDIRECTDRAW, DWORD,W32_LPPALETTEENTRY);
    friend    HRESULT __stdcall PalSetEntries(THIS_ DWORD,DWORD,DWORD,W32_LPPALETTEENTRY);
};


HRESULT __stdcall PalQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
ULONG   __stdcall PalAddRef(THIS);
ULONG   __stdcall PalRelease(THIS);
HRESULT __stdcall PalGetCaps(THIS_ LPDWORD);
HRESULT __stdcall PalGetEntries(THIS_ DWORD,DWORD,DWORD,W32_LPPALETTEENTRY);
HRESULT __stdcall PalInitialize(THIS_ LPDIRECTDRAW, DWORD,W32_LPPALETTEENTRY);
HRESULT __stdcall PalSetEntries(THIS_ DWORD,DWORD,DWORD,W32_LPPALETTEENTRY);


#endif
