/* $Id: libTerm.c,v 1.4 2001-02-21 07:47:58 bird Exp $
 *
 * Terminates the Win32k library functions.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include "libPrivate.h"


/**
 * Initiates the library.
 * @returns   OS/2 return code.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
APIRET APIENTRY  libWin32kTerm(void)
{
    APIRET rc = NO_ERROR;

    if (fInited)
        rc = DosClose(hWin32k);

    if (rc == NO_ERROR)
    {
        hWin32k = NULLHANDLE;
        fInited = FALSE;
    }
    return rc;
}




