/* $Id: dibsect.h,v 1.24 2001-03-27 20:47:52 sandervl Exp $ */

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

typedef struct tagPALETTEENTRY
{
	BYTE peRed, peGreen, peBlue, peFlags;
} PALETTEENTRY, *PPALETTEENTRY, *LPPALETTEENTRY;

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

typedef struct {
  BITMAPINFOHEADER_W bmiHeader;
  RGBQUAD bmiColors[1];
} BITMAPINFO_W;
typedef BITMAPINFO *LPBITMAPINFO;

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
              int   GetWidth();
              void  UnSelectDIBObject()      { this->hdc = 0;   };

        DWORD GetBitmapHandle()      { return handle; };
              void  SetBitmapHandle(DWORD bmphandle) { handle = bmphandle; };
        DWORD GetRGBUsage()            { return iUsage; };

              BOOL  BitBlt(HDC hdcDest, int nXdest, int nYDest,
                           int nDestWidth, int nDestHeight,
                           int nXsrc, int nYSrc,
                           int nSrcWidth, int nSrcHeight,
                           DWORD Rop);
              void  sync(HDC hdc, DWORD nYdest, DWORD nDestHeight, BOOL orgYInversion = TRUE);
              void  sync(DWORD xDst, DWORD yDst, DWORD widthDst, DWORD heightDst, PVOID bits);
               int  SetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb);
               int  SetDIBColorTable(int startIdx, int cEntries, PALETTEENTRY *rgb);

               int  SetDIBits(HDC hdc, HBITMAP hbitmap, UINT startscan, UINT
                              lines, const VOID *bits, BITMAPINFOHEADER_W *pbmi,
                              UINT coloruse);

               int  GetDIBSection(int iSize , void *lpBuffer);

 static DIBSection *getSection() { return section; } ;
 static DIBSection *findObj(HANDLE handle);
 static DIBSection *findHDC(HDC hdc);
 static       void  deleteSection(HANDLE handle);
protected:

private:
          DWORD handle, iUsage;
          DWORD hSection, dwOffset;
          HWND  hwndParent;
          HDC   hdc;
          char *bmpBits, *bmpBitsDblBuffer;
          BOOL  fFlip;
          int   bmpsize, os2bmphdrsize;
    DIBSECTION  dibinfo;

    BITMAPINFO2 *pOS2bmp;
                             // Linked list management
              DIBSection*    next;                   // Next DIB section
    static    DIBSection*    section;                // List of DIB sections
};

#endif


