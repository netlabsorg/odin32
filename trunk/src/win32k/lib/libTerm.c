/* $Id: libTerm.c,v 1.2 2000-02-26 17:48:23 bird Exp $
 *
 * Terminates the Win32k library functions.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
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


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern BOOL        fInited;
extern HFILE       hWin32k;


/**
 * Initiates the library.
 * @returns   OS/2 return code.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
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




