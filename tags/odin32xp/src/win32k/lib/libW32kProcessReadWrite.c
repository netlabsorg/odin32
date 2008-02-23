/* $Id: libW32kProcessReadWrite.c,v 1.2 2001-02-21 07:47:58 bird Exp $
 *
 * libW32kProcessReadWrite  -  Read or write to another process.
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
 * Reads or write memory in another process.
 * @returns     OS2 returncode.
 * @param       pid         Process ID which is to be written to.
 * @param       cb          Number of bytes to write.
 * @param       pvSource    Pointer to data to read.
 * @param       pvTarget    Pointer to where to write.
 * @param       fRead       TRUE:   pvSource is within pid while pvTarget is ours.
 *                          FALSE:  pvTarget is within pid while pvSource is ours.
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
APIRET APIENTRY W32kProcessReadWrite(PID pid, ULONG cb, PVOID pvSource, PVOID pvTarget, BOOL fRead)
{
    APIRET rc;

    if (fInited)
    {
        K32PROCESSREADWRITE Param;
        ULONG               cbParam = sizeof(Param);
        ULONG               cbData = 0UL;

        Param.hdr.cb    = sizeof(Param);
        Param.hdr.rc    = ERROR_NOT_SUPPORTED;
        Param.pid       = pid;
        Param.cb        = cb;
        Param.pvSource  = pvSource;
        Param.pvTarget  = pvTarget;
        Param.fRead     = fRead;

        if (usCGSelector)
            return libCallThruCallGate(K32_PROCESSREADWRITE, &Param);
        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_PROCESSREADWRITE,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);

        if (rc == NO_ERROR)
            rc = Param.hdr.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

