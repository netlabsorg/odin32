/* $Id: d32Win32kIOCtl.c,v 1.1 2000-02-15 23:39:19 bird Exp $
 *
 * Win32k driver IOCtl handler function.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "dev1632.h"
#include "dev32.h"
#include "Win32k.h"
#include "k32.h"



/**
 * IOCtl handler for the Win32k part of the driver.
 * @returns   Gen IOCtl return code.
 * @param     pRpIOCtl  Pointer to 32-bit request packet. (not to the original packet)
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
USHORT _loadds _Far32 _Pascal Win32kIOCtl(PRP32GENIOCTL pRpIOCtl)
{
    switch (pRpIOCtl->Category)
    {
        case IOCTL_W32K_K32:
            switch (pRpIOCtl->Function)
            {
                case K32_ALLOCMEMEX:
                {
                    PK32ALLOCMEMEX pParm = (PK32ALLOCMEMEX)pRpIOCtl->ParmPacket;
                    if (pParm == NULL)
                        return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
                    pParm->rc = k32AllocMemEx(&pParm->pv, pParm->cb, pParm->flFlags,
                                              pParm->ulCS, pParm->ulEIP);
                    return STATUS_DONE;
                }
            }
            break;
    }

    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}

