/* $Id: libWin32kSetEnvironment.c,v 1.1 2001-07-08 02:51:24 bird Exp $
 *
 * libWin32kSetEnvironment - Set the Odin32 environment block pointer for the
 *                           current process.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.no)
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
 * Sets the environment block pointer for a given process.
 * @returns     OS2 returncode.
 * @param       pszzEnvironment     Pointer to environment block.
 * @param       cchEnvironment      Size of the environment block.
 *                                  If 0 we'll do a DosQueryMem on the address
 *                                  to get the size of the memory block.
 * @param       pid                 Process id.
 * @status      completely implelemented.
 * @author      knut st. osmundsen (kosmunds@csc.no)
 */
APIRET APIENTRY  libWin32kSetEnvironment(PSZ pszzEnvironment, ULONG cchEnvironment, PID pid)
{
    APIRET rc;

    /*
     * Simple validation.
     */
    if (    pszzEnvironment < (PSZ)0x10000
        ||  pszzEnvironment >= (PSZ)0xc0000000
        ||  cchEnvironment > 1024*1024  /* Max 1MB environment. */
        ||  pid >= (PID)0x10000         /* pid range check. */
        )
        return ERROR_INVALID_PARAMETER;

    /*
     * Query object size if not given.
     */
    if (cchEnvironment == 0)
    {
        ULONG flFlags = ~0UL;
        ULONG cb = ~0UL;
        rc = DosQueryMem(pszzEnvironment, &cb, &flFlags);
        if (rc || cb <= 0x1000)                             /* DosQueryMem was broken on some Warp Fixpacks (FP9 for instance). */
            rc = DosQueryMem(pszzEnvironment, &cb, &flFlags);
        if (rc)
            return rc;
        cchEnvironment = cb;
    }

    /*
     * Check that we're initiated.
     */
    if (fInited)
    {
        /*
         * Build parameters and call win32k.
         */
        K32SETENVIRONMENT   Param;
        ULONG               cbParam = sizeof(Param);
        ULONG               cbData = 0UL;

        Param.hdr.cb            = sizeof(Param);
        Param.hdr.rc            = ERROR_NOT_SUPPORTED;
        Param.pszzEnvironment   = pszzEnvironment;
        Param.cchEnvironment    = cchEnvironment;
        Param.pid               = pid;

        if (usCGSelector)
            return libCallThruCallGate(K32_SETENVIRONMENT, &Param);
        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_SETENVIRONMENT,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);

        if (rc == NO_ERROR)
            rc = Param.hdr.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

