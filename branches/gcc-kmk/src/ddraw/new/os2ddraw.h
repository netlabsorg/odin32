/* $Id: OS2DDRAW.H,v 1.2 2001-03-09 22:48:48 mike Exp $ */

/*
 * DX Draw base class definition
 *
 * Copyright 1998 Sander va Leeuwen
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __OS2DDRAW_HPP__
  #define __OS2DDRAW_HPP__

#include <ddraw.h>
#include <d3d.h>
#undef APIENTRY
#define APIENTRY _System
#include "divewrap.h"
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                         \
                  ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                  ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#include <fourcc.h>
#undef THIS
#define THIS VOID*

#define MAX_FOURCC_CODES        3

class OS2IDirectDraw
{
  public:
    //this one has to go first!
    IDirectDraw4Vtbl *lpVtbl;
    IDirectDraw4Vtbl *lpVtbl2;         // 2nd Vtable pointer An other MS assumtion see below
    DWORD            *pdwUnknownData;  // MS DDCAPS (DX6 SDK bin\tools )program makes an assumtion that after the
                                       // VTable comes a pointer to some data
                                       // to avoid crashes we do it a favor and alloc a buffer
                                       // with 255 DWORDS
                                       // Known data in the buffer Height(offset 235),
                                       // Height (236),  Bits (241)
                                       // ToDO :
                                       // debug into DirectDrawCreate and se what MS is doing !
    //Use for Direct3D interface (This != this)
    IDirect3DVtbl    *lpVtbl3D;
    IDirect3DVtbl    *lpVtbl3D2;
    DWORD            *pdwUnknownData3D;

    IDirectDrawVtbl  Vtbl;
    IDirectDraw2Vtbl  Vtbl2;
    // MS did it again with direct/X 6 they changed the behavior of the directdraw component
    // So we now need 2 Virt. Tables one for the old and one for the new so we return the
    // right interface if one creates a DX6 directdraw object but wants a DX2,3,5 interface
    IDirectDraw4Vtbl  Vtbl4;
    IDirect3DVtbl     Vtbl3D;

    OS2IDirectDraw(GUID *lpGUID);
    ~OS2IDirectDraw();
    // Simple management for Surfaces should be good enougth for now
    inline  BOOL          HasPrimarySurface() { return PrimaryExists;}
            void          SetPrimarySurface(BOOL NewStatus){PrimaryExists = NewStatus;}
            int           Referenced;
    inline  HRESULT       GetLastError()    { return lastError;    }
    inline  HDIVE         GetDiveInstance() { return hDive;        }
    inline  HDIVE         GetCCDiveInstance() { return hDiveColorConv;}
    inline  char *        GetFrameBuffer()  { return pFrameBuffer; }
    inline  int           GetScreenWidth()  { return screenwidth;  }
    inline  int           GetScreenHeight() { return screenheight; }
    inline  int           GetScreenBpp()    { return screenbpp;}
    FOURCC        GetScreenFourCC();
    // We should be able to use any mode with less or same bits and same or
    // smaller size to be able to report all supported mode in the enum
    // functions we need the physical screen size. Other information is later
    // also needed so get all the info we can get
    DIVE_CAPS dCaps;
    VOID *pPrimSurf;

  private:
   VOID SwitchDisplay(HWND hwnd);


  protected:

    HRESULT lastError;
    HDIVE   hDive;           // Handle to Screen DIVE instance
    HDIVE   hDiveColorConv;  // Handle to Offscreen DIVE instance for Color conversion
    char   *pFrameBuffer;
    HWND    hwndClient;
    DWORD   dwCoopMode;
    int     screenwidth, screenheight, screenbpp;
    BOOL    PrimaryExists;
    BOOL    bScale;
    int     oldwidth, oldheight, oldbpp, oldscanlines;
    BOOL    bPMILoaded;
    BOOL    bInFullScreen;


   // Linked list management
              OS2IDirectDraw* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDraw* ddraw;                  // List of OS2IDirectDraw

    friend    HRESULT __stdcall DrawQueryInterface(THIS, REFIID riid, LPVOID  * ppvObj);
    friend    ULONG   __stdcall DrawAddRef(THIS);
    friend    ULONG   __stdcall DrawRelease(THIS);
    friend    HRESULT __stdcall DrawCompact(THIS);
    friend    HRESULT __stdcall DrawCreateClipper(THIS, DWORD, LPDIRECTDRAWCLIPPER *, IUnknown  * );
    friend    HRESULT __stdcall DrawCreatePalette(THIS, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown  * );
    friend    HRESULT __stdcall DrawCreateSurface(THIS, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE  *, IUnknown  *);
    friend    HRESULT __stdcall DrawCreateSurface4(THIS, LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE4  *, IUnknown  *);
    friend    HRESULT __stdcall DrawDuplicateSurface(THIS, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE  * );
    friend    HRESULT __stdcall DrawDuplicateSurface4(THIS, LPDIRECTDRAWSURFACE4, LPDIRECTDRAWSURFACE4  * );
    friend    HRESULT __stdcall DrawEnumDisplayModes(THIS, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK );
    friend    HRESULT __stdcall DrawEnumDisplayModes4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 );
    friend    HRESULT __stdcall DrawEnumSurfaces(THIS, DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK );
    friend    HRESULT __stdcall DrawEnumSurfaces4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK2 );
    friend    HRESULT __stdcall DrawFlipToGDISurface(THIS);
    friend    HRESULT __stdcall DrawGetCaps(THIS, LPDDCAPS, LPDDCAPS);
    friend    HRESULT __stdcall DrawGetDisplayMode(THIS, LPDDSURFACEDESC);
    friend    HRESULT __stdcall DrawGetFourCCCodes(THIS, LPDWORD, LPDWORD);
    friend    HRESULT __stdcall DrawGetGDISurface(THIS, LPDIRECTDRAWSURFACE  *);
    friend    HRESULT __stdcall DrawGetGDISurface4(THIS, LPDIRECTDRAWSURFACE4  *);
    friend    HRESULT __stdcall DrawGetMonitorFrequency(THIS, LPDWORD);
    friend    HRESULT __stdcall DrawGetScanLine(THIS, LPDWORD);
    friend    HRESULT __stdcall DrawGetVerticalBlankStatus(THIS, LPBOOL);
    friend    HRESULT __stdcall DrawInitialize(THIS, GUID  *);
    friend    HRESULT __stdcall DrawRestoreDisplayMode(THIS);
    friend    HRESULT __stdcall DrawSetCooperativeLevel(THIS, HWND, DWORD);
    friend    HRESULT __stdcall DrawSetDisplayMode(THIS, DWORD, DWORD,DWORD);
    friend    HRESULT __stdcall DrawSetDisplayMode2(THIS, DWORD, DWORD,DWORD, DWORD, DWORD);
    friend    HRESULT __stdcall DrawWaitForVerticalBlank(THIS, DWORD, HANDLE);
    // V2 Interface
    friend    HRESULT __stdcall DrawGetAvailableVidMem(THIS, LPDDSCAPS, LPDWORD, LPDWORD);
    friend    HRESULT __stdcall DrawGetAvailableVidMem4(THIS, LPDDSCAPS2, LPDWORD, LPDWORD);
    // V4 Interface
    friend    HRESULT __stdcall GetSurfaceFromDC(THIS, HDC, LPDIRECTDRAWSURFACE4 *);
    friend    HRESULT __stdcall RestoreAllSurfaces(THIS) ;
    friend    HRESULT __stdcall TestCooperativeLevel(THIS) ;
    friend    HRESULT __stdcall GetDeviceIdentifier(THIS, LPDDDEVICEIDENTIFIER, DWORD );
    /*** IDirect3D methods ***/
    friend    HRESULT __stdcall D3DQueryInterface(THIS, REFIID riid, LPVOID  * ppvObj);
    friend    ULONG   __stdcall D3DAddRef(THIS);
    friend    ULONG   __stdcall D3DRelease(THIS);
    friend    HRESULT __stdcall D3DInitialize(THIS, REFIID);
    friend    HRESULT __stdcall D3DEnumDevices(THIS, LPD3DENUMDEVICESCALLBACK, LPVOID);
    friend    HRESULT __stdcall D3DCreateLight(THIS, LPDIRECT3DLIGHT*, IUnknown*);
    friend    HRESULT __stdcall D3DCreateMaterial(THIS, LPDIRECT3DMATERIAL*, IUnknown*);
    friend    HRESULT __stdcall D3DCreateViewport(THIS, LPDIRECT3DVIEWPORT*, IUnknown*);
    friend    HRESULT __stdcall D3DFindDevice(THIS, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);

};

#define OFFSET_D3DVTABLE        (LONG)(&((OS2IDirectDraw *)NULL)->lpVtbl3D)
#define DDraw3D_GetThisPtr(a)   (OS2IDirectDraw *)((ULONG)a-OFFSET_D3DVTABLE)

//******************************************************************************
HRESULT __stdcall DrawQueryInterface(THIS, REFIID riid, LPVOID  * ppvObj);
ULONG   __stdcall DrawAddRef(THIS);
ULONG   __stdcall DrawRelease(THIS);
HRESULT __stdcall DrawCompact(THIS);
HRESULT __stdcall DrawCreateClipper(THIS, DWORD, LPDIRECTDRAWCLIPPER *, IUnknown  * );
HRESULT __stdcall DrawCreatePalette(THIS, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown  * );
HRESULT __stdcall DrawCreateSurface(THIS, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE  *, IUnknown  *);
HRESULT __stdcall DrawDuplicateSurface(THIS, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE  * );
HRESULT __stdcall DrawEnumDisplayModes(THIS, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK );
HRESULT __stdcall DrawEnumSurfaces(THIS, DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK );
HRESULT __stdcall DrawFlipToGDISurface(THIS);
HRESULT __stdcall DrawGetCaps(THIS, LPDDCAPS, LPDDCAPS);
HRESULT __stdcall DrawGetDisplayMode(THIS, LPDDSURFACEDESC);
HRESULT __stdcall DrawGetFourCCCodes(THIS, LPDWORD, LPDWORD);
HRESULT __stdcall DrawGetGDISurface(THIS, LPDIRECTDRAWSURFACE  *);
HRESULT __stdcall DrawGetMonitorFrequency(THIS, LPDWORD);
HRESULT __stdcall DrawGetScanLine(THIS, LPDWORD);
HRESULT __stdcall DrawGetVerticalBlankStatus(THIS, LPBOOL);
HRESULT __stdcall DrawInitialize(THIS, GUID  *);
HRESULT __stdcall DrawRestoreDisplayMode(THIS);
HRESULT __stdcall DrawSetCooperativeLevel(THIS, HWND, DWORD);
HRESULT __stdcall DrawSetDisplayMode(THIS, DWORD, DWORD,DWORD);
HRESULT __stdcall DrawSetDisplayMode2(THIS, DWORD, DWORD,DWORD, DWORD, DWORD);
HRESULT __stdcall DrawWaitForVerticalBlank(THIS, DWORD, HANDLE);
// V2
HRESULT __stdcall DrawGetAvailableVidMem(THIS, LPDDSCAPS, LPDWORD, LPDWORD);
// V4
HRESULT __stdcall DrawGetSurfaceFromDC(THIS, HDC, LPDIRECTDRAWSURFACE4 *);
HRESULT __stdcall DrawRestoreAllSurfaces(THIS) ;
HRESULT __stdcall DrawTestCooperativeLevel(THIS) ;
HRESULT __stdcall DrawGetDeviceIdentifier(THIS, LPDDDEVICEIDENTIFIER, DWORD );
// functions which ifaces are changed in DirectX6 after all MS uses COM where an programmer
// isn't allowd to change a function iface after he published the iface (See MS COM docs!)
HRESULT __stdcall DrawCreateSurface4(THIS, LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE4  *, IUnknown  *);
HRESULT __stdcall DrawDuplicateSurface4(THIS, LPDIRECTDRAWSURFACE4, LPDIRECTDRAWSURFACE4  * );
HRESULT __stdcall DrawEnumDisplayModes4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 );
HRESULT __stdcall DrawEnumSurfaces4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK2 );
HRESULT __stdcall DrawGetDisplayMode4(THIS, LPDDSURFACEDESC2);
HRESULT __stdcall DrawGetGDISurface4(THIS, LPDIRECTDRAWSURFACE4  *);
HRESULT __stdcall DrawGetAvailableVidMem4(THIS, LPDDSCAPS2, LPDWORD, LPDWORD);

/*** IDirect3D methods ***/
HRESULT __stdcall D3DQueryInterface(THIS, REFIID riid, LPVOID  * ppvObj);
ULONG   __stdcall D3DAddRef(THIS);
ULONG   __stdcall D3DRelease(THIS);
HRESULT __stdcall D3DInitialize(THIS, REFIID);
HRESULT __stdcall D3DEnumDevices(THIS, LPD3DENUMDEVICESCALLBACK, LPVOID);
HRESULT __stdcall D3DCreateLight(THIS, LPDIRECT3DLIGHT*, IUnknown*);
HRESULT __stdcall D3DCreateMaterial(THIS, LPDIRECT3DMATERIAL*, IUnknown*);
HRESULT __stdcall D3DCreateViewport(THIS, LPDIRECT3DVIEWPORT*, IUnknown*);
HRESULT __stdcall D3DFindDevice(THIS, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);

#define FOURCC_YUY2  mmioFOURCC( 'Y', 'U', 'Y', '2' )
#define FOURCC_UYVY  mmioFOURCC( 'U', 'Y', 'V', 'Y' )

#endif
