/* $Id: libTerm.c,v 1.1 2000-02-19 16:52:38 bird Exp $
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
#define INCL_DOSERROR
#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern static BOOL fInited;
extern HFILE       hWin32k;


/**
 * Initiates the library.
 * @returns   OS/2 return code.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
APIRET APIENTRY  libWin32kTerm(void)
{
    if (fInited)
        DosClose(hWin32k);
    hWin32k = NULLHANDLE;
    fInited = FALSE;
    return NO_ERROR;
}




