/* $Id: dibsect.h,v 1.3 1999-06-10 17:09:03 phaller Exp $ */

/*
 * GDI32 DIB sections
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DIBSECT_H__
#define __DIBSECT_H__

#ifdef OS2_ONLY
typedef struct {
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

class DIBSection
{
public:
              DIBSection(WINBITMAPINFOHEADER *pbmi, DWORD handle, int fFlip);
             ~DIBSection();

              char *GetDIBObject()           { return bmpBits;  };
              void  SelectDIBObject(HDC hdc);

              void  UnSelectDIBObject()      { this->hdc = 0;   };

              BOOL  BitBlt(HDC hdcDest, HWND hwndDest, int nXdest, int nYDest,
                           int nWidth, int nHeight, int nXsrc, int nYSrc, DWORD Rop);

               int  SetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb);
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

    BITMAPINFO2 *pOS2bmp;
                             // Linked list management
              DIBSection*    next;                   // Next DIB section
    static    DIBSection*    section;                // List of DIB sections
};

#endif
