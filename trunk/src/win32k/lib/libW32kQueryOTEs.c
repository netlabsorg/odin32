/* $Id: libW32kQueryOTEs.c,v 1.3 2001-02-21 07:47:59 bird Exp $
 *
 * libW32kQueryOTEs - Get's the object table entries (OTEs) for a given
 *                    module (given by a module handle).
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
 * Gets the object table entries for a module.
 * @returns     OS2 returncode.
 * @param       hMTE    Module handle (HMTE) of the module.
 * @param       pQOte   Pointer to output buffer.
 * @param       cbQOte  Size (in bytes) of the output buffer.
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
APIRET APIENTRY  W32kQueryOTEs(HMODULE hMTE, PQOTEBUFFER pQOte, ULONG cbQOte)
{
    APIRET rc;

    if (fInited)
    {
        K32QUERYOTES    Param;
        ULONG           cbParam = sizeof(Param);
        ULONG           cbData = 0UL;

        Param.hdr.cb = sizeof(Param);
        Param.hdr.rc = ERROR_NOT_SUPPORTED;
        Param.hMTE   = hMTE;
        Param.pQOte  = pQOte;
        Param.cbQOte = cbQOte;

        if (usCGSelector)
            return libCallThruCallGate(K32_QUERYOTES, &Param);
        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_QUERYOTES,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);

        if (rc == NO_ERROR)
            rc = Param.hdr.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

