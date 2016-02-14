/*
 * GDI32: public dc data definitions
 *
 *
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __DCDATA_H__
#define __DCDATA_H__

//Flags for Reserved DWORD in DCData structure
#define DC_FLAG_SEND_POSTSCRIPT_SETUP_STRING	1
#define DC_FLAG_POLYGON_PRINTER_HACK		2

typedef enum PS_Type { MICRO_CACHED, MICRO, NORMAL } PS_Type;
typedef enum HDC_Type{ TYPE_0, TYPE_1, TYPE_2, TYPE_3, TYPE_4 } HDC_Type;

typedef struct _tagXFORM
{
    FLOAT eM11;
    FLOAT eM12;
    FLOAT eM21;
    FLOAT eM22;
    FLOAT eDx;
    FLOAT eDy;
} XFORM_W, *PXFORM_W, *LPXFORM_W;

#ifndef INCL_GPI
   typedef struct _SIZEF
   {
      FIXED cx;
      FIXED cy;
   } SIZEF;
   typedef SIZEF *PSIZEF;

#ifndef OS2DEF_INCLUDED
   typedef struct _MATRIXLF
   {
      FIXED fxM11;
      FIXED fxM12;
      LONG  lM13;
      FIXED fxM21;
      FIXED fxM22;
      LONG  lM23;
      LONG  lM31;
      LONG  lM32;
      LONG  lM33;
   } MATRIXLF;
   typedef MATRIXLF *PMATRIXLF;

   typedef ULONG   HPS;
#endif

   typedef struct _CHARBUNDLE
   {
      LONG      lColor;
      LONG      lBackColor;
      USHORT    usMixMode;
      USHORT    usBackMixMode;
      USHORT    usSet;
      USHORT    usPrecision;
      SIZEF     sizfxCell;
      POINTL    ptlAngle;
      POINTL    ptlShear;
      USHORT    usDirection;
      USHORT    usTextAlign;
      FIXED     fxExtra;
      FIXED     fxBreakExtra;
   } CHARBUNDLE;
   typedef CHARBUNDLE *PCHARBUNDLE;

#endif

#pragma pack(4)
typedef struct _DCData
{
   HDC         hdc;
   HWND        hwnd;
   HPS         hps;

   UINT        lastBrushKey;
   PVOID       lastBrushObject;
   UINT        lastPenKey;
   struct _penobject *lastPenObject;
   UINT        lastFontKey;
   PVOID       lastFontObject;
   UINT        lastBitmapKey;
   PVOID       lastBitmapObject;
   UINT        lastPaletteKey;
   PVOID       lastPaletteObject;

   UINT        nullBitmapHandle;

   COLORREF    BkColor;
   COLORREF    TextColor;
   ULONG       BkColor_PM;
   ULONG       TextColor_PM;

   int         BkMode;
   ULONG       BkMode_PM;

   int         ROP2Mode;
   int         ROP2Mode_PM;

   unsigned    isMemoryPS:1;
   unsigned    isMetaPS:1;
   unsigned    isPrinter:1;
   unsigned    isFrameWindow:1;
   unsigned    isOD_QUEUED:1;
   unsigned    isOD_INFO:1;
   unsigned    isClient:1;
   unsigned    isClientArea:1;
   unsigned    isLeftLeft:1;
   unsigned    isTopTop:1;
   unsigned    isWideLine:1;
   unsigned    alignUpdateCP:1;
   unsigned    isCacheable:1;
   unsigned    penIsExtPen:1;
   unsigned    isValid:1;
   unsigned    inPath:1;
   unsigned    isStartDoc:1;
   unsigned    resetStockFonts:1;
#ifdef CUSTOM_CURPOS
   unsigned    curPosSet:1;
#endif
   unsigned    bAttrSet:1;
   unsigned    bFirstSet:1;
   unsigned    unused:11;

   ULONG       MapMode;
   HBITMAP     bitmapHandle;
   ULONG       bitmapHeight;
   ULONG       bitmapWidth;
   HMETAFILE   hMeta;
   PVOID       pMetaFileObject;
   int         polyFillMode;
   int         arcDirection;
   int         stretchBltMode;
   int         graphicsMode;
   HRGN        hrgnHDC;

   PS_Type     psType;

   HDC_Type    hdcType;
   USHORT      usUnused;
   POINTL      viewportOrg;
   double      viewportXExt;
   double      viewportYExt;
   POINTL      windowOrg;
   SIZEL       windowExt;
   HRGN        hrgnWin32Clip;	//current Win32 clip region handle

   POINTL      ptlOrigin;

   ULONG       printPageHeight;
   PVOID       printerObject;

   LONG        taMode;
   XFORM_W     xform;
   INT         worldYDeltaFor1Pixel;
   INT         worldXDeltaFor1Pixel;
   ULONG       colorMode;
   PULONG      pLogColorTable;

   ULONG       lcidBitfield;
   HWND        hwndRealize;
   ULONG       cpeMap;
   LONG        lTechnology;
   LONG        lWndXExtSave, lWndYExtSave,
               lVwpXExtSave, lVwpYExtSave;
   int         height;
   POINTL      brushOrgPoint;
   PVOID       pEnhMetaPalette;
   ABORTPROC   lpAbortProc;
   ULONG       HPStoHDCInversionHeight;
   int         saveLevel;
   CHARBUNDLE  CBundle;
   CHARBUNDLE  CSetBundle;
   ULONG       ulCharMask;
   ULONG       ulDefCharMask;
#ifdef CUSTOM_CURPOS
   POINTL      curPos;
#endif
   MATRIXLF    defView;
   int         yInvert;
   int         yInvert4Enable;

   ULONG       Reserved;

   HRGN        hrgnWinVis;	//current Win32 visible region handle
   struct      _DCData *nextHPS_HDC;
} tDCData, *pDCData;
#pragma pack()

#endif //__DCDATA_H__
