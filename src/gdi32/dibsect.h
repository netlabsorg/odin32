/* $Id: dibsect.h,v 1.18 2000-05-23 18:46:20 sandervl Exp $ */

/*
 * GDI32 DIB sections
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DIBSECT_H__
#define __DIBSECT_H__

#ifdef OS2_ONLY
typedef struct
{
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;
#else
#define BITMAPINFO2     DWORD
#endif

#define FLIP_VERT       1
#define FLIP_HOR        2

typedef struct {
        DWORD    biSize;
        LONG     biWidth;
        LONG     biHeight;
        WORD     biPlanes;
        WORD     biBitCount;
        DWORD    biCompression;
        DWORD    biSizeImage;
        LONG     biXPelsPerMeter;
        LONG     biYPelsPerMeter;
        DWORD    biClrUsed;
        DWORD    biClrImportant;
} BITMAPINFOHEADER_W;

typedef struct
{
    INT  bmType;
    INT  bmWidth;
    INT  bmHeight;
    INT  bmWidthBytes;
    WORD   bmPlanes;
    WORD   bmBitsPixel;
    LPVOID bmBits;
} BITMAP_W, *LPBITMAP_W;

#ifdef OS2_ONLY
#define DIB_RGB_COLORS   0
#define DIB_PAL_COLORS   1

typedef struct
{
  BITMAP_W   dsBm;
  BITMAPINFOHEADER_W dsBmih;
  DWORD     dsBitfields[3];
  HANDLE    dshSection;
  DWORD     dsOffset;
} DIBSECTION,*LPDIBSECTION;
#endif

class DIBSection
{
public:
              DIBSection(BITMAPINFOHEADER_W *pbmi, char *pColors, DWORD iUsage, DWORD hSection, DWORD dwOffset, DWORD handle, int fFlip);
             ~DIBSection();

              char *GetDIBObject()           { return bmpBits;  };
              void  SelectDIBObject(HDC hdc);
              int   GetBitCount();
              int   GetHeight();
              void  UnSelectDIBObject()      { this->hdc = 0;   };

	      DWORD GetBitmapHandle()	     { return handle; };
	      DWORD GetRGBUsage()            { return iUsage; };

              BOOL  BitBlt(HDC hdcDest, int nXdest, int nYDest,
                           int nDestWidth, int nDestHeight,
                           int nXsrc, int nYSrc,
                           int nSrcWidth, int nSrcHeight,
                           DWORD Rop);
              void  sync(HDC hdc, DWORD nYdest, DWORD nDestHeight);
               int  SetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb);

               int  SetDIBits(HDC hdc, HBITMAP hbitmap, UINT startscan, UINT
                              lines, const VOID *bits, BITMAPINFOHEADER_W *pbmi,
                              UINT coloruse);

               int  GetDIBSection(int iSize , void *lpBuffer);

 static DIBSection *getSection() { return section; } ;
 static DIBSection *find(DWORD handle);
 static DIBSection *findHDC(HDC hdc);
 static       void  deleteSection(DWORD handle);
protected:

private:
          DWORD handle, iUsage;
          DWORD hSection;
          HWND  hwndParent;
          HDC   hdc;
          char *bmpBits, *bmpBitsBuffer;
          BOOL  fFlip;
          int   bmpsize, os2bmphdrsize;
    DIBSECTION  dibinfo;

    BITMAPINFO2 *pOS2bmp;
                             // Linked list management
              DIBSection*    next;                   // Next DIB section
    static    DIBSection*    section;                // List of DIB sections
};

#endif


