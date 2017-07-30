/* $Id: dibsect.h,v 1.28 2004-01-11 11:42:11 sandervl Exp $ */

/*
 * GDI32 DIB sections
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DIBSECT_H__
#define __DIBSECT_H__

#ifdef OS2_INCLUDED
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

#ifdef OS2_INCLUDED
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

//
//Mark DIB section as invalid (dib section memory is out of sync with
//GDI bitmap).
//
//This must be done for any GDI function that modifies the bitmap
//
#define DIBSECTION_MARK_INVALID(hdc)                              \
    if(DIBSection::getSection() != NULL) {                        \
        DIBSection *dib = DIBSection::findHDC(hdc);               \
        if(dib) {                                                 \
            dib->setInvalid();                                    \
        }                                                         \
    }

#define DIBSECTION_MARK_INVALID_BMP(hBitmap)                      \
    if(DIBSection::getSection() != NULL) {                        \
        DIBSection *dib = DIBSection::findObj(hBitmap);           \
        if(dib) {                                                 \
            dib->setInvalid();                                    \
        }                                                         \
    }

//
//Check if DIB section is marked dirty (bitmap data modified by application)
//If true, then update the GDI bitmap
//
//This must be done for any GDI function that accesses the bitmap
//
#define DIBSECTION_CHECK_IF_DIRTY(hdc)                            \
    if(DIBSection::getSection() != NULL) {                        \
        DIBSection *dib = DIBSection::findHDC(hdc);               \
        if(dib && dib->isDirty()) {                               \
            dib->flush();                                         \
        }                                                         \
    }

#define DIBSECTION_CHECK_IF_DIRTY_BMP(hBitmap)                    \
    if(DIBSection::getSection() != NULL) {                        \
        DIBSection *dib = DIBSection::findObj(hBitmap);           \
        if(dib && dib->isDirty()) {                               \
            dib->flush();                                         \
        }                                                         \
    }


class DIBSection
{
public:
              DIBSection(BITMAPINFOHEADER_W *pbmi, char *pColors, DWORD iUsage, DWORD hSection, DWORD dwOffset, HBITMAP hBitmap, int fFlip);
             ~DIBSection();

              char *GetDIBObject()           { return bmpBits;  };
              void  SelectDIBObject(HDC hdc);
              int   GetBitCount();
              int   GetHeight();
              int   GetWidth();
              void  UnSelectDIBObject()      { this->hdc = 0;   };

              void  setDirty()               { fDirty = TRUE;   };
              BOOL  isDirty()                { return fDirty;   }
              void  setInvalid();
              BOOL  isInvalid()              { return fInvalid; };

        DWORD GetBitmapHandle()              { return hBitmap; };
              void  SetBitmapHandle(DWORD bmphandle) { hBitmap = bmphandle; };
        DWORD GetRGBUsage()            { return iUsage; };

              BOOL  BitBlt(HDC hdcDest, int nXdest, int nYDest,
                           int nDestWidth, int nDestHeight,
                           int nXsrc, int nYSrc,
                           int nSrcWidth, int nSrcHeight,
                           DWORD Rop);

              void  flush();
              void  sync();

              void  sync(HDC hdc, DWORD nYdest, DWORD nDestHeight, BOOL orgYInversion = TRUE);
              void  flush(HDC hdc, DWORD nYdest, DWORD nDestHeight, BOOL orgYInversion = TRUE);
              void  sync(DWORD xDst, DWORD yDst, DWORD widthDst, DWORD heightDst, PVOID bits);

               int  SetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb);
               int  GetDIBColorTable(int startIdx, int cEntries, RGBQUAD *rgb);
               int  SetDIBColorTable(int startIdx, int cEntries, PALETTEENTRY *rgb);

               int  GetDIBSection(int iSize , void *lpBuffer);

 static DIBSection *getSection() { return section; } ;
 static DIBSection *findObj(HBITMAP hBitmap);
 static DIBSection *findHDC(HDC hdc);
 static       void  deleteSection(HBITMAP hBitmap);

 static       void  initDIBSection();

 static       void  lock()   { EnterCriticalSection(&dibcritsect); };
 static       void  unlock() { LeaveCriticalSection(&dibcritsect); };

 static       void  syncAll();

protected:

private:
          HBITMAP hBitmap;
 
          DWORD hSection, dwOffset, iUsage, dwSize;
          HWND  hwndParent;
          HDC   hdc;
          char *bmpBits, *bmpBitsDblBuffer;
          BOOL  fFlip;
          int   bmpsize, os2bmphdrsize;
    DIBSECTION  dibinfo;

          BOOL  fDirty;		//bitmap is out of sync with dib memory
          BOOL  fInvalid;	//dib memory is out of sync with bitmap

    BITMAPINFO2 *pOS2bmp;
                             // Linked list management
              DIBSection*    next;                   // Next DIB section
    static    DIBSection*    section;                // List of DIB sections

    static    CRITICAL_SECTION dibcritsect;
};

#endif


