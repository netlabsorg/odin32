/* $Id: libWin32kQueryOptionsStatus.c,v 1.3 2001-02-21 07:47:59 bird Exp $
 *
 * libWin32kQueryOptionsStatus - Queries the options and/or the status of
 *                               Win32k.sys driver.
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
APIRET APIENTRY  libWin32kQueryOptionsStatus(PK32OPTIONS pOptions, PK32STATUS pStatus)
{
    APIRET rc;

    /*
     * Simple validation.
     */
    if ((pOptions != NULL && pOptions->cb != sizeof(K32OPTIONS))
        ||
        (pStatus  != NULL && pStatus->cb != sizeof(K32STATUS))
        ||
        (pOptions == NULL && pStatus == NULL)
        )
        rc = ERROR_INVALID_PARAMETER;

    /*
     * Check that we're initiated.
     */
    else if (fInited)
    {
        /*
         * Build parameters and call win32k.
         */
        K32QUERYOPTIONSSTATUS   Param;
        ULONG           cbParam = sizeof(Param);
        ULONG           cbData = 0UL;

        Param.hdr.cb    = sizeof(Param);
        Param.hdr.rc    = ERROR_NOT_SUPPORTED;
        Param.pOptions  = pOptions;
        Param.pStatus   = pStatus;

        if (usCGSelector)
            return libCallThruCallGate(K32_QUERYOPTIONSSTATUS, &Param);
        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_QUERYOPTIONSSTATUS,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);

        if (rc == NO_ERROR)
            rc = Param.hdr.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

