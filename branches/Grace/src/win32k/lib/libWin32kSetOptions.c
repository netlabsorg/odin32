/* $Id: libWin32kSetOptions.c,v 1.1.2.1 2000-09-02 20:42:54 bird Exp $
 *
 * libWin32kSetOptions - Sets the changable options of win32k.sys the options.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern BOOL     fInited;
extern HFILE    hWin32k;

#include "log.h"

/**
 * Gets the options settings and/or the status of win32k.sys.
 * @returns     OS2 returncode.
 * @param       pOptions    Pointer to an options struct. (NULL is allowed)
 *                          (cb have to be set to the size of the structure.)
 * @param       pStatus     Pointer to a status struct. (NULL is allowed)
 *                          (cb have to be set to the size of the structure.)
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
APIRET APIENTRY  libWin32kSetOptions(PK32OPTIONS pOptions)
{
    APIRET rc;

    /*
     * Simple validation.
     */
    if (pOptions == NULL || pOptions->cb != sizeof(K32OPTIONS))
        rc = ERROR_INVALID_PARAMETER;

    /*
     * Check that we're initiated.
     */
    else if (fInited)
    {
        /*
         * Build parameters and call win32k.
         */
        K32SETOPTIONS   Param;
        ULONG           cbParam = sizeof(Param);
        ULONG           cbData = 0UL;

        Param.pOptions  = pOptions;
        Param.rc = ERROR_INVALID_PARAMETER;

        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_SETOPTIONS,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);

        if (rc == NO_ERROR)
            rc = Param.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

