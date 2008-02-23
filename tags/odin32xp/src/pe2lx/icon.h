/* $Id: icon.h,v 1.5 2002-01-05 11:15:29 sandervl Exp $ */

/* $Header: /home/ktk/tmp/odin/odin32xp/src/pe2lx/icon.h,v 1.5 2002-01-05 11:15:29 sandervl Exp $
 *
 * Declarations, protypes and defintions for conversion of icons.
 *
 * Copyright (c) 1997-1999 Sander van Leeuwen
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __ICON_H__
#define __ICON_H__

#pragma pack(1)

/* Win v4.0 bitmap structs */
/* Image Color Matching color definitions */

typedef LONG   LCSCSTYPE;

#define LCS_CALIBRATED_RGB              0x00000000L
#define LCS_DEVICE_RGB                  0x00000001L
#define LCS_DEVICE_CMYK                 0x00000002L
typedef LONG    LCSGAMUTMATCH;

#define LCS_GM_BUSINESS                 0x00000001L
#define LCS_GM_GRAPHICS                 0x00000002L
#define LCS_GM_IMAGES                   0x00000004L

/* ICM Defines for results from CheckColorInGamut() */
#define CM_OUT_OF_GAMUT         255
#define CM_IN_GAMUT                     0

/* Macros to retrieve CMYK values from a COLORREF */
#define GetKValue(cmyk)      ((BYTE)(cmyk))
#define GetYValue(cmyk)      ((BYTE)((cmyk)>> 8))
#define GetMValue(cmyk)      ((BYTE)((cmyk)>>16))
#define GetCValue(cmyk)      ((BYTE)((cmyk)>>24))
#define CMYK(c,m,y,k)       ((COLORREF)((((BYTE)(k)|((WORD)((BYTE)(y))<<8))|(((DWORD)(BYTE)(m))<<16))|(((DWORD)(BYTE)(c))<<24)))

typedef long            FXPT16DOT16, FAR *LPFXPT16DOT16;
typedef long            FXPT2DOT30, FAR *LPFXPT2DOT30;

/* ICM Color Definitions */
// The following two structures are used for defining RGB's in terms of
// CIEXYZ. The values are fixed point 16.16.
typedef struct tagCIEXYZ
{
        FXPT2DOT30 ciexyzX;
        FXPT2DOT30 ciexyzY;
        FXPT2DOT30 ciexyzZ;
} CIEXYZ;
typedef CIEXYZ  FAR *LPCIEXYZ;

typedef struct tagICEXYZTRIPLE
{
        CIEXYZ  ciexyzRed;
        CIEXYZ  ciexyzGreen;
        CIEXYZ  ciexyzBlue;
} CIEXYZTRIPLE;
typedef CIEXYZTRIPLE    FAR *LPCIEXYZTRIPLE;

#ifndef MAX_PATH
        #define MAX_PATH CCHMAXPATH
#endif
// The next structures the logical color space. Unlike pens and brushes,
// but like palettes, there is only one way to create a LogColorSpace.
// A pointer to it must be passed, its elements can't be pushed as
// arguments.

typedef struct tagLOGCOLORSPACEA
{
        DWORD lcsSignature;
        DWORD lcsVersion;
        DWORD lcsSize;
        LCSCSTYPE lcsCSType;
        LCSGAMUTMATCH lcsIntent;
        CIEXYZTRIPLE lcsEndpoints;
        DWORD lcsGammaRed;
        DWORD lcsGammaGreen;
        DWORD lcsGammaBlue;
        CHAR   lcsFilename[MAX_PATH];
} LOGCOLORSPACEA, *LPLOGCOLORSPACEA;

typedef struct tagLOGCOLORSPACEW
{
        DWORD lcsSignature;
        DWORD lcsVersion;
        DWORD lcsSize;
        LCSCSTYPE lcsCSType;
        LCSGAMUTMATCH lcsIntent;
        CIEXYZTRIPLE lcsEndpoints;
        DWORD lcsGammaRed;
        DWORD lcsGammaGreen;
        DWORD lcsGammaBlue;
        WCHAR  lcsFilename[MAX_PATH];
} LOGCOLORSPACEW, *LPLOGCOLORSPACEW;

#ifdef UNICODE
typedef LOGCOLORSPACEW LOGCOLORSPACE;
typedef LPLOGCOLORSPACEW LPLOGCOLORSPACE;
#else
typedef LOGCOLORSPACEA LOGCOLORSPACE;
typedef LPLOGCOLORSPACEA LPLOGCOLORSPACE;
#endif // UNICODE

typedef struct
{
        DWORD        bV4Size;
        LONG         bV4Width;
        LONG         bV4Height;
        WORD         bV4Planes;
        WORD         bV4BitCount;
        DWORD        bV4V4Compression;
        DWORD        bV4SizeImage;
        LONG         bV4XPelsPerMeter;
        LONG         bV4YPelsPerMeter;
        DWORD        bV4ClrUsed;
        DWORD        bV4ClrImportant;
        DWORD        bV4RedMask;
        DWORD        bV4GreenMask;
        DWORD        bV4BlueMask;
        DWORD        bV4AlphaMask;
        DWORD        bV4CSType;
        CIEXYZTRIPLE bV4Endpoints;
        DWORD        bV4GammaRed;
        DWORD        bV4GammaGreen;
        DWORD        bV4GammaBlue;
} BITMAPV4HEADER,       /*FAR *LPBITMAPV4HEADER,*/ *PBITMAPV4HEADER;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

/* Win v3.0 bitmap structs */
typedef struct
{
        WORD   wReserved;                               // Currently zero
        WORD   wType;                                   // 1 for icons
        WORD   wCount;                                  // Number of components
//Fout in docs, geen padding
//  WORD   padding;            // filler for DWORD alignment
} IconHeader;

typedef struct tagWINBITMAPINFOHEADER
{
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
typedef struct
{
        BYTE   bWidth;
        BYTE   bHeight;
        BYTE   bColorCount;
        BYTE   bReserved;
        WORD   wPlanes;
        WORD   wBitCount;
        DWORD  lBytesInRes;
        WORD   wNameOrdinal;                    // Points to component
//Fout in docs, geen padding
//  WORD   padding;            // filler for DWORD alignment
} ResourceDirectory;

#pragma pack()

class OS2Icon
{
public:
        // Constructors and destructors
        OS2Icon(int id, WINBITMAPINFOHEADER *bmpHdr, int size);
        ~OS2Icon();

        int                QueryIconSize();
        void               SetIconHdrOffset(int offset);
        BITMAPFILEHEADER2 *GetIconHeader();

        static OS2Icon    *GetIcon(int id);
        static void        DestroyAll();

protected:

private:
        int                       id, iconsize, prevoffset;
        BITMAPFILEHEADER2        *iconhdr;
        BITMAPFILEHEADER2        *iconhdr2;

        //@KSO: This may need to be changed later. Statics work fine for one single file,
        //      but not necessarily for concurrent files. But for the time beeing, it is
        //      not a problem.
        // Linked list management
        OS2Icon                  *next;                                         // Next Icon
        static OS2Icon           *icons;                                                // List of Icons
};
#endif

