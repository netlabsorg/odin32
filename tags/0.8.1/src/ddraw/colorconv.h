/* $Id: colorconv.h,v 1.2 2001-03-18 21:44:44 mike Exp $ */

/*
 * ColorConversion definitions
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


 extern void __cdecl Conv8to16( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        );

 extern void __cdecl Conv8to24( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        );
 extern void __cdecl Conv8to32( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        );

 extern void __cdecl Conv16to8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        );

 extern void __cdecl Conv16to24( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                        );
 extern void __cdecl Conv16to32( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                         );
 extern void __cdecl Conv24to8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        );
 extern void __cdecl Conv24to16( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                        );
 extern void __cdecl Conv24to32( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                         );
 extern void __cdecl Conv32to8( char *pDB,
                         char *pFB,
                         DWORD dwTop,
                         DWORD dwLeft,
                         DWORD dwWidth,
                         DWORD dwHeight,
                         DWORD dwPitchDB,
                         DWORD dwPitchFB,
                         VOID  *pPalette
                        );
 extern void __cdecl Conv32to16( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                        );
 extern void __cdecl Conv32to24( char *pDB,
                          char *pFB,
                          DWORD dwTop,
                          DWORD dwLeft,
                          DWORD dwWidth,
                          DWORD dwHeight,
                          DWORD dwPitchDB,
                          DWORD dwPitchFB,
                          VOID  *pPalette
                         );

