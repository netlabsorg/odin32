/* $Id: fillfunc.h,v 1.2 2001-03-18 21:44:45 mike Exp $ */

/*
 * Colorfill function definitions
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


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

