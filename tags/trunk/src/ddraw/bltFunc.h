/* $Id: bltFunc.h,v 1.2 1999-12-21 01:28:17 hugh Exp $ */

/*
 * Blitting functions definitions
 *
 * Copyright 1999 Markus Montkowski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


// without ColorConversion
extern void __cdecl BltSolid8to8( char *pDBDst,
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
                                 );
extern void __cdecl BltSolid16to16( char *pDBDst,
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
                                   );
extern void __cdecl BltSolid24to24( char *pDBDst,
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
                                   );
extern void __cdecl BltSolid32to32( char *pDBDst,
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
                                   );

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
                            );

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
                            );

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
                            );

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
                            );

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
                            );

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
                             );

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
                            );

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
                            );

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
                             );

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
                            );

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
                             );

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
                            );

