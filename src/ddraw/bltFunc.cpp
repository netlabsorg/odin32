/* $Id: bltFunc.cpp,v 1.4 2001-04-04 09:02:14 sandervl Exp $ */

/*
 * Blitting functions
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
#include "bltFunc.h"

//
// Blt Functions
//

#ifndef USE_ASM
void __cdecl BltSolid8( char* pDst,
                        char* pSrc,
                        DWORD dwWidth,
                        DWORD dwHeight,
                        DWORD dwPitchDst,
                        DWORD dwPitchSrc)
{
  dprintf(("DDRAW:BltSolid8\n"));
  while(dwHeight)
  {
    memcpy(pDst,pSrc,dwWidth);
    pDst += dwPitchDst;
    pSrc += dwPitchSrc;
    dwHeight--;
  }
}
void __cdecl BltSolid16(char* pDst,
                        char* pSrc,
                        DWORD dwWidth,
                        DWORD dwHeight,
                        DWORD dwPitchDst,
                        DWORD dwPitchSrc)
{
  dprintf(("DDRAW:BltSolid16\n"));
  dwWidth *=2;
  while(dwHeight)
  {
    memcpy(pDst,pSrc,dwWidth);
    pDst += dwPitchDst;
    pSrc += dwPitchSrc;
    dwHeight--;
  }
}
void __cdecl BltSolid24(char* pDst,
                        char* pSrc,
                        DWORD dwWidth,
                        DWORD dwHeight,
                        DWORD dwPitchDst,
                        DWORD dwPitchSrc)
{
  dprintf(("DDRAW:BltSolid24\n"));
  dwWidth *=3;
  while(dwHeight)
  {
    memcpy(pDst,pSrc,dwWidth);
    pDst += dwPitchDst;
    pSrc += dwPitchSrc;
    dwHeight--;
  }
}
void __cdecl BltSolid32(char* pDst,
                        char* pSrc,
                        DWORD dwWidth,
                        DWORD dwHeight,
                        DWORD dwPitchDst,
                        DWORD dwPitchSrc)
{
  dprintf(("DDRAW:BltSolid32\n"));
  dwWidth *=4;
  while(dwHeight)
  {
    memcpy(pDst,pSrc,dwWidth);
    pDst += dwPitchDst;
    pSrc += dwPitchSrc;
    dwHeight--;
  }
}
#else
  #define BltSolid8  BltSolid8ASM
  #define BltSolid16 BltSolid16ASM
  #define BltSolid24 BltSolid24ASM
  #define BltSolid32 BltSolid32ASM
#endif

// without ColorConversion
void __cdecl BltSolid8to8( char *pDBDst,
                           char *pFBDst,
                           DWORD dwDstTop,
                           DWORD dwDstLeft,
                           DWORD dwPitchDBDst,
                           DWORD dwPitchFBDst,
                           char *pDBSrc,
                           char *pFBSrc,
                           DWORD dwSrcTop,
                           DWORD dwSrcLeft,
                           DWORD dwWidth,
                           DWORD dwHeight,
                           DWORD dwPitchDBSrc,
                           DWORD dwPitchFBSrc
                           )
{
  dprintf(("DDRAW:BltSolid8to8\n"));
  BltSolid8( pDBDst + dwDstLeft + (dwDstTop*dwPitchDBDst),
             pDBSrc + dwSrcLeft + (dwSrcTop*dwPitchDBSrc),
             dwWidth,
             dwHeight,
             dwPitchDBDst,
             dwPitchDBSrc);
}
void __cdecl BltSolid16to16( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                             )
{
  dprintf(("DDRAW:BltSolid16to16\n"));
  BltSolid16( pDBDst + (dwDstLeft*2) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*2) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
}
void __cdecl BltSolid24to24( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                             )
{
  dprintf(("DDRAW:BltSolid24to24\n"));
  BltSolid24( pDBDst + (dwDstLeft*3) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*3) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
}
void __cdecl BltSolid32to32( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                             )
{
  dprintf(("DDRAW:BltSolid32to32\n"));
  BltSolid32( pDBDst + (dwDstLeft*4) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*4) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
}

// With ColorConversion

// 8 to X bit
void __cdecl BltSolid8to16( char *pDBDst,
                            char *pFBDst,
                            DWORD dwDstTop,
                            DWORD dwDstLeft,
                            DWORD dwPitchDBDst,
                            DWORD dwPitchFBDst,
                            char *pDBSrc,
                            char *pFBSrc,
                            DWORD dwSrcTop,
                            DWORD dwSrcLeft,
                            DWORD dwWidth,
                            DWORD dwHeight,
                            DWORD dwPitchDBSrc,
                            DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid8to16\n"));

  BltSolid8( pFBDst + dwDstLeft + (dwDstTop*dwPitchFBDst),
             pFBSrc + dwSrcLeft + (dwSrcTop*dwPitchFBSrc),
             dwWidth,
             dwHeight,
             dwPitchFBDst,
             dwPitchFBSrc);
  BltSolid16( pDBDst + (dwDstLeft*2) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*2) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);

}

void __cdecl BltSolid8to24( char *pDBDst,
                            char *pFBDst,
                            DWORD dwDstTop,
                            DWORD dwDstLeft,
                            DWORD dwPitchDBDst,
                            DWORD dwPitchFBDst,
                            char *pDBSrc,
                            char *pFBSrc,
                            DWORD dwSrcTop,
                            DWORD dwSrcLeft,
                            DWORD dwWidth,
                            DWORD dwHeight,
                            DWORD dwPitchDBSrc,
                            DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid8to24\n"));
  BltSolid8( pFBDst + dwDstLeft + (dwDstTop*dwPitchFBDst),
             pFBSrc + dwSrcLeft + (dwSrcTop*dwPitchFBSrc),
             dwWidth,
             dwHeight,
             dwPitchFBDst,
             dwPitchFBSrc);
  BltSolid24( pDBDst + (dwDstLeft*3) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*3) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
}
void __cdecl BltSolid8to32( char *pDBDst,
                            char *pFBDst,
                            DWORD dwDstTop,
                            DWORD dwDstLeft,
                            DWORD dwPitchDBDst,
                            DWORD dwPitchFBDst,
                            char *pDBSrc,
                            char *pFBSrc,
                            DWORD dwSrcTop,
                            DWORD dwSrcLeft,
                            DWORD dwWidth,
                            DWORD dwHeight,
                            DWORD dwPitchDBSrc,
                            DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid8to32\n"));
  BltSolid8( pFBDst + dwDstLeft + (dwDstTop*dwPitchFBDst),
             pFBSrc + dwSrcLeft + (dwSrcTop*dwPitchFBSrc),
             dwWidth,
             dwHeight,
             dwPitchFBDst,
             dwPitchFBSrc);
  BltSolid32( pDBDst + (dwDstLeft*4) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*4) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
}

// 16 to X bit
void __cdecl BltSolid16to8( char *pDBDst,
                            char *pFBDst,
                            DWORD dwDstTop,
                            DWORD dwDstLeft,
                            DWORD dwPitchDBDst,
                            DWORD dwPitchFBDst,
                            char *pDBSrc,
                            char *pFBSrc,
                            DWORD dwSrcTop,
                            DWORD dwSrcLeft,
                            DWORD dwWidth,
                            DWORD dwHeight,
                            DWORD dwPitchDBSrc,
                            DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid16to8\n"));
  BltSolid8( pDBDst + dwDstLeft + (dwDstTop*dwPitchDBDst),
             pDBSrc + dwSrcLeft + (dwSrcTop*dwPitchDBSrc),
             dwWidth,
             dwHeight,
             dwPitchDBDst,
             dwPitchDBSrc);
  BltSolid16( pFBDst + (dwDstLeft*2) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*2) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}

void __cdecl BltSolid16to24( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid16to24\n"));
  BltSolid24( pDBDst + (dwDstLeft*3) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*3) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
  BltSolid16( pFBDst + (dwDstLeft*2) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*2) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}
void __cdecl BltSolid16to32( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                             )
{
  dprintf(("DDRAW:BltSolid16to32\n"));
  BltSolid32( pDBDst + (dwDstLeft*4) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*4) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
  BltSolid16( pFBDst + (dwDstLeft*2) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*2) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}

// 24 to X bit
void __cdecl BltSolid24to8( char *pDBDst,
                            char *pFBDst,
                            DWORD dwDstTop,
                            DWORD dwDstLeft,
                            DWORD dwPitchDBDst,
                            DWORD dwPitchFBDst,
                            char *pDBSrc,
                            char *pFBSrc,
                            DWORD dwSrcTop,
                            DWORD dwSrcLeft,
                            DWORD dwWidth,
                            DWORD dwHeight,
                            DWORD dwPitchDBSrc,
                            DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid24to8\n"));
  BltSolid8( pDBDst + dwDstLeft + (dwDstTop*dwPitchDBDst),
             pDBSrc + dwSrcLeft + (dwSrcTop*dwPitchDBSrc),
             dwWidth,
             dwHeight,
             dwPitchDBDst,
             dwPitchDBSrc);
  BltSolid24( pFBDst + (dwDstLeft*3) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*3) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}
void __cdecl BltSolid24to16( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid24to16\n"));
  BltSolid16( pDBDst + (dwDstLeft*2) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*2) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
  BltSolid24( pFBDst + (dwDstLeft*3) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*3) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}
void __cdecl BltSolid24to32( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                             )
{
  dprintf(("DDRAW:BltSolid24to32\n"));
  BltSolid32( pDBDst + (dwDstLeft*4) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*4) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
  BltSolid24( pFBDst + (dwDstLeft*3) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*3) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}

// 32 to X bit
void __cdecl BltSolid32to8( char *pDBDst,
                            char *pFBDst,
                            DWORD dwDstTop,
                            DWORD dwDstLeft,
                            DWORD dwPitchDBDst,
                            DWORD dwPitchFBDst,
                            char *pDBSrc,
                            char *pFBSrc,
                            DWORD dwSrcTop,
                            DWORD dwSrcLeft,
                            DWORD dwWidth,
                            DWORD dwHeight,
                            DWORD dwPitchDBSrc,
                            DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid32to8\n"));
  BltSolid8( pDBDst + dwDstLeft + (dwDstTop*dwPitchDBDst),
             pDBSrc + dwSrcLeft + (dwSrcTop*dwPitchDBSrc),
             dwWidth,
             dwHeight,
             dwPitchDBDst,
             dwPitchDBSrc);
  BltSolid32( pFBDst + (dwDstLeft*4) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*4) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}
void __cdecl BltSolid32to16( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                             )
{
  dprintf(("DDRAW:BltSolid32to16\n"));
  BltSolid16( pDBDst + (dwDstLeft*2) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*2) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
  BltSolid32( pFBDst + (dwDstLeft*4) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*4) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}

void __cdecl BltSolid32to24( char *pDBDst,
                             char *pFBDst,
                             DWORD dwDstTop,
                             DWORD dwDstLeft,
                             DWORD dwPitchDBDst,
                             DWORD dwPitchFBDst,
                             char *pDBSrc,
                             char *pFBSrc,
                             DWORD dwSrcTop,
                             DWORD dwSrcLeft,
                             DWORD dwWidth,
                             DWORD dwHeight,
                             DWORD dwPitchDBSrc,
                             DWORD dwPitchFBSrc
                            )
{
  dprintf(("DDRAW:BltSolid32to24\n"));
  BltSolid24( pDBDst + (dwDstLeft*3) + (dwDstTop*dwPitchDBDst),
              pDBSrc + (dwSrcLeft*3) + (dwSrcTop*dwPitchDBSrc),
              dwWidth,
              dwHeight,
              dwPitchDBDst,
              dwPitchDBSrc);
  BltSolid32( pFBDst + (dwDstLeft*4) + (dwDstTop*dwPitchFBDst),
              pFBSrc + (dwSrcLeft*4) + (dwSrcTop*dwPitchFBSrc),
              dwWidth,
              dwHeight,
              dwPitchFBDst,
              dwPitchFBSrc);
}

