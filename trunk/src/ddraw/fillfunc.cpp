/* $Id: fillfunc.cpp,v 1.3 2001-03-18 21:44:44 mike Exp $ */

/*
 * ColorFill functions
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <memory.h>
#include <misc.h>
#include "fillfunc.h"

//
// Blt Functions
//

#ifndef USE_ASM
void __cdecl Fill8( char* pDst,
                    DWORD dwWidth,
                    DWORD dwHeight,
                    DWORD dwPitch,
                    DWORD dwColor)
{
  DWORD *pColor;
  char  *pFillPos;
  int i;
  dprintf(("DDRAW:Fill8 with %08X\n",dwColor));

  // First Fill First row with the color

  for( i=0,pColor = (DWORD*)pDst;i<(dwWidth/4);i++)
    pColor[i] = dwColor;

  if(dwWidth % 4)
  {
    pFillPos = (char*) (&pColor[i-1]);
    for(i=0;i<dwWidth % 4;i++)
      pFillPos[i] = (UCHAR) dwColor;
  }

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
void __cdecl Fill16( char* pDst,
                     DWORD dwWidth,
                     DWORD dwHeight,
                     DWORD dwPitch,
                     DWORD dwColor)
{
  DWORD *pColor;
  char  *pFillPos;
  int i;
  dprintf(("DDRAW:Fill16 with %08X \n",dwColor));

  // First Fill First row with the color

  for( i=0,pColor = (DWORD*)pDst;i<(dwWidth/2);i++)
    pColor[i] = dwColor;

  if(dwWidth % 2)
  {
     pFillPos = (char*)(&pColor[i-1]);
    *((USHORT*)pFillPos) = (USHORT)dwColor;
  }

  dwWidth *=2;
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
void __cdecl Fill24( char* pDst,
                     DWORD dwWidth,
                     DWORD dwHeight,
                     DWORD dwPitch,
                     DWORD dwColor)
{
  DWORD *pColor;
  char  *pFillPos;
  int i;
  dprintf(("DDRAW:Fill24 with %08X \n",dwColor));

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

void __cdecl Fill32( char* pDst,
                     DWORD dwWidth,
                     DWORD dwHeight,
                     DWORD dwPitch,
                     DWORD dwColor)
{
  DWORD *pColor;
  char  *pFillPos;
  int i;
  dprintf(("DDRAW:Fill24 with %08X \n",dwColor));

  // First Fill First row with the color

  for(i=0 ; i<dwWidth ; i++)
  {
    pColor[i] = dwColor;
  }
  dwWidth *=4;
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
#else
  #define Fill8  Fill8ASM
  #define Fill16 Fill16ASM
  #define Fill24 Fill24ASM
  #define Fill32 Fill32ASM
#endif



// without ColorConversion
 void __cdecl Fill8on8( char *pDB,
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
 void __cdecl Fill16on16( char *pDB,
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
  dprintf(("Fill16on16\n"));
  dwColor = (dwColor&0xFFFF)+((dwColor&0xFFFF)<<16);

  Fill16( pDB+(dwTop*dwPitchDB)+(dwLeft*2),
          dwWidth,
          dwHeight,
          dwPitchDB,
          dwColor);
}
 void __cdecl Fill24on24( char *pDB,
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
 void __cdecl Fill32on32( char *pDB,
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

 void __cdecl Fill8on16( char *pDB,
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

 void __cdecl Fill8on24( char *pDB,
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
 void __cdecl Fill8on32( char *pDB,
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
 void __cdecl Fill16on8( char *pDB,
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
}
 void __cdecl Fill16on24( char *pDB,
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
 void __cdecl Fill16on32( char *pDB,
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

 void __cdecl Fill24on8( char *pDB,
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
}
 void __cdecl Fill24on16( char *pDB,
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
 void __cdecl Fill24on32( char *pDB,
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
 void __cdecl Fill32on8( char *pDB,
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
}
 void __cdecl Fill32on16( char *pDB,
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
 void __cdecl Fill32on24( char *pDB,
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

