/*
 * GDI definitions
 *
 * Copyright 1993 Alexandre Julliard
 */

#ifndef __WINE_GDI_H
#define __WINE_GDI_H

#include "windef.h"
#include "wingdi.h"
#include "ldt.h"
#include "path.h"
#include <math.h>
#include "local.h"

  /* GDI objects magic numbers */
#define PEN_MAGIC             0x4f47
#define BRUSH_MAGIC           0x4f48
#define FONT_MAGIC            0x4f49
#define PALETTE_MAGIC         0x4f4a
#define BITMAP_MAGIC          0x4f4b
#define REGION_MAGIC          0x4f4c
#define DC_MAGIC              0x4f4d
#define DISABLED_DC_MAGIC     0x4f4e
#define META_DC_MAGIC         0x4f4f
#define METAFILE_MAGIC        0x4f50
#define METAFILE_DC_MAGIC     0x4f51
#define ENHMETAFILE_MAGIC     0x4f52
#define ENHMETAFILE_DC_MAGIC  0x4f53

#define MAGIC_DONTCARE	      0xffff

typedef struct tagGDIOBJHDR
{
    HANDLE16    hNext;
    WORD        wMagic;
    DWORD       dwCount;
} GDIOBJHDR;


typedef struct tagDeviceCaps
{
    WORD   version;       /*   0: driver version */
    WORD   technology;    /*   2: device technology */
    WORD   horzSize;      /*   4: width of display in mm */
    WORD   vertSize;      /*   6: height of display in mm */
    WORD   horzRes;       /*   8: width of display in pixels */
    WORD   vertRes;       /*  10: width of display in pixels */
    WORD   bitsPixel;     /*  12: bits per pixel */
    WORD   planes;        /*  14: color planes */
    WORD   numBrushes;    /*  16: device-specific brushes */
    WORD   numPens;       /*  18: device-specific pens */
    WORD   numMarkers;    /*  20: device-specific markers */
    WORD   numFonts;      /*  22: device-specific fonts */
    WORD   numColors;     /*  24: size of color table */
    WORD   pdeviceSize;   /*  26: size of PDEVICE structure */
    WORD   curveCaps;     /*  28: curve capabilities */
    WORD   lineCaps;      /*  30: line capabilities */
    WORD   polygonalCaps; /*  32: polygon capabilities */
    WORD   textCaps;      /*  34: text capabilities */
    WORD   clipCaps;      /*  36: clipping capabilities */
    WORD   rasterCaps;    /*  38: raster capabilities */
    WORD   aspectX;       /*  40: relative width of device pixel */
    WORD   aspectY;       /*  42: relative height of device pixel */
    WORD   aspectXY;      /*  44: relative diagonal width of device pixel */
    WORD   pad1[21];      /*  46-86: reserved */
    WORD   logPixelsX;    /*  88: pixels / logical X inch */
    WORD   logPixelsY;    /*  90: pixels / logical Y inch */
    WORD   pad2[6];       /*  92-102: reserved */
    WORD   sizePalette;   /* 104: entries in system palette */
    WORD   numReserved;   /* 106: reserved entries */
    WORD   colorRes;      /* 108: color resolution */    
} DeviceCaps;

typedef struct tagGDI_DRIVER 
{
    BOOL (*pInitialize)(void);
    void (*pFinalize)(void);
} GDI_DRIVER;

extern GDI_DRIVER *GDI_Driver;

  /* Device independent DC information */
typedef struct
{
    int           flags;
    const DeviceCaps *devCaps;

    HRGN16        hClipRgn;     /* Clip region (may be 0) */
    HRGN16        hVisRgn;      /* Visible region (must never be 0) */
    HRGN16        hGCClipRgn;   /* GC clip region (ClipRgn AND VisRgn) */
    HPEN16        hPen;
    HBRUSH16      hBrush;
    HFONT16       hFont;
    HBITMAP16     hBitmap;
    HBITMAP16     hFirstBitmap; /* Bitmap selected at creation of the DC */
    HANDLE16      hDevice;
    HPALETTE16    hPalette;
    
    GdiPath       path;

    WORD          ROPmode;
    WORD          polyFillMode;
    WORD          stretchBltMode;
    WORD          relAbsMode;
    WORD          backgroundMode;
    COLORREF      backgroundColor;
    COLORREF      textColor;
    short         brushOrgX;
    short         brushOrgY;

    WORD          textAlign;         /* Text alignment from SetTextAlign() */
    short         charExtra;         /* Spacing from SetTextCharacterExtra() */
    short         breakTotalExtra;   /* Total extra space for justification */
    short         breakCount;        /* Break char. count */
    short         breakExtra;        /* breakTotalExtra / breakCount */
    short         breakRem;          /* breakTotalExtra % breakCount */

    RECT        totalExtent;
    BYTE          bitsPerPixel;

    INT         MapMode;
    INT         GraphicsMode;      /* Graphics mode */
    INT         DCOrgX;            /* DC origin */
    INT         DCOrgY;
    FARPROC16     lpfnPrint;         /* AbortProc for Printing */
    INT         CursPosX;          /* Current position */
    INT         CursPosY;
    INT         ArcDirection;
    XFORM         xformWorld2Wnd;    /* World-to-window transformation */
    XFORM         xformWorld2Vport;  /* World-to-viewport transformation */
    XFORM         xformVport2World;  /* Inverse of the above transformation */
    BOOL        vport2WorldValid;  /* Is xformVport2World valid? */
} WIN_DC_INFO;

typedef struct tagDC
{
    GDIOBJHDR      header;
    HDC          hSelf;            /* Handle to this DC */
    const struct tagDC_FUNCS *funcs; /* DC function table */
    void          *physDev;          /* Physical device (driver-specific) */
    INT          saveLevel;
    DWORD          dwHookData;
    FARPROC16      hookProc;

    INT          wndOrgX;          /* Window origin */
    INT          wndOrgY;
    INT          wndExtX;          /* Window extent */
    INT          wndExtY;
    INT          vportOrgX;        /* Viewport origin */
    INT          vportOrgY;
    INT          vportExtX;        /* Viewport extent */
    INT          vportExtY;

    WIN_DC_INFO w;
} DC;


  /* LoadOEMResource types */
#define OEM_BITMAP  1
#define OEM_CURSOR  2
#define OEM_ICON    3

  /* DC hook codes */
#define DCHC_INVALIDVISRGN      0x0001
#define DCHC_DELETEDC           0x0002

#define DCHF_INVALIDATEVISRGN   0x0001
#define DCHF_VALIDATEVISRGN     0x0002

  /* DC flags */
#define DC_MEMORY     0x0001   /* It is a memory DC */
#define DC_SAVED      0x0002   /* It is a saved DC */
#define DC_DIRTY      0x0004   /* hVisRgn has to be updated */
#define DC_THUNKHOOK  0x0008   /* DC hook is in the 16-bit code */ 

  /* Last 32 bytes are reserved for stock object handles */
#define GDI_HEAP_SIZE               0xffe0

  /* First handle possible for stock objects (must be >= GDI_HEAP_SIZE) */
#define FIRST_STOCK_HANDLE          GDI_HEAP_SIZE

  /* Stock objects handles */

#define NB_STOCK_OBJECTS          (DEFAULT_GUI_FONT + 1)

#define STOCK_WHITE_BRUSH       ((HBRUSH16)(FIRST_STOCK_HANDLE+WHITE_BRUSH))
#define STOCK_LTGRAY_BRUSH      ((HBRUSH16)(FIRST_STOCK_HANDLE+LTGRAY_BRUSH))
#define STOCK_GRAY_BRUSH        ((HBRUSH16)(FIRST_STOCK_HANDLE+GRAY_BRUSH))
#define STOCK_DKGRAY_BRUSH      ((HBRUSH16)(FIRST_STOCK_HANDLE+DKGRAY_BRUSH))
#define STOCK_BLACK_BRUSH       ((HBRUSH16)(FIRST_STOCK_HANDLE+BLACK_BRUSH))
#define STOCK_NULL_BRUSH        ((HBRUSH16)(FIRST_STOCK_HANDLE+NULL_BRUSH))
#define STOCK_HOLLOW_BRUSH      ((HBRUSH16)(FIRST_STOCK_HANDLE+HOLLOW_BRUSH))
#define STOCK_WHITE_PEN	        ((HPEN16)(FIRST_STOCK_HANDLE+WHITE_PEN))
#define STOCK_BLACK_PEN	        ((HPEN16)(FIRST_STOCK_HANDLE+BLACK_PEN))
#define STOCK_NULL_PEN	        ((HPEN16)(FIRST_STOCK_HANDLE+NULL_PEN))
#define STOCK_OEM_FIXED_FONT    ((HFONT16)(FIRST_STOCK_HANDLE+OEM_FIXED_FONT))
#define STOCK_ANSI_FIXED_FONT   ((HFONT16)(FIRST_STOCK_HANDLE+ANSI_FIXED_FONT))
#define STOCK_ANSI_VAR_FONT     ((HFONT16)(FIRST_STOCK_HANDLE+ANSI_VAR_FONT))
#define STOCK_SYSTEM_FONT       ((HFONT16)(FIRST_STOCK_HANDLE+SYSTEM_FONT))
#define STOCK_DEVICE_DEFAULT_FONT ((HFONT16)(FIRST_STOCK_HANDLE+DEVICE_DEFAULT_FONT))
#define STOCK_DEFAULT_PALETTE   ((HPALETTE16)(FIRST_STOCK_HANDLE+DEFAULT_PALETTE))
#define STOCK_SYSTEM_FIXED_FONT ((HFONT16)(FIRST_STOCK_HANDLE+SYSTEM_FIXED_FONT))
#define STOCK_DEFAULT_GUI_FONT  ((HFONT16)(FIRST_STOCK_HANDLE+DEFAULT_GUI_FONT))

#define FIRST_STOCK_FONT        STOCK_OEM_FIXED_FONT
#define LAST_STOCK_FONT         STOCK_DEFAULT_GUI_FONT

#define LAST_STOCK_HANDLE       ((DWORD)STOCK_DEFAULT_GUI_FONT)

  /* Device <-> logical coords conversion */

/* A floating point version of the POINT structure */
typedef struct tagFLOAT_POINT
{
   FLOAT x, y;
} FLOAT_POINT;


#define XDPTOLP(dc,x) \
    (((x)-(dc)->vportOrgX) * (dc)->wndExtX / (dc)->vportExtX+(dc)->wndOrgX)
#define YDPTOLP(dc,y) \
    (((y)-(dc)->vportOrgY) * (dc)->wndExtY / (dc)->vportExtY+(dc)->wndOrgY)
#define XLPTODP(dc,x) \
    (((x)-(dc)->wndOrgX) * (dc)->vportExtX / (dc)->wndExtX+(dc)->vportOrgX)
#define YLPTODP(dc,y) \
    (((y)-(dc)->wndOrgY) * (dc)->vportExtY / (dc)->wndExtY+(dc)->vportOrgY)

  /* Device <-> logical size conversion */

#define XDSTOLS(dc,x) \
    ((x) * (dc)->wndExtX / (dc)->vportExtX)
#define YDSTOLS(dc,y) \
    ((y) * (dc)->wndExtY / (dc)->vportExtY)
#define XLSTODS(dc,x) \
    ((x) * (dc)->vportExtX / (dc)->wndExtX)
#define YLSTODS(dc,y) \
    ((y) * (dc)->vportExtY / (dc)->wndExtY)

  /* GDI local heap */

extern WORD GDI_HeapSel;

#define GDI_HEAP_ALLOC(size) \
            LOCAL_Alloc( GDI_HeapSel, LMEM_FIXED, (size) )
#define GDI_HEAP_ALLOC_MOVEABLE(size) \
            LOCAL_Alloc( GDI_HeapSel, LMEM_MOVEABLE, (size) )
#define GDI_HEAP_REALLOC(handle,size) \
            LOCAL_ReAlloc( GDI_HeapSel, (handle), (size), LMEM_FIXED )
#define GDI_HEAP_FREE(handle) \
            LOCAL_Free( GDI_HeapSel, (handle) )

#define GDI_HEAP_LOCK(handle) \
            LOCAL_Lock( GDI_HeapSel, (handle) )
#define GDI_HEAP_LOCK_SEGPTR(handle) \
            LOCAL_LockSegptr( GDI_HeapSel, (handle) )
#define GDI_HEAP_UNLOCK(handle) \
    ((((HGDIOBJ16)(handle) >= FIRST_STOCK_HANDLE) && \
      ((HGDIOBJ16)(handle)<=LAST_STOCK_HANDLE)) ? \
      0 : LOCAL_Unlock( GDI_HeapSel, (handle) ))

extern GDIOBJHDR * GDI_GetObjPtr( HGDIOBJ16, WORD );

VOID   GDI_ReleaseObj(HANDLE hObject);
BOOL   GDI_FreeObject(HANDLE hObject, LPVOID lpObjectData);
LPVOID GDI_AllocObject( int size, DWORD ulType, HANDLE *phObject );

#endif  /* __WINE_GDI_H */
