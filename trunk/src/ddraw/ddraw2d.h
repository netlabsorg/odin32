/* $Id: ddraw2d.h,v 1.2 2002-12-29 14:11:01 sandervl Exp $ */

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

//testestest
//#define SUPPORT_ALL_DIVE_FOURCCS
//testestest

#define MAX_FOURCC_CODES        4

#define ODINDDRAW_SECTION       "DirectDraw"

// This variable is global, mixing windowed and FS DDraw doesn't
// make much sense	
extern BOOL bUseFSDD;
					
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
    inline  DWORD         GetCooperativeLevel() { return dwCoopMode; };
    inline  HWND          GetClientWindow() { return hwndClient; };

    FOURCC        GetScreenFourCC();
    BOOL          IsFourCCSupported(FOURCC fourcc);
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
    BOOL    bInFullScreen;


   // Linked list management
              OS2IDirectDraw* next;                   // Next OS2IDirectDraw
    static    OS2IDirectDraw* ddraw;                  // List of OS2IDirectDraw

    friend    HRESULT WIN32API DrawQueryInterface(THIS, REFIID riid, LPVOID  * ppvObj);
    friend    ULONG   WIN32API DrawAddRef(THIS);
    friend    ULONG   WIN32API DrawRelease(THIS);
    friend    HRESULT WIN32API DrawCompact(THIS);
    friend    HRESULT WIN32API DrawCreateClipper(THIS, DWORD, LPDIRECTDRAWCLIPPER *, IUnknown  * );
    friend    HRESULT WIN32API DrawCreatePalette(THIS, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown  * );
    friend    HRESULT WIN32API DrawCreateSurface(THIS, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE  *, IUnknown  *);
    friend    HRESULT WIN32API DrawCreateSurface4(THIS, LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE4  *, IUnknown  *);
    friend    HRESULT WIN32API DrawDuplicateSurface(THIS, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE  * );
    friend    HRESULT WIN32API DrawDuplicateSurface4(THIS, LPDIRECTDRAWSURFACE4, LPDIRECTDRAWSURFACE4  * );
    friend    HRESULT WIN32API DrawEnumDisplayModes(THIS, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK );
    friend    HRESULT WIN32API DrawEnumDisplayModes4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 );
    friend    HRESULT WIN32API DrawEnumSurfaces(THIS, DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK );
    friend    HRESULT WIN32API DrawEnumSurfaces4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK2 );
    friend    HRESULT WIN32API DrawFlipToGDISurface(THIS);
    friend    HRESULT WIN32API DrawGetCaps(THIS, LPDDCAPS, LPDDCAPS);
    friend    HRESULT WIN32API DrawGetDisplayMode(THIS, LPDDSURFACEDESC);
    friend    HRESULT WIN32API DrawGetFourCCCodes(THIS, LPDWORD, LPDWORD);
    friend    HRESULT WIN32API DrawGetGDISurface(THIS, LPDIRECTDRAWSURFACE  *);
    friend    HRESULT WIN32API DrawGetGDISurface4(THIS, LPDIRECTDRAWSURFACE4  *);
    friend    HRESULT WIN32API DrawGetMonitorFrequency(THIS, LPDWORD);
    friend    HRESULT WIN32API DrawGetScanLine(THIS, LPDWORD);
    friend    HRESULT WIN32API DrawGetVerticalBlankStatus(THIS, LPBOOL);
    friend    HRESULT WIN32API DrawInitialize(THIS, GUID  *);
    friend    HRESULT WIN32API DrawRestoreDisplayMode(THIS);
    friend    HRESULT WIN32API DrawSetCooperativeLevel(THIS, HWND, DWORD);
    friend    HRESULT WIN32API DrawSetDisplayMode(THIS, DWORD, DWORD,DWORD);
    friend    HRESULT WIN32API DrawSetDisplayMode2(THIS, DWORD, DWORD,DWORD, DWORD, DWORD);
    friend    HRESULT WIN32API DrawWaitForVerticalBlank(THIS, DWORD, HANDLE);
    // V2 Interface
    friend    HRESULT WIN32API DrawGetAvailableVidMem(THIS, LPDDSCAPS, LPDWORD, LPDWORD);
    friend    HRESULT WIN32API DrawGetAvailableVidMem4(THIS, LPDDSCAPS2, LPDWORD, LPDWORD);
    // V4 Interface
    friend    HRESULT WIN32API GetSurfaceFromDC(THIS, HDC, LPDIRECTDRAWSURFACE4 *);
    friend    HRESULT WIN32API RestoreAllSurfaces(THIS) ;
    friend    HRESULT WIN32API TestCooperativeLevel(THIS) ;
    friend    HRESULT WIN32API GetDeviceIdentifier(THIS, LPDDDEVICEIDENTIFIER, DWORD );
    /*** IDirect3D methods ***/
    friend    HRESULT WIN32API D3DQueryInterface(THIS, REFIID riid, LPVOID  * ppvObj);
    friend    ULONG   WIN32API D3DAddRef(THIS);
    friend    ULONG   WIN32API D3DRelease(THIS);
    friend    HRESULT WIN32API D3DInitialize(THIS, REFIID);
    friend    HRESULT WIN32API D3DEnumDevices(THIS, LPD3DENUMDEVICESCALLBACK, LPVOID);
    friend    HRESULT WIN32API D3DCreateLight(THIS, LPDIRECT3DLIGHT*, IUnknown*);
    friend    HRESULT WIN32API D3DCreateMaterial(THIS, LPDIRECT3DMATERIAL*, IUnknown*);
    friend    HRESULT WIN32API D3DCreateViewport(THIS, LPDIRECT3DVIEWPORT*, IUnknown*);
    friend    HRESULT WIN32API D3DFindDevice(THIS, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);

};

#define OFFSET_D3DVTABLE        (LONG)(&((OS2IDirectDraw *)NULL)->lpVtbl3D)
#define DDraw3D_GetThisPtr(a)   (OS2IDirectDraw *)((ULONG)a-OFFSET_D3DVTABLE)

//******************************************************************************
HRESULT WIN32API DrawQueryInterface(THIS, REFIID riid, LPVOID  * ppvObj);
ULONG   WIN32API DrawAddRef(THIS);
ULONG   WIN32API DrawRelease(THIS);
HRESULT WIN32API DrawCompact(THIS);
HRESULT WIN32API DrawCreateClipper(THIS, DWORD, LPDIRECTDRAWCLIPPER *, IUnknown  * );
HRESULT WIN32API DrawCreatePalette(THIS, DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE *, IUnknown  * );
HRESULT WIN32API DrawCreateSurface(THIS, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE  *, IUnknown  *);
HRESULT WIN32API DrawDuplicateSurface(THIS, LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE  * );
HRESULT WIN32API DrawEnumDisplayModes(THIS, DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK );
HRESULT WIN32API DrawEnumSurfaces(THIS, DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK );
HRESULT WIN32API DrawFlipToGDISurface(THIS);
HRESULT WIN32API DrawGetCaps(THIS, LPDDCAPS, LPDDCAPS);
HRESULT WIN32API DrawGetDisplayMode(THIS, LPDDSURFACEDESC);
HRESULT WIN32API DrawGetFourCCCodes(THIS, LPDWORD, LPDWORD);
HRESULT WIN32API DrawGetGDISurface(THIS, LPDIRECTDRAWSURFACE  *);
HRESULT WIN32API DrawGetMonitorFrequency(THIS, LPDWORD);
HRESULT WIN32API DrawGetScanLine(THIS, LPDWORD);
HRESULT WIN32API DrawGetVerticalBlankStatus(THIS, LPBOOL);
HRESULT WIN32API DrawInitialize(THIS, GUID  *);
HRESULT WIN32API DrawRestoreDisplayMode(THIS);
HRESULT WIN32API DrawSetCooperativeLevel(THIS, HWND, DWORD);
HRESULT WIN32API DrawSetDisplayMode(THIS, DWORD, DWORD,DWORD);
HRESULT WIN32API DrawSetDisplayMode2(THIS, DWORD, DWORD,DWORD, DWORD, DWORD);
HRESULT WIN32API DrawWaitForVerticalBlank(THIS, DWORD, HANDLE);
// V2
HRESULT WIN32API DrawGetAvailableVidMem(THIS, LPDDSCAPS, LPDWORD, LPDWORD);
// V4
HRESULT WIN32API DrawGetSurfaceFromDC(THIS, HDC, LPDIRECTDRAWSURFACE4 *);
HRESULT WIN32API DrawRestoreAllSurfaces(THIS) ;
HRESULT WIN32API DrawTestCooperativeLevel(THIS) ;
HRESULT WIN32API DrawGetDeviceIdentifier(THIS, LPDDDEVICEIDENTIFIER, DWORD );
// functions which ifaces are changed in DirectX6 after all MS uses COM where an programmer
// isn't allowd to change a function iface after he published the iface (See MS COM docs!)
HRESULT WIN32API DrawCreateSurface4(THIS, LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE4  *, IUnknown  *);
HRESULT WIN32API DrawDuplicateSurface4(THIS, LPDIRECTDRAWSURFACE4, LPDIRECTDRAWSURFACE4  * );
HRESULT WIN32API DrawEnumDisplayModes4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2 );
HRESULT WIN32API DrawEnumSurfaces4(THIS, DWORD, LPDDSURFACEDESC2, LPVOID,LPDDENUMSURFACESCALLBACK2 );
HRESULT WIN32API DrawGetDisplayMode4(THIS, LPDDSURFACEDESC2);
HRESULT WIN32API DrawGetGDISurface4(THIS, LPDIRECTDRAWSURFACE4  *);
HRESULT WIN32API DrawGetAvailableVidMem4(THIS, LPDDSCAPS2, LPDWORD, LPDWORD);

/*** IDirect3D methods ***/
HRESULT WIN32API D3DQueryInterface(THIS, REFIID riid, LPVOID  * ppvObj);
ULONG   WIN32API D3DAddRef(THIS);
ULONG   WIN32API D3DRelease(THIS);
HRESULT WIN32API D3DInitialize(THIS, REFIID);
HRESULT WIN32API D3DEnumDevices(THIS, LPD3DENUMDEVICESCALLBACK, LPVOID);
HRESULT WIN32API D3DCreateLight(THIS, LPDIRECT3DLIGHT*, IUnknown*);
HRESULT WIN32API D3DCreateMaterial(THIS, LPDIRECT3DMATERIAL*, IUnknown*);
HRESULT WIN32API D3DCreateViewport(THIS, LPDIRECT3DVIEWPORT*, IUnknown*);
HRESULT WIN32API D3DFindDevice(THIS, LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);

#define FOURCC_YUY2  mmioFOURCC( 'Y', 'U', 'Y', '2' )
#define FOURCC_UYVY  mmioFOURCC( 'U', 'Y', 'V', 'Y' )

extern BOOL fHideCursorOnLock;

#endif
