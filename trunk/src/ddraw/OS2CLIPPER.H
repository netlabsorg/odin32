/* $Id: OS2CLIPPER.H,v 1.10 2001-10-03 13:49:39 sandervl Exp $ */

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

        LPRGNDATA              lpRgnData;
        BOOL                   fClipListChanged;
        BOOL                   fDrawingAllowed;

                              // Linked list management
              OS2IDirectDrawClipper* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDrawClipper* ddraw;                  // List of OS2IDirectDraw

    friend    HRESULT WIN32API ClipQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   WIN32API ClipAddRef(THIS);
    friend    ULONG   WIN32API ClipRelease(THIS);
    friend    HRESULT WIN32API ClipGetClipList(THIS, LPRECT, LPRGNDATA, LPDWORD);
    friend    HRESULT WIN32API ClipGetHWnd(THIS, HWND FAR *);
    friend    HRESULT WIN32API ClipInitialize(THIS, LPDIRECTDRAW, DWORD);
    friend    HRESULT WIN32API ClipIsClipListChanged(THIS, BOOL FAR *);
    friend    HRESULT WIN32API ClipSetClipList(THIS, LPRGNDATA,DWORD);
    friend    HRESULT WIN32API ClipSetHWnd(THIS, DWORD, HWND );
    friend    BOOL    WIN32API ClipVisRgnCallback(HWND hwnd, BOOL fDrawingAllowed, DWORD dwUserData);

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
