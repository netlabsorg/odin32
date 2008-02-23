/* $Id: lz32.h,v 1.4 1999-08-17 16:58:34 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Includefile for the decompression library, lzexpand
 *
 * Copyright 1996 Marcus Meissner
 * Copyright 1999 Patrick Haller
 */

#ifndef __WINE_LZEXPAND_H
#define __WINE_LZEXPAND_H

#include "windef.h"

#ifdef __cplusplus
  extern "C" {
#endif /* defined(__cplusplus) */


/****************************************************************************
 * Defines                                                                  *
 ****************************************************************************/

#define LZERROR_BADINHANDLE              -1 /* -1 */
#define LZERROR_BADOUTHANDLE             -2 /* -2 */
#define LZERROR_READ                     -3 /* -3 */
#define LZERROR_WRITE                    -4 /* -4 */
#define LZERROR_GLOBALLOC                -5 /* -5 */
#define LZERROR_GLOBLOCK                 -6 /* -6 */
#define LZERROR_BADVALUE                 -7 /* -7 */
#define LZERROR_UNKNOWNALG               -8 /* -8 */


/****************************************************************************
 * Conditional Defines                                                      *
 ****************************************************************************/

#ifndef SEEK_SET
#define SEEK_SET   0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR   1
#endif

#ifndef SEEK_END
#define SEEK_END   2
#endif


/****************************************************************************
 * Prototypes                                                               *
 ****************************************************************************/

// external interface
VOID        WIN32API LZDone(void);
LONG        WIN32API CopyLZFile(HFILE,HFILE);
HFILE       WIN32API LZOpenFileA(LPCSTR,LPOFSTRUCT,UINT);
HFILE       WIN32API LZOpenFileW(LPCWSTR,LPOFSTRUCT,UINT);
INT         WIN32API LZRead(HFILE,LPVOID,UINT);
INT         WIN32API LZStart(void);
void        WIN32API LZClose(HFILE);
LONG        WIN32API LZCopy(HFILE,HFILE);
HFILE       WIN32API LZInit(HFILE);
LONG        WIN32API LZSeek(HFILE,LONG,INT);
INT         WIN32API GetExpandedNameA(LPCSTR,LPSTR);
INT         WIN32API GetExpandedNameW(LPCWSTR,LPWSTR);

#ifdef __cplusplus
  } /* extern "C" */
#endif /* defined(__cplusplus) */

#endif  /* __WINE_LZEXPAND_H */

