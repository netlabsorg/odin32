/* $Id: libInit.c,v 1.4 2001-02-21 07:47:58 bird Exp $
 *
 * Inits the Win32k library functions.
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

#include "win32k.h"
#include "libPrivate.h"

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
BOOL    fInited = FALSE;
HFILE   hWin32k = NULLHANDLE;


/**
 * Initiates the library.
 * @returns   OS/2 return code.
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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

        /*
         * Get the callgate selector.
         */
        if (fInited)
        {
            K32QUERYCALLGATE    Param;
            ULONG               cbParam = sizeof(Param);
            ULONG               cbData = 0UL;

            Param.hdr.cb    = sizeof(Param);
            Param.hdr.rc    = ERROR_NOT_SUPPORTED;
            Param.pusCGSelector = &usCGSelector;

            rc = DosDevIOCtl(hWin32k,
                             IOCTL_W32K_K32,
                             K32_QUERYCALLGATE,
                             &Param, sizeof(Param), &cbParam,
                             "", 1, &cbData);
            if (rc != NO_ERROR || Param.hdr.rc != NO_ERROR)
            {
                usCGSelector = 0;       /* Just to be 100% it isn't set on a failure */
                                        /* since we checks if it's 0 to see if it's usable */
                rc = NO_ERROR;          /* This isn't a fatal error, we may still use the IOCtls. */
            }
        }
    }
    else
        rc = NO_ERROR;

    return rc;
}

