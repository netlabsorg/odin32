/* $Id: OS2CLIPPER.H,v 1.8 2001-05-28 17:17:12 phaller Exp $ */

/*
 * DX clipper class definition
 *
 * Copyright 1998 Sander va Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2CLIPPER_H__
#define __OS2CLIPPER_H__

#include "divewrap.h"
#define FAR
#undef THIS
#define THIS IDirectDrawClipper*
class OS2IDirectDrawClipper
{
 public:
  //this one has to go first!
  IDirectDrawClipperVtbl *lpVtbl;
  IDirectDrawClipperVtbl  Vtbl;

  OS2IDirectDrawClipper(OS2IDirectDraw *lpDirectDraw);
  OS2IDirectDrawClipper(void);
  ~OS2IDirectDrawClipper();

        int             Referenced;
        inline  HRESULT       GetLastError() { return lastError; };
 private:

 protected:
        HRESULT                lastError;
        OS2IDirectDraw        *lpDraw;
        HDIVE                  hDive;
        HWND                   clipWindow;
        RECT                   windowRect;

                              // Linked list management
              OS2IDirectDrawClipper* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDrawClipper* ddraw;                  // List of OS2IDirectDraw

    friend    HRESULT __stdcall ClipQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   __stdcall ClipAddRef(THIS);
    friend    ULONG   __stdcall ClipRelease(THIS);
    friend    HRESULT __stdcall ClipGetClipList(THIS, LPRECT, LPRGNDATA, LPDWORD);
    friend    HRESULT __stdcall ClipGetHWnd(THIS, HWND FAR *);
    friend    HRESULT __stdcall ClipInitialize(THIS, LPDIRECTDRAW, DWORD);
    friend    HRESULT __stdcall ClipIsClipListChanged(THIS, BOOL FAR *);
    friend    HRESULT __stdcall ClipSetClipList(THIS, LPRGNDATA,DWORD);
    friend    HRESULT __stdcall ClipSetHWnd(THIS, DWORD, HWND );
};

HRESULT __stdcall ClipQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
ULONG   __stdcall ClipAddRef(THIS);
ULONG   __stdcall ClipRelease(THIS);
HRESULT __stdcall ClipGetClipList(THIS, LPRECT, LPRGNDATA, LPDWORD);
HRESULT __stdcall ClipGetHWnd(THIS, HWND FAR *);
HRESULT __stdcall ClipInitialize(THIS, LPDIRECTDRAW, DWORD);
HRESULT __stdcall ClipIsClipListChanged(THIS, BOOL FAR *);
HRESULT __stdcall ClipSetClipList(THIS, LPRGNDATA,DWORD);
HRESULT __stdcall ClipSetHWnd(THIS, DWORD, HWND );

#endif
