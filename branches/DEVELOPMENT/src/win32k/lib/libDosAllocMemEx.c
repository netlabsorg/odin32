/* $Id: libDosAllocMemEx.c,v 1.3 2000-02-26 19:59:55 bird Exp $
 *
 * DosAllocMemEx - Extened Edition of DosAllocMem.
 *                 Allows you to suggest an address of the memory.
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
#define INCL_WIN32K_LIB


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




APIRET APIENTRY  DosAllocMemEx(PPVOID ppv, ULONG cb, ULONG flag)
{
    APIRET rc;

    if (fInited)
    {
        K32ALLOCMEMEX Param;
        ULONG         cbParam = sizeof(Param);
        ULONG         cbData = 0UL;
        Param.pv = *ppv;
        Param.cb = cb;
        Param.flFlags = flag;
        Param.rc = 0;
        Param.ulCS = libHelperGetCS();
        Param.ulEIP = *(PULONG)((int)(&ppv) - 4);

        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_ALLOCMEMEX,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);
        if (rc == NO_ERROR)
        {
            ppv = Param.pv;
            rc = Param.rc;
        }
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}
