/* $Id: libW32kQuerySystemMemInfo.c,v 1.1 2001-02-11 15:24:04 bird Exp $
 *
 * libW32kQuerySystemMemInfo - Collects more or less useful information on the
 *                             memory state of the system.
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


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern BOOL     fInited;
extern HFILE    hWin32k;


/**
 * Collects more or less useful information on the memory state of the system.
 * @returns     OS2 returncode.
 * @param       pMemInfo    Pointer to memory info.
 *                          The cb data member must contain a valid value on
 *                          entry. The rest of the structure is output data
 *                          and hence will only be valid on successful return.
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      This function must be backward compatitible if changed.
 */
APIRET APIENTRY  W32kQuerySystemMemInfo(PK32SYSTEMMEMINFO pMemInfo)
{
    APIRET rc;

    if (fInited)
    {
        K32QUERYSYSTEMMEMINFO   Param;
        ULONG                   cbParam = sizeof(Param);
        ULONG                   cbData = 0UL;

        Param.pMemInfo = pMemInfo;
        Param.rc = ERROR_INVALID_PARAMETER;

        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_QUERYSYSTEMMEMINFO,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);

        if (rc == NO_ERROR)
            rc = Param.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

