/* $Id: libDosKillProcessEx.c,v 1.1 2001-07-09 23:48:52 bird Exp $
 *
 * DosKillProcessEx - Extened Edition of DosKillProcess.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
 * Extended DosKillProcess API.
 * @returns NO_ERROR on success.
 *          ERROR_SIGNAL_PENDING returned if a kill signal is allready pending.
 *          ERROR_INIT_ROUTINE_FAILED if library isn't initiated.
 *          (Try set DKP_FLAG_KILL9.)
 *          other error code..
 * @param   flAction    Action and flags defined in os2 headers and win32k.h
 * @param   pid         Pid to kill or top of process tree to kill.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
APIRET APIENTRY  DosKillProcessEx(ULONG flAction, PID pid)
{
    APIRET rc;

    if (fInited)
    {
        K32KILLPROCESSEX    Param;
        ULONG               cbParam = sizeof(Param);
        ULONG               cbData = 0UL;

        Param.hdr.cb = sizeof(Param);
        Param.hdr.rc = ERROR_NOT_SUPPORTED;
        Param.flAction = flAction;
        Param.pid = pid;

        if (usCGSelector)
            return libCallThruCallGate(K32_KILLPROCESSEX, &Param);
        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_KILLPROCESSEX,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);
        if (rc == NO_ERROR)
            rc = Param.hdr.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

