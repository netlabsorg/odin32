/* $Id: OS2DDRAW.H,v 1.1 1999-05-24 20:19:36 ktk Exp $ */

/*
 * DirectDraw main class
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OS2DDRAW_HPP__
#define __OS2DDRAW_HPP__


/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectDraw2

class OS2IDirectDraw
{
 public:
    //this one has to go first!
    IDirectDraw2Vtbl *lpVtbl;
    IDirectDraw2Vtbl  Vtbl;
    IDirect3DVtbl     Vtbl3D;

    OS2IDirectDraw(GUID *lpGUID);
    ~OS2IDirectDraw();

            int           Referenced;
    inline  HRESULT       GetLastError()    { return lastError;    };
    inline  HDIVE         GetDiveInstance() { return hDive;        };
    inline  char *        GetFrameBuffer()  { return pFrameBuffer; };
    inline  int           GetScreenWidth()  { return screenwidth;  };
    inline  int           GetScreenHeight() { return screenheight; };
    inline  BOOL          IsFullScreen()    { return fFullScreen;  };
    inline  W32_HWND      GetWindowHandle() { return hwndClient;   };

 private:

 protected:
          HRESULT   lastError;
          HDIVE     hDive;
          char     *pFrameBuffer;
          W32_HWND  hwndClient;
          int       screenwidth, screenheight, screenbpp;

          BOOL      fFullScreen;

              // Linked list management
              OS2IDirectDraw* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDraw* ddraw;                  // List of OS2IDirectDraw

    friend    HRESULT __stdcall DrawQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   __stdcall DrawAddRef(THIS);
    friend    ULONG   __stdcall DrawRelease(THIS);
    friend    HRESULT __stdcall DrawCompact(THIS);
    friend    HRESULT __stdcall DrawCreateClipper(THIS, DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR * );
    friend    HRESULT __stdcall DrawCreatePalette(THIS, DWORD, W32_LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR * );
    friend    HRESULT __stdcall DrawCreateSurface(THIS, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR *, IUnknown FAR *);
    friend    HRESULT __stdcall DrawDuplicateSurface(THIS, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * );
    friend    HRESULT __stdcall DrawEnumDisplayModes(THIS, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK );
    friend    HRESULT __stdcall DrawEnumSurfaces(THIS, DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK );
    friend    HRESULT __stdcall DrawFlipToGDISurface(THIS);
    friend    HRESULT __stdcall DrawGetCaps(THIS, LPDDCAPS, LPDDCAPS);
    friend    HRESULT __stdcall DrawGetDisplayMode(THIS, LPDDSURFACEDESC);
    friend    HRESULT __stdcall DrawGetFourCCCodes(THIS, LPDWORD, LPDWORD);
    friend    HRESULT __stdcall DrawGetGDISurface(THIS, LPDIRECTDRAWSURFACE FAR *);
    friend    HRESULT __stdcall DrawGetMonitorFrequency(THIS, LPDWORD);
    friend    HRESULT __stdcall DrawGetScanLine(THIS, LPDWORD);
    friend    HRESULT __stdcall DrawGetVerticalBlankStatus(THIS, LPBOOL);
    friend    HRESULT __stdcall DrawInitialize(THIS, GUID FAR *);
    friend    HRESULT __stdcall DrawRestoreDisplayMode(THIS);
    friend    HRESULT __stdcall DrawSetCooperativeLevel(THIS, W32_HWND, DWORD);
    friend    HRESULT __stdcall DrawSetDisplayMode(THIS, DWORD, DWORD,DWORD);
    friend    HRESULT __stdcall DrawSetDisplayMode2(THIS, DWORD, DWORD,DWORD, DWORD, DWORD);
    friend    HRESULT __stdcall DrawWaitForVerticalBlank(THIS, DWORD, W32_HANDLE);
    friend    HRESULT __stdcall DrawGetAvailableVidMem(THIS, LPDDSCAPS, LPDWORD, LPDWORD);

    /*** IDirect3D methods ***/
    /* KSO Apr 19 1999: Set correct interface.           *
     * (INTERFACE is used in the THIS and THIS_ macros)  */
    #undef  INTERFACE
    #define INTERFACE   IDirect3D
    friend    HRESULT __stdcall D3DQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
    friend    ULONG   __stdcall D3DAddRef(THIS);
    friend    ULONG   __stdcall D3DRelease(THIS);
    friend    HRESULT __stdcall D3DInitialize(THIS, REFIID);
    friend    HRESULT __stdcall D3DEnumDevices(THIS, LPD3DENUMDEVICESCALLBACK, LPVOID);
    friend    HRESULT __stdcall D3DCreateLight(THIS, LPDIRECT3DLIGHT*, IUnknown*);
    friend    HRESULT __stdcall D3DCreateMaterial(THIS, LPDIRECT3DMATERIAL*, IUnknown*);
    friend    HRESULT __stdcall D3DCreateViewport(THIS, LPDIRECT3DVIEWPORT*, IUnknown*);
    friend    HRESULT __stdcall D3DFindDevice(THIS, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);
};


//******************************************************************************
/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirectDraw2
HRESULT __stdcall DrawQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
ULONG   __stdcall DrawAddRef(THIS);
ULONG   __stdcall DrawRelease(THIS);
HRESULT __stdcall DrawCompact(THIS);
HRESULT __stdcall DrawCreateClipper(THIS, DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR * );
HRESULT __stdcall DrawCreatePalette(THIS, DWORD, W32_LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR * );
HRESULT __stdcall DrawCreateSurface(THIS, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR *, IUnknown FAR *);
HRESULT __stdcall DrawDuplicateSurface(THIS, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * );
HRESULT __stdcall DrawEnumDisplayModes(THIS, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK );
HRESULT __stdcall DrawEnumSurfaces(THIS, DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK );
HRESULT __stdcall DrawFlipToGDISurface(THIS);
HRESULT __stdcall DrawGetCaps(THIS, LPDDCAPS, LPDDCAPS);
HRESULT __stdcall DrawGetDisplayMode(THIS, LPDDSURFACEDESC);
HRESULT __stdcall DrawGetFourCCCodes(THIS, LPDWORD, LPDWORD);
HRESULT __stdcall DrawGetGDISurface(THIS, LPDIRECTDRAWSURFACE FAR *);
HRESULT __stdcall DrawGetMonitorFrequency(THIS, LPDWORD);
HRESULT __stdcall DrawGetScanLine(THIS, LPDWORD);
HRESULT __stdcall DrawGetVerticalBlankStatus(THIS, LPBOOL);
HRESULT __stdcall DrawInitialize(THIS, GUID FAR *);
HRESULT __stdcall DrawRestoreDisplayMode(THIS);
HRESULT __stdcall DrawSetCooperativeLevel(THIS, W32_HWND, DWORD);
HRESULT __stdcall DrawSetDisplayMode(THIS, DWORD, DWORD,DWORD);
HRESULT __stdcall DrawSetDisplayMode2(THIS, DWORD, DWORD,DWORD, DWORD, DWORD);
HRESULT __stdcall DrawWaitForVerticalBlank(THIS, DWORD, W32_HANDLE);
HRESULT __stdcall DrawGetAvailableVidMem(THIS, LPDDSCAPS, LPDWORD, LPDWORD);

/*** IDirect3D methods ***/
/* KSO Apr 19 1999: Set correct interface.           *
 * (INTERFACE is used in the THIS and THIS_ macros)  */
#undef  INTERFACE
#define INTERFACE   IDirect3D
HRESULT __stdcall D3DQueryInterface(THIS, REFIID riid, LPVOID FAR * ppvObj);
ULONG   __stdcall D3DAddRef(THIS);
ULONG   __stdcall D3DRelease(THIS);
HRESULT __stdcall D3DInitialize(THIS_ REFIID);
HRESULT __stdcall D3DEnumDevices(THIS_ LPD3DENUMDEVICESCALLBACK, LPVOID);
HRESULT __stdcall D3DCreateLight(THIS_ LPDIRECT3DLIGHT*, IUnknown*);
HRESULT __stdcall D3DCreateMaterial(THIS_ LPDIRECT3DMATERIAL*, IUnknown*);
HRESULT __stdcall D3DCreateViewport(THIS_ LPDIRECT3DVIEWPORT*, IUnknown*);
HRESULT __stdcall D3DFindDevice(THIS_ LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);

#endif