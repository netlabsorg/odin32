/*
 * DirectDraw Clipper Class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2CLIPPER_H__
#define __OS2CLIPPER_H__

/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectDrawClipper

class OS2IDirectDrawClipper
{
 public:
    //this one has to go first!
    IDirectDrawClipperVtbl *lpVtbl;
    IDirectDrawClipperVtbl  Vtbl;

    OS2IDirectDrawClipper(OS2IDirectDraw *lpDirectDraw);
       ~OS2IDirectDrawClipper();

          int             Referenced;
        inline  HRESULT       GetLastError() { return lastError; };
 private:

 protected:
          HRESULT                lastError;
          OS2IDirectDraw        *lpDraw;
          HDIVE                  hDive;
          W32_HWND               clipWindow;

                              // Linked list management
              OS2IDirectDrawClipper* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDrawClipper* ddraw;                  // List of OS2IDirectDraw

    friend    HRESULT __stdcall ClipQueryInterface(THIS_ REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   __stdcall ClipAddRef(THIS);
    friend    ULONG   __stdcall ClipRelease(THIS);
    friend    HRESULT __stdcall ClipGetClipList(THIS_ W32_LPRECT, W32_LPRGNDATA, LPDWORD);
    friend    HRESULT __stdcall ClipGetHWnd(THIS_ W32_HWND FAR *);
    friend    HRESULT __stdcall ClipInitialize(THIS_ LPDIRECTDRAW, DWORD);
    friend    HRESULT __stdcall ClipIsClipListChanged(THIS_ BOOL FAR *);
    friend    HRESULT __stdcall ClipSetClipList(THIS_ W32_LPRGNDATA,DWORD);
    friend    HRESULT __stdcall ClipSetHWnd(THIS_ DWORD, W32_HWND );
};

HRESULT __stdcall ClipQueryInterface(THIS_ REFIID riid, LPVOID FAR * ppvObj);
ULONG   __stdcall ClipAddRef(THIS);
ULONG   __stdcall ClipRelease(THIS);
HRESULT __stdcall ClipGetClipList(THIS_ W32_LPRECT, W32_LPRGNDATA, LPDWORD);
HRESULT __stdcall ClipGetHWnd(THIS_ W32_HWND FAR *);
HRESULT __stdcall ClipInitialize(THIS_ LPDIRECTDRAW, DWORD);
HRESULT __stdcall ClipIsClipListChanged(THIS_ BOOL FAR *);
HRESULT __stdcall ClipSetClipList(THIS_ W32_LPRGNDATA,DWORD);
HRESULT __stdcall ClipSetHWnd(THIS_ DWORD, W32_HWND );

#endif
