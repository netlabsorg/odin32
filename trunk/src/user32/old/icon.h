/* $Id: icon.h,v 1.1 2001-05-11 08:35:18 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 icon conversion functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 *
 */
#ifndef __ICON_H__
#define __ICON_H__

extern "C" PBYTE ConvertWin32Icon(PBYTE presbits, DWORD dwResSize, DWORD *OS2ResSize);
extern "C" void  FreeIcon(void *os2icon);


#ifdef _ICON_C_

#pragma pack(1)

typedef struct {
  WORD   wReserved;          // Currently zero
  WORD   wType;              // 1 for icons
  WORD   wCount;             // Number of components
//Fout in docs, geen padding
//  WORD   padding;            // filler for DWORD alignment
} IconHeader;

typedef struct tagWINBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} WINBITMAPINFOHEADER;

typedef struct
{
   BYTE                      blue;
   BYTE                      green;
   BYTE                      red;
   BYTE                      res;
}  RGBQUAD;

//The next portion is repeated for each component resource:
typedef struct {
  BYTE   bWidth;
  BYTE   bHeight;
  BYTE   bColorCount;
  BYTE   bReserved;
  WORD   wPlanes;
  WORD   wBitCount;
  DWORD  lBytesInRes;
  WORD   wNameOrdinal;       // Points to component
//Fout in docs, geen padding
//  WORD   padding;            // filler for DWORD alignment
} ResourceDirectory;

#pragma pack()

#endif

#endif
