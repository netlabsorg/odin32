/* $Id: libDosAllocMemEx.c,v 1.8 2001-02-21 07:47:58 bird Exp $
 *
 * DosAllocMemEx - Extened Edition of DosAllocMem.
 *                 Allows you to suggest an address of the memory.
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



APIRET APIENTRY  DosAllocMemEx(PPVOID ppv, ULONG cb, ULONG flag)
{
    APIRET rc;

    if (fInited)
    {
        K32ALLOCMEMEX Param;
        ULONG         cbParam = sizeof(Param);
        ULONG         cbData = 0UL;

        Param.hdr.cb = sizeof(Param);
        Param.hdr.rc = ERROR_NOT_SUPPORTED;
        Param.ppv = ppv;
        Param.cb = cb;
        Param.flFlags = flag;
        Param.ulCS = libHelperGetCS();
        Param.ulEIP = *(PULONG)((int)(&ppv) - 4);

        if (usCGSelector)
            return libCallThruCallGate(K32_ALLOCMEMEX, &Param);
        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_ALLOCMEMEX,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);
        if (rc == NO_ERROR)
            rc = Param.hdr.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

