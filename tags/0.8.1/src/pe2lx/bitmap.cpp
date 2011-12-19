/* $Id: bitmap.cpp,v 1.3 1999-06-10 17:08:52 phaller Exp $ */

/*
 * PE2LX bitmap conversion code
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Knut St. Osmundsen
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_GPIBITMAPS
#define INCL_BITMAPFILEFORMAT
#define INCL_DOSFILEMGR          /* File Manager values      */
#define INCL_DOSERRORS           /* DOS Error values         */
#define INCL_DOSPROCESS          /* DOS Process values       */
#define INCL_DOSMISC             /* DOS Miscellanous values  */
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <assert.h>
#include "pefile.h"
#include "lx.h"
#include "icon.h"
#include "bitmap.h"
#include "misc.h"


#define __StackToFlat(a) a
#define ltassert(a)
#define MAXPTR
#define MINPTR


/*
 *   HISTORY
 *  * * * * *
 *      KSO - Aug 3 1998 6:30pm:
 * -Fixed some bugs.
 * -Standalone bitmap converter.
 * -Need a 16 and/or 32bit bitmaps to test these and to implement bitfields
 *
 *      KSO - Aug 1 1998 6:22pm:
 * -Rewrote everything.
 * -New bitmaps from nt/win9x are not supported but detected. Need docs!!!
 * -2 COLOR (not BW) bitmaps are now converted to 16 color bitmaps.
 *  Converting of OS/2 v1.x bitmaps (2 color -> 16 color) are not tested
 * -All Resources should have 3 operation: Show<rsrc>, Convert<rsrc> and QuerySize<rsrc>
 */


/* COMPILETIME CONVERSION OPTIONS */
//#define NO_1BIT_TO_4BIT_CONVERSION
//#define DUMP_BMPS_TO_FILES

#ifdef DUMP_BMPS_TO_FILES
        #include <stdio.h>
#endif

static ULONG Convert1BitTo4Bit(void *pNewData, PBYTE pbOldData, LONG cx, LONG cy);
static ULONG QueryPaletteSize(WINBITMAPINFOHEADER *pBHdr);
static ULONG QueryPaletteSize(BITMAPINFOHEADER *pBHdr);
static ULONG QueryPaletteSize(BITMAPINFOHEADER2 *pBHdr);
static ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy);



/**********/
/* Macros */
/**********/
#define Need1BitTo4BitConvertWIN(pBHdr)         \
        (                                                                                                               \
                pBHdr->biBitCount == 1                                          \
                && !(                                                                                           \
                        (((PULONG)pBHdr)[10] == 0x00000000 && (pBHdr->biClrUsed == 1 || ((PULONG)pBHdr)[11] == 0x00ffffff))  /*black and white*/  \
                || (((PULONG)pBHdr)[10] == 0x00ffffff && (pBHdr->biClrUsed == 1 || ((PULONG)pBHdr)[11] == 0x00000000))  /*white and black*/  \
                )                                                                                                       \
        )

#define Need1BitTo4BitConvertOS2(pBIH)                  \
        (                                                                                                               \
                pBIH->cBitCount == 1                                                    \
                && !(                                                                                           \
                        (((PULONG)pBIH)[3] == 0x0ff00000 && ((PUSHORT)pBIH)[8] == 0xffff)  /*black and white*/  \
                || (((PULONG)pBIH)[3] == 0x00ffffff && ((PUSHORT)pBIH)[8] == 0x0000)  /*white and black*/  \
                )                                                                                                       \
        )



/*************/
/* Functions */
/*************/
#ifndef BITMAPCONVERTER

BOOL ShowBitmap(LXHeader &OS2Exe, int id, WINBITMAPINFOHEADER *bhdr, int size)
{
        void   *pConvertedBitmap;
        ULONG     ulSize;
        BOOL    rc;

        pConvertedBitmap = ConvertBitmap(bhdr, size, (PULONG)__StackToFlat(&ulSize));
        if (pConvertedBitmap != NULL)
        {
                OS2Exe.StoreResource(id, RT_BITMAP, ulSize, (char *)pConvertedBitmap);

                #ifdef DUMP_BMPS_TO_FILES
                        /* debug option to dump bitmaps to file(s). */
                        char szFilename[CCHMAXPATH];
                        sprintf(__StackToFlat(szFilename), "%d.bmp", id);
                        FILE *pFile = fopen(szFilename, "wb");
                        if (pFile != NULL)
                        {
                                fwrite(pConvertedBitmap, 1, ulSize, pFile);
                                fclose(pFile);
                        }
                        else
                                cout << "warning: could not open " << szFilename << endl;

                        sprintf(__StackToFlat(szFilename), "w%d.bmp", id);
                        pFile = fopen(szFilename, "wb");
                        if (pFile != NULL)
                        {
                                PBITMAPFILEHEADER pBFHConv = (PBITMAPFILEHEADER)pConvertedBitmap;
                                BITMAPFILEHEADER BFHWin;
                                memcpy( (void*)&BFHWin, (void*)pConvertedBitmap, sizeof(BFHWin));
                                BFHWin.cbSize  -= pBFHConv->bmp.cbFix - bhdr->biSize;
                                BFHWin.offBits -= pBFHConv->bmp.cbFix - bhdr->biSize;

                                fwrite(&BFHWin, 1, 14, pFile);
                                fwrite(bhdr, 1, size, pFile);
                                fclose(pFile);
                        }
                        else
                                cout << "warning: could not open " << szFilename << endl;

                #endif //DUMP_BMPS_TO_FILES
                free(pConvertedBitmap);
        }
        else
                rc = FALSE;

        return rc;
}

#endif //#ifndef BITMAPCONVERTER


void *ConvertBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize, PULONG pulSize)
{
        ltassert((ULONG)pBHdr > MINPTR && (ULONG)pBHdr+ulSize < MAXPTR);

        switch (pBHdr->biSize)
        {
                /**************************************/
                /* Windows v3.0 bitmap                */
      /**************************************/
      case sizeof(WINBITMAPINFOHEADER):
                {//convert to OS/2 v2.0 bitmap
                        PBITMAPFILEHEADER2 pBFH;
                        ULONG cbPalette;
                        ULONG cbImage;
                        BOOL fConvert1Bit = FALSE;

                        cout << "Windows v3.0 bitmap" << endl;
                        cout << "  Size:     " << pBHdr->biSizeImage << endl;
                        cout << "  Width:    " << pBHdr->biWidth << endl;
                        cout << "  Height:   " << pBHdr->biHeight << endl;
                        cout << "  Bitcount: " << pBHdr->biBitCount << endl;
                        cout << "  Compress: " << pBHdr->biCompression << endl;

                        #ifndef NO_1BIT_TO_4BIT_CONVERSION
                                fConvert1Bit = Need1BitTo4BitConvertWIN(pBHdr);
                        #endif

                        cbPalette = QueryPaletteSize(pBHdr);
                        ltassert(cbPalette != -1);
                        cout << "  PaletteSize: " << cbPalette << endl;


                        /* calc bitmap size and allocate */
                        #ifdef BITFIELDS
                        if (pBHdr->biCompression == BI_RGB || pBHdr->biCompression == BI_BITFIELDS)
                        #else
                        if (pBHdr->biCompression == BI_RGB)
                        #endif
                        {
                                /* recalc for non-compressed images */
                                if (fConvert1Bit)
                                        cbImage = CalcBitmapSize(4, pBHdr->biWidth, pBHdr->biHeight);
                                else
                                        cbImage = CalcBitmapSize(pBHdr->biBitCount, pBHdr->biWidth, pBHdr->biHeight);
                        }
                        else
                        {
                                ltassert(!fConvert1Bit);
                                cbImage = pBHdr->biSizeImage;
                                if (cbImage == 0)
                                        cbImage = ulSize - sizeof(WINBITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER2);
                        }
                        ltassert(cbImage != 0);

                        //bounds checking
                        if (!fConvert1Bit)
                        {       /* brackets must not be removed! */
                                ltassert(cbImage <= ulSize - sizeof(WINBITMAPINFOHEADER) - cbPalette);
                        }
                        else
                        {
                                ltassert(CalcBitmapSize(pBHdr->biBitCount, pBHdr->biWidth, pBHdr->biHeight) <= ulSize - sizeof(WINBITMAPINFOHEADER) - cbPalette);
                        }

                        *pulSize =  sizeof(BITMAPFILEHEADER2) + cbPalette + cbImage;

                        cout << "  Debug: Size(recalced): " << cbImage << endl;
                        cout << "  Debug: Size in       : " << ulSize << endl;
                        cout << "  Debug: Size out      : " << *pulSize << endl;

                        pBFH = (PBITMAPFILEHEADER2)malloc(*pulSize + 0x10);//allocate more than we need!
                        ltassert(pBFH);


                        /* file header */
                        pBFH->usType     = BFT_BMAP;
                        pBFH->cbSize     = sizeof(BITMAPFILEHEADER2);
                        pBFH->xHotspot   = 0;
                        pBFH->yHotspot   = 0;
                        pBFH->offBits    = sizeof(BITMAPFILEHEADER2) +  cbPalette;


                        /* info header*/
                        memset((void*)&pBFH->bmp2, 0, sizeof(BITMAPINFOHEADER2));
                        memcpy((void*)&pBFH->bmp2, (void*)pBHdr, sizeof(WINBITMAPINFOHEADER));
                        pBFH->bmp2.cbFix = sizeof(BITMAPINFOHEADER2);

                        /* bitfields */
                        if (pBFH->bmp2.ulCompression == BI_BITFIELDS)
                        {//TODO: Identical except for BI_BITFIELDS (3L) type! - cast a warning note
                                /* KSO: I need files to test it on */
                                #ifndef BITFIELDS
                                        cout << "Warning: ulCompression = BI_BITFIELDS" << endl;
                                        pBFH->bmp2.ulCompression = BCA_UNCOMP; //since BI_BITFIELDS == BCA_RLE24
                                #else
                                        #error("Sorry, BITFIELDS are not implmented yet.")
                                        /* generate new palette */
                                        /* convert bitmap data */
                                        /* update fileheader */
                                #endif //#ifdef BITFIELDS
                                #ifdef RING0
                                        ltassert(FALSE && "bitfields are fatal\n");
                                #endif
                        }


                        /* palette */
                        if (cbPalette > 0)
                                memcpy((void*)((ULONG)pBFH + sizeof(BITMAPFILEHEADER2)), (void*)((ULONG)pBHdr + sizeof(WINBITMAPINFOHEADER)), cbPalette);


                        /* bitmap data */
                        if (fConvert1Bit)
                        {
                                /* convert */
                                Convert1BitTo4Bit(
                                        (PVOID)((ULONG)pBFH + sizeof(BITMAPFILEHEADER2)   + cbPalette),
                                        (PBYTE)((ULONG)pBHdr + sizeof(WINBITMAPINFOHEADER)+ cbPalette),
                                        pBFH->bmp2.cx,
                                        pBFH->bmp2.cy
                                        );

                                /* update some infoheader fields */
                                pBFH->bmp2.cBitCount = 4;
                                if (pBFH->bmp2.cclrUsed == 0)
                                        pBFH->bmp2.cclrUsed = 2;
                                if (pBFH->bmp2.cbImage != 0)
                                        pBFH->bmp2.cbImage = cbImage;
                        }
                        else
                                memcpy((void*)((ULONG)pBFH + sizeof(BITMAPFILEHEADER2) + cbPalette), (void*)((ULONG)pBHdr + sizeof(WINBITMAPINFOHEADER) + cbPalette), cbImage);

                        return pBFH;
                }



                /**************************************/
                /* Windows v4.0 bitmap                */
      /**************************************/
                case sizeof(BITMAPV4HEADER):
                {//convert to OS/2 v2.0 bitmap - some other day when I find the specs.
                        cout << "Windows v4.0 bitmap" << endl;
                        cout << "  Not supported yet!" << endl;
                        ltassert(FALSE && "Windows v4.0 bitmap");
                }
                break;


                /**************************************/
                /* OS/2 v1.x bitmap                   */
      /**************************************/
                case sizeof(BITMAPINFOHEADER):
                {//no conversion needed - only build fileheader
         PBITMAPINFOHEADER pBIH = (PBITMAPINFOHEADER)pBHdr;
         PBITMAPFILEHEADER pBFH ;
         BOOL fConvert1Bit;
                        ULONG cbPalette;

                        cbPalette = QueryPaletteSize(pBIH);
                        ltassert(cbPalette != -1);
                        *pulSize = ulSize + sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);

                        fConvert1Bit = Need1BitTo4BitConvertOS2(pBIH);
                        #ifdef NO_1BIT_TO_4BIT_CONVERSION
                                fConvert1Bit = FALSE;
                        #endif

                        if (fConvert1Bit)
                                *pulSize += (ulSize - cbPalette - sizeof(BITMAPINFOHEADER))*3 + 14 * sizeof(RGB);

                        pBFH = (PBITMAPFILEHEADER)malloc(*pulSize);
                        ltassert(pBFH != NULL);

                        if (fConvert1Bit)
                        {
                                /* copy infoheader */
                                memcpy(&pBFH->bmp, pBIH, sizeof(pBFH->bmp));
                                memset((void*)((ULONG)pBFH + sizeof(*pBFH)), 0, 16*sizeof(RGB));
                                memcpy((void*)((ULONG)pBFH + sizeof(*pBFH)), (void*)((ULONG)pBIH + sizeof(*pBIH)), 2*sizeof(RGB));
                                *pulSize  = Convert1BitTo4Bit(
                                        (PVOID)((ULONG)pBFH + sizeof(BITMAPFILEHEADER) + 16 * sizeof(RGB)),
                                        (PBYTE)((ULONG)pBHdr + sizeof(BITMAPINFOHEADER) + cbPalette),
                                        pBFH->bmp.cx,
                                        pBFH->bmp.cy
                                        );

                                /* update infoheader */
                                pBFH->bmp.cBitCount = 4;
                                cbPalette = 16 * sizeof(RGB); //used in fileheader
                        }
                        else
                        {
                                /* copy entire bitmap from infoheader */
                                memcpy(&pBFH->bmp, pBIH, ulSize);
                                *pulSize  = CalcBitmapSize(pBIH->cBitCount, pBIH->cx, pBIH->cy);
                                ltassert(*pulSize != 0);
                        }
                        *pulSize += sizeof(BITMAPFILEHEADER) + cbPalette;

                        /* fileheader */
                        pBFH->usType     = BFT_BMAP;
                        pBFH->cbSize     = sizeof(BITMAPFILEHEADER);
                        pBFH->xHotspot   = 0;
                        pBFH->yHotspot   = 0;
                        pBFH->offBits    = sizeof(BITMAPFILEHEADER) + cbPalette;

                        /* log info */
                        cout << "OS/2 v1.x bitmap" << endl;
                        cout << "  Width:    " << pBFH->bmp.cx << endl;
                        cout << "  Height:   " << pBFH->bmp.cy << endl;
                        cout << "  Bitcount: " << pBFH->bmp.cBitCount << endl;
                        cout << "  PaletteSize: " << cbPalette << endl;
                        cout << "debug 1" << endl;

                        return (void*)pBFH;
                }



                /**************************************/
                /* OS/2 v2.0 bitmap - highly unlikely!*/
                /**************************************/
                case sizeof(BITMAPINFOHEADER2):
                {//no conversion needed - only build fileheader
         PBITMAPFILEHEADER2 pBFH;
                        pBFH = (PBITMAPFILEHEADER2)malloc(ulSize + sizeof(BITMAPFILEHEADER2) - sizeof(BITMAPINFOHEADER2));
                        ltassert(pBFH != NULL);

                        memcpy(&pBFH->bmp2, pBHdr, ulSize);

                        pBFH->usType     = BFT_BMAP;
                        pBFH->cbSize     = sizeof(BITMAPFILEHEADER2);
                        pBFH->xHotspot   = 0;
                        pBFH->yHotspot   = 0;
                        pBFH->offBits    = sizeof(BITMAPFILEHEADER2);

                        cout << "OS/2 v2.0 bitmap" << endl;
                        cout << "  Width:    " << pBFH->bmp2.cx << endl;
                        cout << "  Height:   " << pBFH->bmp2.cy << endl;
                        cout << "  Bitcount: " << pBFH->bmp2.cBitCount << endl;

                        if (pBFH->bmp2.cBitCount < 24)
                                pBFH->offBits += ( 1 << pBFH->bmp2.cBitCount ) * sizeof(RGB2);
                        else
                                pBFH->offBits += pBFH->bmp2.cclrUsed;

                        return pBFH;
                }


                /**************************************/
                /* Unknown                            */
                /**************************************/
                default: //fail
                        cout << "ConvertBitmap - default - fail!" << endl;
                        return NULL;
        }

        return NULL;
}




/* Converting 1 bit bitmaps to 4 bit bitmaps
 */
static ULONG  Convert1BitTo4Bit(void *pNewData, PBYTE pbOldData, LONG cx, LONG cy)
{
        PBYTE pbNewData = (PBYTE)pNewData;

        cout << "  1->4 convert" << endl;

        //negative height
        cy = cy < 0 ? -cy : cy;

        *pbNewData = 0;
        for (int y = 0; y < cy; y++)
        {//outer loop: lines
                int x;

                for (x = 0; x < cx; x++)
                {//inner loop: scanline
                        int iBit = 7-(x%8);
                        *pbNewData |= (((0x1 << iBit) & *pbOldData) >> iBit) << (iBit%2)*4;

                        if (x % 2)
                        {
                                pbNewData++;
                                *pbNewData = 0;
                        }

                        if ((x+1) % 8 == 0)
                                pbOldData++;
                }

                //next byte
                if (x % 2)
                {
                        pbNewData++;
                        *pbNewData = 0;
                }
                if ((x+1) % 8 == 0)
                        pbOldData++;

                //aligning
                for (int a = ((x+1)/2)%4; a < 4 && a != 0; a++)
                {
                        pbNewData++;
                        *pbNewData = 0;
                }
                for (int b = ((x+7)/8)%4; b < 4 && b != 0; b++)
                        pbOldData++;
        }

        return (int)pbNewData - (int)pNewData;
}



static ULONG QueryPaletteSize(WINBITMAPINFOHEADER *pBHdr)
{
        ULONG cbPalette;

        switch (pBHdr->biBitCount)
        {
                case 1:
                case 4:
                case 8:
                        /* Windows Bitmaps seems to have a full palette even when it don't use all of it - OS/2 does not...
                           Good for us, the fileheader holds the offset to the bitmap data - which means that this not should be a problem, luckily. */
                        //cbPalette = ((pBHdr->biClrUsed != 0) ? pBHdr->biClrUsed : 1 << pBHdr->biBitCount) * sizeof(RGB2);
                        cbPalette =  (1 << pBHdr->biBitCount) * sizeof(RGB2);
                        break;

                case 16:
                case 32:
                        #ifdef BITFIELDS
                                if (pBHdr->biCompression == BI_BITFIELDS)
                                {
                                        cbPalette = 3* sizeof(DWORD);
                                        break;
                                }
                        #endif
                case 24:
                        /* docs have it that there may be a palette used for optmizing processing of 16, 24 and 32 bits bitmaps */
                        /* the size of the palette is contained in biClrUsed  - don't know if it stored as number of colors or as bytes... */
                        cbPalette = pBHdr->biClrUsed; //size in bytes or in (*sizeof(RGB2))?
                        break;

                default:
                        cout << "QueryPaletteSize: error pBHdr->biBitCount = " << pBHdr->biBitCount << endl;
                        cbPalette = -1;
        }

   return cbPalette;
}


static ULONG QueryPaletteSize(BITMAPINFOHEADER *pBHdr)
{
        ULONG cbPalette;

        switch (pBHdr->cBitCount)
        {
                case 1:
                case 4:
                case 8:
                        cbPalette = (1 << pBHdr->cBitCount) * sizeof(RGB);
                        break;

                case 16:
                case 24:
                case 32:
                        cbPalette = 0;
                        break;

                default:
                        cout << "QueryPaletteSize: error pBHdr->biBitCount = " << pBHdr->cBitCount << endl;
                        cbPalette = -1;
        }

   return cbPalette;
}


static ULONG QueryPaletteSize(BITMAPINFOHEADER2 *pBHdr)
{
        ULONG cbPalette;

        switch (pBHdr->cBitCount)
        {
                case 1:
                case 4:
                case 8:
                        cbPalette = ((pBHdr->cclrUsed != 0) ? pBHdr->cclrUsed : 1 << pBHdr->cBitCount) * sizeof(RGB2);
                        //cbPalette =  (1 << pBHdr->cBitCount) * sizeof(RGB2);
                        break;

                case 16:
                case 32:
                case 24:
                        /* docs have it that there may be a palette used for optmizing processing of 16, 24 and 32 bits bitmaps */
                        /* the size of the palette is contained in biClrUsed  - don't know if it stored as number of colors or as bytes... */
                        cbPalette = pBHdr->cclrUsed; //size in bytes or in (*sizeof(RGB2))?
                        break;

                default:
                        cout << "QueryPaletteSize: error pBHdr->biBitCount = " << pBHdr->cBitCount << endl;
                        cbPalette = -1;
        }

   return cbPalette;
}




static ULONG CalcBitmapSize(ULONG cBits, LONG cx, LONG cy)
{
        ULONG alignment;
        ULONG factor;
        BOOL flag = TRUE;       //true: '*'     false: '/'

        cy = cy < 0 ? -cy : cy;

        switch(cBits)
        {
                case 1:
                        factor = 8;
                        flag = FALSE;
                        break;

                case 4:
                        factor = 2;
                        flag = FALSE;
                        break;

                case 8:
                        factor = 1;
                        break;

                case 16:
                        factor = 2;
                        break;

                case 24:
                        factor = 3;
                        break;

                case 32:
                        return cx*cy;

                default:
                        return 0;
        }

        if (flag)
                alignment = (cx = (cx*factor)) % 4;
        else
                alignment = (cx = ((cx+factor-1)/factor)) % 4;

        if (alignment != 0)
                cx += 4 - alignment;

        return cx*cy;
}


ULONG QuerySizeBitmap(WINBITMAPINFOHEADER *pBHdr, ULONG ulSize)
{
        ULONG retSize;
        ltassert((ULONG)pBHdr > MINPTR && (ULONG)pBHdr+ulSize < MAXPTR);

        switch (pBHdr->biSize)
        {
                /**************************************/
                /* Windows v3.0 bitmap                */
      /**************************************/
      case sizeof(WINBITMAPINFOHEADER):
                {//convert to OS/2 v2.0 bitmap
                        #ifdef BITFIELDS
                        if (pBHdr->biCompression == BI_RGB || pBHdr->biCompression == BI_BITFIELDS)
                        #else
                        if (pBHdr->biCompression == BI_RGB)
                        #endif
                        {
                                if (Need1BitTo4BitConvertWIN(pBHdr))
                                        retSize = CalcBitmapSize(4, pBHdr->biWidth, pBHdr->biHeight);
                                else
                                        retSize = CalcBitmapSize(pBHdr->biBitCount, pBHdr->biWidth, pBHdr->biHeight);
                        }
                        else
                        {
                                retSize = pBHdr->biSizeImage;
                                if (retSize == 0)
                                        retSize = ulSize - sizeof(WINBITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER2);
                        }
                        ltassert(retSize != 0);

                        ULONG a = QueryPaletteSize(pBHdr);
                        ltassert(a != -1);
                        retSize += sizeof(BITMAPFILEHEADER2) + a;
                }
                break;

                /**************************************/
                /* Windows v4.0 bitmap                */
      /**************************************/
                case sizeof(BITMAPV4HEADER):
                {//convert to OS/2 v2.0 bitmap - some other day when I find the specs.
                        ltassert(FALSE && "Windows v4.0 bitmap");
                }
                break;

                /**************************************/
                /* OS/2 v1.x bitmap                   */
      /**************************************/
                case sizeof(BITMAPINFOHEADER):
                {//no conversion needed - only build fileheader
                        PBITMAPINFOHEADER pBIH = (PBITMAPINFOHEADER)pBHdr;

                        if (Need1BitTo4BitConvertOS2(pBIH))
                        {
                                retSize = CalcBitmapSize(4, pBIH->cx, pBIH->cy);
                                ltassert(retSize != 0);

                                retSize += sizeof(RGB)*16; //palette
                        }
                        else
                        {
                                retSize = CalcBitmapSize(pBIH->cBitCount, pBIH->cx, pBIH->cy);
                                ltassert(retSize != 0);

                                ULONG a = QueryPaletteSize(pBIH);
                                ltassert(a != -1);
                                retSize += a;
                        }

                        retSize += sizeof(BITMAPFILEHEADER);
                }

      /**************************************/
                /* OS/2 v2.0 bitmap - highly unlikely!*/
                /**************************************/
                case sizeof(BITMAPINFOHEADER2):
                {//no conversion needed - only build fileheader
                        return ulSize + sizeof(BITMAPFILEHEADER2) - sizeof(BITMAPINFOHEADER2);
                }

                /**************************************/
                /* Unknown                            */
                /**************************************/
                default: //fail
                        cout << "QuerySizeBitmap - default - fail!" << endl;
                        return 0;
        }

        return retSize;
}


#ifdef BITMAPCONVERTER

ULONG fsize(FILE *ph)
{
        LONG  lPos;
        ULONG ulSize;

        lPos   = ftell(ph);
        fseek(ph, 0, SEEK_END);
        ulSize = ftell(ph);
        fseek(ph, lPos, SEEK_SET);

        return ulSize;
}


void main(int argi, char **argc)
{
        char  *pF1Buf;
        char  *pF2Buf;
        char  *pszF1;
        char  *pszF2;
        FILE  *phF1;
        FILE  *phF2;
   ULONG  cbF1;
   ULONG  cbF2;

        /* tittle */
        cout << "PE2LX Resources: Bitmap Convert v0.0.2alpha" << endl;

        /* quick test of arguments */
        if (argi < 3)
        {
                cout << "error: incorrect number of parameters" << endl;
                return;
        }
        pszF1 = argc[1];
        pszF2 = argc[2];

        /* open files */
        phF1 = fopen(pszF1, "rb");
        phF2 = fopen(pszF2, "wb");
        if (phF1 == NULL || phF2 == NULL)
        {
                cout << "error: could not open file ";
                if (phF1 == NULL)
                        cout << pszF1 << endl;
                else
                        cout << pszF2 << endl;
        }


        /* read infile */
        cbF1 = fsize(phF1);
        if (cbF1 > sizeof(BITMAPFILEHEADER))
        {
                pF1Buf = (char*)malloc(cbF1);
                assert(pF1Buf != NULL);

                if (fread(pF1Buf, 1, cbF1, phF1))
                {
                        ULONG ulOffset;
                        if (pF1Buf[0]=='B' && pF1Buf[1]=='M')
                                ulOffset = sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
                        else
                                ulOffset = 0;
                        pF2Buf = (char*)ConvertBitmap((WINBITMAPINFOHEADER*)&pF1Buf[ulOffset], cbF1-ulOffset, &cbF2);
                        if (pF2Buf != NULL)
                        {
                                cout << "filesize: " << cbF2 << endl;
                                if (fwrite (pF2Buf, 1, cbF2, phF2))
                                        cout << "Converted bitmap is successfully written to disk." << endl;
                                else
                                        cout << "error: error occured while writing converted bitmap to disk." << endl;
                                free(pF2Buf);
                        }
                        else
                                cout << "error: failed!" << endl;
        }
                else
                        cout << "error: error while reading infile" << endl;

                free(pF1Buf);
        }
        else
                cout << "error: invalid infile" << endl;

        /* finished */
        fclose(phF1);
        fclose(phF2);
}

#endif
