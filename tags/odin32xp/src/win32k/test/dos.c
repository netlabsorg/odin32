/* $Id: dos.c,v 1.3 2000-12-11 06:53:57 bird Exp $
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

#include "devSegDf.h"                   /* Win32k segment definitions. */

#include "test.h"


/*******************************************************************************
*   External Functions                                                         *
* (These are prototypes for functions in the IMPORTS section of the def-file.) *
*******************************************************************************/
APIRET  APIENTRY        _DosWrite(HFILE hFile, PVOID pBuffer, ULONG cbWrite, PULONG pcbActual);


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

