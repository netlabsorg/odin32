/* $Id: colorconv.cpp,v 1.4 2001-03-09 22:48:47 mike Exp $ */

/*
 * ColorConversion routines
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <memory.h>
#include <misc.h>
#include "colorconv.h"

 void __cdecl Conv8to16( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        )
{
  char *pSrcLine, *pDstLine;

  dprintf(("DDRAW: Conv8to16(%d,%d ,%d,%d)\n",dwTop,dwLeft,dwWidth,dwHeight));

  pSrcLine = pFB + (dwTop*dwPitchDB) + dwLeft;
  pDstLine = pDB + (dwTop*dwPitchFB) + (dwLeft*2);

  for(;dwHeight;dwHeight--)
  {
    for(int x=0;x<dwWidth;x++)
    {
      *(((WORD*)pDstLine)+x) = ((WORD*)pPalette)[pSrcLine[x]];

    }
    pSrcLine += dwPitchFB;
    pDstLine += dwPitchDB;
  }
}

 void __cdecl Conv8to24( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        )
{
  char *pSrcLine, *pDstLine;

  dprintf(("DDRAW: Conv8to24\n"));

  pSrcLine = pFB + (dwTop*dwPitchDB) + dwLeft;
  pDstLine = pDB + (dwTop*dwPitchFB) + (dwLeft*3);

  for(;dwHeight;dwHeight--)
  {
    char *pIter;
    pIter=pDstLine;
    for(int x=0;x<dwWidth;x++)
    {
      *((DWORD*)pIter) = ((DWORD*)pPalette)[pSrcLine[x]];
      pIter+=3;
    }
    pSrcLine += dwPitchFB;
    pDstLine += dwPitchDB;
  }
}
 void __cdecl Conv8to32( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        )
{
  char *pSrcLine, *pDstLine;

  dprintf(("DDRAW: Conv8to32\n"));

  pSrcLine = pFB + (dwTop*dwPitchDB) + dwLeft;
  pDstLine = pDB + (dwTop*dwPitchFB) + (dwLeft*4);

  for(;dwHeight;dwHeight--)
  {
    for(int x=0;x<dwWidth;x++)
    {
      *(((DWORD*)pDstLine)+x) = ((DWORD*)pPalette)[pSrcLine[x]];

    }
    pSrcLine += dwPitchFB;
    pDstLine += dwPitchDB;
  }
}

 void __cdecl Conv16to8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        )
{
  dprintf(("DDRAW: Conv16to8 Not Implemented\n"));
}

 void __cdecl Conv16to24( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                        )
{
  WORD *pSrcLine;
  char *pDstLine;

  dprintf(("DDRAW: Conv16to24 (assuming R565) \n"));

  pSrcLine = (WORD*)(pFB + (dwTop*dwPitchDB) + (dwLeft*2));
  pDstLine = pDB + (dwTop*dwPitchFB) + (dwLeft*3);

  for(;dwHeight;dwHeight--)
  {
    char *pIter;
    pIter=pDstLine;
    for(int x=0;x<dwWidth;x++)
    {
      *((DWORD*)pIter) = ((pSrcLine[x] & 0xF800)<< 16) +
                         ((pSrcLine[x] & 0x07E0)<< 13) +
                         ((pSrcLine[x] & 0x001F)<< 11);
      pIter+=3;
    }
    pSrcLine = (WORD*)((char*)pSrcLine +dwPitchFB);
    pDstLine += dwPitchDB;
  }

}
 void __cdecl Conv16to32( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                         )
{
  WORD *pSrcLine;
  char *pDstLine;

  dprintf(("DDRAW: Conv16to32 (assuming R565) \n"));

  pSrcLine = (WORD*)(pFB + (dwTop*dwPitchDB) + (dwLeft*2));
  pDstLine = pDB + (dwTop*dwPitchFB) + (dwLeft*4);

  for(;dwHeight;dwHeight--)
  {

    for(int x=0;x<dwWidth;x++)
    {
      *(((DWORD*)pDstLine)+x) = ((pSrcLine[x] & 0xF800)<< 16) +
                                ((pSrcLine[x] & 0x07E0)<< 13) +
                                ((pSrcLine[x] & 0x001F)<< 11);
    }
    pSrcLine = (WORD*)((char*)pSrcLine + dwPitchFB);
    pDstLine += dwPitchDB;
  }
}

 void __cdecl Conv24to8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        )
{
  dprintf(("DDRAW: Conv24to8 Not Implmented\n"));
}
 void __cdecl Conv24to16( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                        )
{
  char *pSrcLine;
  char *pDstLine;

  dprintf(("DDRAW: Conv24to16 (assuming R565) \n"));

  pSrcLine = pFB + (dwTop*dwPitchDB) + (dwLeft*3);
  pDstLine = pDB + (dwTop*dwPitchFB) + (dwLeft*2);

  for(;dwHeight;dwHeight--)
  {

    for(int x=0;x<dwWidth;x++)
    {
      *((WORD*)pDstLine[x]) = ((pSrcLine[x*3] & 0x1F)<< 11) +
                              ((pSrcLine[x*3+1] & 0x3F)<< 5) +
                              ((pSrcLine[x*3+2] & 0x1F));
    }
    pSrcLine += dwPitchFB;
    pDstLine += dwPitchDB;
  }
}

 void __cdecl Conv24to32( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                         )
{
  char *pSrcLine;
  char *pDstLine;


  dprintf(("DDRAW: Conv24to32 Assuming RGBA \n"));

  pSrcLine = pFB + (dwTop*dwPitchDB) + (dwLeft*3);;
  pDstLine = pDB + (dwTop*dwPitchFB) + (dwLeft*4);

  for(;dwHeight;dwHeight--)
  {
    char *pIter;
    pIter=pDstLine;
    for(int x=0;x<dwWidth;x++)
    {
      *((DWORD*)pDstLine[x]) = *((DWORD*)pIter) & 0xffffff00;
      pIter +=3;
    }
    pSrcLine += dwPitchFB;
    pDstLine += dwPitchDB;
  }

}

 void __cdecl Conv32to8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        )
{
  dprintf(("DDRAW: Conv32to8 Not Implemented\n"));
}
 void __cdecl Conv32to16( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                        )
{
  DWORD *pSrcLine;
  WORD  *pDstLine;

  dprintf(("DDRAW: Conv32to16 (assuming R565)"));

  pSrcLine = (DWORD*)(pFB + (dwTop*dwPitchDB) + (dwLeft*4));
  pDstLine = (WORD*)(pDB + (dwTop*dwPitchFB) + (dwLeft*2));

  for(;dwHeight;dwHeight--)
  {

    for(int x=0;x<dwWidth;x++)
    {
      DWORD tmp;
      tmp         = ((pSrcLine[x] & 0xF8000000) >> 16) |
                    ((pSrcLine[x] & 0x00FC0000) >> 13) |
                    ((pSrcLine[x] & 0x0000F800) >> 11);
      pDstLine[x] = (WORD)tmp;
    }
    pSrcLine = (DWORD*)((char*)pSrcLine + dwPitchFB);
    pDstLine = (WORD*)((char*)pDstLine + dwPitchDB);
  }
}

 void __cdecl Conv32to24( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                         )
{
  char *pSrcLine;
  char *pDstLine;


  dprintf(("DDRAW: Conv32to24 Assuming RGBA \n"));

  pSrcLine = pFB + (dwTop*dwPitchDB) + (dwLeft*3);
  pDstLine = pDB + (dwTop*dwPitchFB) + (dwLeft*4);

  for(;dwHeight;dwHeight--)
  {
    char *pIter;
    pIter=pDstLine;
    for(int x=0;x<dwWidth;x++)
    {
      *((DWORD*)pIter[x]) = *((DWORD*)pSrcLine[x]);
      pIter +=3;
    }
    pSrcLine += dwPitchFB;
    pDstLine += dwPitchDB;
  }

}

