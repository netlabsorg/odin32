/* $Id: libDosAllocMemEx.c,v 1.1 2000-02-18 19:27:31 bird Exp $
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
#define INCL_DOSERROR
#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES
#define INCL_WIN32K_LIB


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include <win32k.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static BOOL fInited = FALSE;
HFILE       hWin32k = NULLHANDLE;




APIRET APIENTRY  DosAllocMemEx(PPVOID ppv, ULONG cb, ULONG flag)
{
    APIRET rc;

    if (fInited)
    {
        K32ALLOCMEMEX Param;
        Param.pv = *ppv;
        Param.cb = cb;
        Param.flFlags = flag;
        Param.rc = 0;
        Param.ulCS = ;
        Param.ulEIP = ;

        param.pRpInitIn = pRpIn;
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

