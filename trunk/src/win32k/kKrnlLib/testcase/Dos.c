/* $Id: Dos.c,v 1.2 2001-09-23 06:45:20 bird Exp $
 *
 * Thunkers for OS/2 APIs.
 *
 * Some APIs requires Linear Stack, some have to overridden to
 * emulate different things.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_BASE
#define INCL_NO_FAKE

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"                   /* kKrnlLib segment definitions. */

#include "testcase.h"


/*******************************************************************************
*   External Functions                                                         *
* (These are prototypes for functions in the IMPORTS section of the def-file.) *
*******************************************************************************/
APIRET  APIENTRY        _DosWrite(HFILE hFile, PVOID pBuffer, ULONG cbWrite, PULONG pcbActual);
APIRET  APIENTRY        _DosClose(HFILE hFile);
APIRET  APIENTRY        _DosOpen(
                            PSZ     pszFileName,
                            PHFILE  pHf,
                            PULONG  pulAction,
                            ULONG   cbFile,
                            ULONG   ulAttribute,
                            ULONG   fsOpenFlags,
                            ULONG   fsOpenMode,
                            PEAOP2  peaop2);
APIRET  APIENTRY        _DosSetFilePtr(
                            HFILE   hFile,
                            LONG    ib,
                            ULONG   method,
                            PULONG  ibActual);

/**
 * DosWrite overload. Ensures that the stack is 32-bit!
 */
APIRET  APIENTRY        DosWrite(HFILE hFile, PVOID pBuffer, ULONG cbWrite, PULONG pcbActual)
{
    BOOL    f32Stack = ((int)&hFile > 0x10000);
    APIRET  rc;
    if (!f32Stack)
        ThunkStack16To32();

    rc = _DosWrite(hFile, pBuffer, cbWrite, pcbActual);

    if (!f32Stack)
        ThunkStack32To16();

    return rc;
}


/**
 * DosClose overload. Ensures that the stack is 32-bit!
 */
APIRET  APIENTRY        DosClose(HFILE hFile)
{
    BOOL    f32Stack = ((int)&hFile > 0x10000);
    APIRET  rc;
    if (!f32Stack)
        ThunkStack16To32();

    rc = _DosClose(hFile);

    if (!f32Stack)
        ThunkStack32To16();

    return rc;
}


/**
 * DosOpen overload. Ensures that the stack is 32-bit!
 */
APIRET  APIENTRY        DosOpen(
                            PSZ    pszFileName,
                            PHFILE pHf,
                            PULONG pulAction,
                            ULONG  cbFile,
                            ULONG  ulAttribute,
                            ULONG  fsOpenFlags,
                            ULONG  fsOpenMode,
                            PEAOP2 peaop2)
{
    BOOL    f32Stack = ((int)&cbFile > 0x10000);
    APIRET  rc;
    if (!f32Stack)
        ThunkStack16To32();

    rc = _DosOpen(pszFileName,
                   pHf,
                   pulAction,
                   cbFile,
                   ulAttribute,
                   fsOpenFlags,
                   fsOpenMode,
                   peaop2);

    if (!f32Stack)
        ThunkStack32To16();

    return rc;
}


/**
 * DosSetFilePtr overload. Ensures that the stack is 32-bit!
 */
APIRET  APIENTRY        DosSetFilePtr(
                            HFILE   hFile,
                            LONG    ib,
                            ULONG   method,
                            PULONG  ibActual)
{
    BOOL    f32Stack = ((int)&hFile > 0x10000);
    APIRET  rc;
    if (!f32Stack)
        ThunkStack16To32();

    rc = _DosSetFilePtr(hFile,
                        ib,
                        method,
                        ibActual);

    if (!f32Stack)
        ThunkStack32To16();

    return rc;
}



