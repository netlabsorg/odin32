/* $Id: fillfunc.h,v 1.3 2002-07-01 19:15:26 sandervl Exp $ */

/*
 * Colorfill function definitions
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

//RGB 555 -> RGB 565
#define MAKE_RGB565(a)  (((a<<1)& 0xFFC0) | (a & 0x1F))


// without ColorConversion
extern void __cdecl Fill8on8( char *pDBDst,
                              char *pFBDst,
                              DWORD dwDstTop,
                              DWORD dwDstLeft,
                              DWORD dwWidth,
                              DWORD dwHeight,
                              DWORD dwPitchDBDst,
                              DWORD dwPitchFBDst,
                              DWORD dwColor,
                              VOID  *pPalette
                             );

extern void __cdecl Fill16on16( char *pDBDst,
                                char *pFBDst,
                                DWORD dwDstTop,
                                DWORD dwDstLeft,
                                DWORD dwWidth,
                                DWORD dwHeight,
                                DWORD dwPitchDBDst,
                                DWORD dwPitchFBDst,
                                DWORD dwColor,
                                VOID  *pPalette
                              );
extern void __cdecl Fill24on24( char *pDBDst,
                                char *pFBDst,
                                DWORD dwDstTop,
                                DWORD dwDstLeft,
                                DWORD dwWidth,
                                DWORD dwHeight,
                                DWORD dwPitchDBDst,
                                DWORD dwPitchFBDst,
                                DWORD dwColor,
                                VOID  *pPalette
                               );
extern void __cdecl Fill32on32( char *pDBDst,
                                char *pFBDst,
                                DWORD dwDstTop,
                                DWORD dwDstLeft,
                                DWORD dwWidth,
                                DWORD dwHeight,
                                DWORD dwPitchDBDst,
                                DWORD dwPitchFBDst,
                                DWORD dwColor,
                                VOID  *pPalette
                               );

extern void __cdecl Fill8on16( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

extern void __cdecl Fill8on24( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

extern void __cdecl Fill8on32( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

extern void __cdecl Fill16on8( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

extern void __cdecl Fill16on24( char *pDBDst,
                                char *pFBDst,
                                DWORD dwDstTop,
                                DWORD dwDstLeft,
                                DWORD dwWidth,
                                DWORD dwHeight,
                                DWORD dwPitchDBDst,
                                DWORD dwPitchFBDst,
                                DWORD dwColor,
                                VOID  *pPalette
                              );

extern void __cdecl Fill16on32( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

extern void __cdecl Fill24on8( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

extern void __cdecl Fill24on16( char *pDBDst,
                                char *pFBDst,
                                DWORD dwDstTop,
                                DWORD dwDstLeft,
                                DWORD dwWidth,
                                DWORD dwHeight,
                                DWORD dwPitchDBDst,
                                DWORD dwPitchFBDst,
                                DWORD dwColor,
                                VOID  *pPalette
                              );

extern void __cdecl Fill24on32( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

extern void __cdecl Fill32on8( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

extern void __cdecl Fill32on16( char *pDBDst,
                                char *pFBDst,
                                DWORD dwDstTop,
                                DWORD dwDstLeft,
                                DWORD dwWidth,
                                DWORD dwHeight,
                                DWORD dwPitchDBDst,
                                DWORD dwPitchFBDst,
                                DWORD dwColor,
                                VOID  *pPalette
                              );

extern void __cdecl Fill32on24( char *pDBDst,
                               char *pFBDst,
                               DWORD dwDstTop,
                               DWORD dwDstLeft,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               DWORD dwPitchDBDst,
                               DWORD dwPitchFBDst,
                               DWORD dwColor,
                               VOID  *pPalette
                              );

