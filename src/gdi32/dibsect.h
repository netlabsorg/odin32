/* $Id: dibsect.h,v 1.9 1999-12-30 11:21:30 sandervl Exp $ */

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
} WINBITMAPINFOHEADER;

typedef struct
{
    INT  bmType;
    INT  bmWidth;
    INT  bmHeight;
    INT  bmWidthBytes;
    WORD   bmPlanes;
    WORD   bmBitsPixel;
    LPVOID bmBits;
} WINBITMAP, *LPWINBITMAP;

#ifdef OS2_ONLY
typedef struct
{
  WINBITMAP   dsBm;
  WINBITMAPINFOHEADER dsBmih;
  DWORD     dsBitfields[3];
  HANDLE    dshSection;
  DWORD     dsOffset;
} DIBSECTION,*LPDIBSECTION;
#endif

class DIBSection
{
public:
              DIBSection(WINBITMAPINFOHEADER *pbmi, DWORD handle, int fFlip);
             ~DIBSection();

              char *GetDIBObject()           { return bmpBits;  };
              void  SelectDIBObject(HDC hdc);
              char  GetBitCount();
              void  UnSelectDIBObject()      { this->hdc = 0;   };

              BOOL  BitBlt(HDC hdcDest, int nXdest, int nYDest,
                           int nDestWidth, int nDestHeight,
                           int nXsrc, int nYSrc,
                           int nSrcWidth, int nSrcHeight,
                           DWORD Rop);

               int  SetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb);

         int  SetDIBits(HDC hdc, HBITMAP hbitmap, UINT startscan, UINT
                              lines, const VOID *bits, WINBITMAPINFOHEADER *pbmi,
                              UINT coloruse);

               int  GetDIBSection(int iSize , void *lpBuffer);

 static DIBSection *getSection() { return section; } ;
 static DIBSection *find(DWORD handle);
 static DIBSection *findHDC(HDC hdc);
 static       void  deleteSection(DWORD handle);
protected:

private:
          DWORD handle;
          HWND  hwndParent;
          HDC   hdc;
          char *bmpBits;
          BOOL  fFlip;
          int   bmpsize;
    BITMAPINFO2 *pOS2bmp;
                             // Linked list management
              DIBSection*    next;                   // Next DIB section
    static    DIBSection*    section;                // List of DIB sections
};

#endif


