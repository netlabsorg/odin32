/* $Id: libW32kQuerySystemMemInfo.c,v 1.2 2001-02-21 07:47:59 bird Exp $
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
#include "libPrivate.h"


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

        Param.hdr.cb    = sizeof(Param);
        Param.hdr.rc    = ERROR_NOT_SUPPORTED;
        Param.pMemInfo  = pMemInfo;

        if (usCGSelector)
            return libCallThruCallGate(K32_QUERYSYSTEMMEMINFO, &Param);
        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_QUERYSYSTEMMEMINFO,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);

        if (rc == NO_ERROR)
            rc = Param.hdr.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

