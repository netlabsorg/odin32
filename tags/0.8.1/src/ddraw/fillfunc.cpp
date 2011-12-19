/* $Id: fillfunc.cpp,v 1.8 2002-07-03 15:44:39 sandervl Exp $ */

/*
 * ColorFill functions
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#define INCL_BASE
#include <os2wrap.h>
#include <win32type.h>
#include <memory.h>
#include <misc.h>
#include "fillfunc.h"
#include "asmutil.h"

//
// Blt Functions
//

//*****************************************************************************
//*****************************************************************************
void CDECL Fill8(char* pDst, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, DWORD dwColor)
{
    DWORD *pColor;
    char  *pFillPos;
    
    dprintf(("DDRAW:Fill8 %x (%d,%d) %d with %x \n", pDst, dwWidth, dwHeight, dwPitch, dwColor));

    if(dwWidth == dwPitch)
    {
        // Width = Pitch => fill buffer so no need to take care of lines
        memset(pDst, dwColor, dwWidth*dwHeight);
    }
    else
    {
        pFillPos = pDst;

        while(dwHeight)
        {
            memset(pFillPos, dwColor, dwWidth);
            pFillPos += dwPitch;
            dwHeight--;
        }
    }
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill16(char* pDst, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, DWORD dwColor)
{
    DWORD *pColor;
    char  *pFillPos;
    
    dprintf(("DDRAW:Fill16 %x (%d,%d) %d with %x \n", pDst, dwWidth, dwHeight, dwPitch, dwColor));

    dwWidth *=2;

    if(dwWidth == dwPitch)
    {
        // Width = Pitch => fill buffer so no need to take care of lines
        ddmemfill16(pDst, dwColor, dwWidth*dwHeight);
    }
    else
    {
        pFillPos = pDst;

        while(dwHeight)
        {
            ddmemfill16(pFillPos, dwColor, dwWidth);
            pFillPos += dwPitch;
            dwHeight--;
        }
    }
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill24(char* pDst, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, DWORD dwColor)
{
    DWORD *pColor;
    char  *pFillPos;
    int i;

    dprintf(("DDRAW:Fill24 %x (%d,%d) %d with %x \n", pDst, dwWidth, dwHeight, dwPitch, dwColor));

    // First Fill First row with the color

    for(i=0 ; i<dwWidth ; i++)
    {
        pColor = (DWORD*)(pDst+(i*3));
        *pColor = dwColor;
    }
    dwWidth *=3;
    dwHeight--;

    if(dwWidth == dwPitch)
    {
        // Width = Pitch => fill buffer so no need to take care of lines
        memcpy(pDst+dwPitch, pDst, dwWidth*dwHeight);
    }
    else
    {
        pFillPos = pDst+dwPitch;
        while(dwHeight)
        {
            memcpy(pFillPos,pDst,dwWidth);
            pFillPos += dwPitch;
            dwHeight--;
        }
    }
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill32(char* pDst, DWORD dwWidth, DWORD dwHeight, DWORD dwPitch, DWORD dwColor)
{
    DWORD *pColor;
    char  *pFillPos;
    
    dprintf(("DDRAW:Fill32 %x (%d,%d) %d with %x \n", pDst, dwWidth, dwHeight, dwPitch, dwColor));

    dwWidth *= 4;

    if(dwWidth == dwPitch)
    {
        // Width = Pitch => fill buffer so no need to take care of lines
        ddmemfill32(pDst, dwColor, dwWidth*dwHeight);
    }
    else
    {
        pFillPos = pDst;

        while(dwHeight)
        {
            ddmemfill32(pFillPos, dwColor, dwWidth);
            pFillPos += dwPitch;
            dwHeight--;
        }
  }
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill8on8( char *pDB,
                        char *pFB,
                        DWORD dwTop,
                        DWORD dwLeft,
                        DWORD dwWidth,
                        DWORD dwHeight,
                        DWORD dwPitchDB,
                        DWORD dwPitchFB,
                        DWORD dwColor,
                        VOID  *pPalette
                       )
{
  dprintf(("Fill8on8\n"));
  dwColor = (dwColor&0xFF)+(dwColor&0xFF<<8);
  dwColor += (dwColor<<16);

  Fill8( pDB+(dwTop*dwPitchDB)+dwLeft,
         dwWidth,
         dwHeight,
         dwPitchDB,
         dwColor);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill16on16( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                        )
{
  dprintf(("Fill16on16 %x (%d,%d)(%d,%d) %d %x", pDB+(dwTop*dwPitchDB)+(dwLeft*2), dwLeft, dwTop, dwWidth, dwHeight, dwPitchDB, dwColor));

  dwColor = (dwColor&0xFFFF)+((dwColor&0xFFFF)<<16);

  Fill16( pDB+(dwTop*dwPitchDB)+(dwLeft*2),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwColor);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill24on24( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                         )
{
  dprintf(("Fill24on24\n"));
  dwColor <<= 8;

  Fill24( pDB+(dwTop*dwPitchDB)+(dwLeft*3),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwColor);

}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill32on32( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                         )
{
  dprintf(("Fill32on32\n"));
  Fill32( pDB+(dwTop*dwPitchDB)+(dwLeft*4),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwColor);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill8on16( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         DWORD dwColor,
                         VOID  *pPalette
                        )
{
  DWORD dwCol;
  dprintf(("Fill8on16\n"));
  dwCol = (dwColor&0xFF)+(dwColor&0xFF<<8);
  dwCol += (dwCol<<16);

  Fill8( pFB+(dwTop*dwPitchFB)+dwLeft,
         dwWidth,
         dwHeight,
         dwPitchFB,
         dwCol);

  dwCol = ((WORD*)pPalette)[dwColor];
  dwCol += (dwCol<<16);
  Fill16( pDB+(dwTop*dwPitchDB)+(dwLeft*2),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwCol);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill8on24( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         DWORD dwColor,
                         VOID  *pPalette
                        )
{
  DWORD dwCol;
  dprintf(("Fill8on24\n"));
  dwCol = (dwColor&0xFF)+(dwColor&0xFF<<8);
  dwCol += (dwCol<<16);

  Fill8( pFB+(dwTop*dwPitchFB)+dwLeft,
         dwWidth,
         dwHeight,
         dwPitchFB,
         dwCol);

  dwCol = ((DWORD*)pPalette)[dwColor];
  //dwCol <<= 8;
  Fill24( pDB+(dwTop*dwPitchDB)+(dwLeft*3),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwCol);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill8on32( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         DWORD dwColor,
                         VOID  *pPalette
                        )
{
  DWORD dwCol;
  dprintf(("Fill8on32\n"));
  dwCol = (dwColor&0xFF)+(dwColor&0xFF<<8);
  dwCol += (dwCol<<16);

  Fill8( pFB+(dwTop*dwPitchFB)+dwLeft,
         dwWidth,
         dwHeight,
         dwPitchFB,
         dwCol);

  dwCol = ((DWORD*)pPalette)[dwColor];
  Fill32( pDB+(dwTop*dwPitchDB)+(dwLeft*4),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwCol);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill16on8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         DWORD dwColor,
                         VOID  *pPalette
                        )
{
    dprintf(("Fill16on8 NOT Implemented \n"));
    DebugInt3();
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill16on24( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                        )
{
  DWORD dwCol;
  dprintf(("Fill16on24\n"));
  dwCol = dwColor + (dwColor<<16);

  Fill16( pFB+(dwTop*dwPitchFB)+(dwLeft*2),
          dwWidth,
          dwHeight,
          dwPitchFB,
          dwCol);

  dwCol = ((dwColor & 0xF800)<< 16) +
          ((dwColor & 0x07E0)<< 13) +
          ((dwColor & 0x001F)<< 11);
  Fill24( pDB+(dwTop*dwPitchDB)+(dwLeft*2),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwCol);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill16on32( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                         )
{
  DWORD dwCol;
  dprintf(("Fill16on32\n"));
  dwCol = dwColor + (dwColor<<16);

  Fill16( pFB+(dwTop*dwPitchFB)+(dwLeft*2),
          dwWidth,
          dwHeight,
          dwPitchFB,
          dwCol);

  dwCol = ((dwColor & 0xF800)<< 16) +
          ((dwColor & 0x07E0)<< 13) +
          ((dwColor & 0x001F)<< 11);
  Fill32( pDB+(dwTop*dwPitchDB)+(dwLeft*4),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwCol);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill24on8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         DWORD dwColor,
                         VOID  *pPalette
                        )
{
    dprintf(("Fill24on8 NOT implemented\n"));
    DebugInt3();
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill24on16( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                        )
{
  DWORD dwCol;
  dprintf(("Fill24on16\n"));
  //dwColor <<=8;

  Fill24( pFB+(dwTop*dwPitchFB)+(dwLeft*3),
          dwWidth,
          dwHeight,
          dwPitchFB,
          dwColor);

  dwCol = ((dwColor & 0xF8000000) >>16) +
          ((dwColor & 0x00FE0000)>> 13) +
          ((dwColor & 0x0000F800)>> 11);

  dwCol = dwCol + (dwCol<<16);
  Fill16( pDB+(dwTop*dwPitchDB)+(dwLeft*2),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwCol);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill24on32( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                         )
{
  dprintf(("Fill24on32\n"));
  //dwColor <<=8;

  Fill24( pFB+(dwTop*dwPitchFB)+(dwLeft*3),
          dwWidth,
          dwHeight,
          dwPitchFB,
          dwColor);
  Fill32( pDB+(dwTop*dwPitchDB)+(dwLeft*4),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwColor);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill32on8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         DWORD dwColor,
                         VOID  *pPalette
                        )
{
    dprintf(("Fill32on8 NOT implemented\n"));
    DebugInt3();
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill32on16( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                        )
{
  DWORD dwCol;
  dprintf(("Fill32on16\n"));

  Fill32( pFB+(dwTop*dwPitchFB)+(dwLeft*4),
          dwWidth,
          dwHeight,
          dwPitchFB,
          dwColor);

  dwCol = ((dwColor & 0xF8000000) >>16) +
          ((dwColor & 0x00FE0000)>> 13) +
          ((dwColor & 0x0000F800)>> 11);

  dwCol = dwCol + (dwCol<<16);
  Fill16( pDB+(dwTop*dwPitchDB)+(dwLeft*2),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwCol);
}
//*****************************************************************************
//*****************************************************************************
void CDECL Fill32on24( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          DWORD dwColor,
                          VOID  *pPalette
                         )
{
  dprintf(("Fill32on24\n"));
  Fill32( pFB+(dwTop*dwPitchFB)+(dwLeft*4),
          dwWidth,
          dwHeight,
          dwPitchFB,
          dwColor);

  Fill24( pDB+(dwTop*dwPitchDB)+(dwLeft*3),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwColor);
}
//*****************************************************************************
//*****************************************************************************

