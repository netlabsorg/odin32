/* $Id: libInit.c,v 1.2 2000-02-26 17:48:22 bird Exp $
 *
 * Inits the Win32k library functions.
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
BOOL    fInited = FALSE;
HFILE   hWin32k = NULLHANDLE;


/**
 * Initiates the library.
 * @returns   OS/2 return code.
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
APIRET APIENTRY  libWin32kInit(void)
{
    APIRET rc;

    if (!fInited)
    {
        ULONG ulAction = 0UL;

        rc = DosOpen("\\dev\\win32k$",
                     &hWin32k,
                     &ulAction,
                     0UL,
                     FILE_NORMAL,
                     OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                     OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                     NULL);

        fInited = rc == NO_ERROR;
    }
    else
        rc = NO_ERROR;

    return rc;
}

