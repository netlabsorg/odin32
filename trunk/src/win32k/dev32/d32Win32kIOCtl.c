/* $Id: d32Win32kIOCtl.c,v 1.5 2001-02-11 15:19:47 bird Exp $
 *
 * Win32k driver IOCtl handler function.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

#define NO_WIN32K_LIB_FUNCTIONS


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"
#include "dev1632.h"
#include "dev32.h"
#include "OS2Krnl.h"
#include "Win32k.h"
#include "k32.h"



/**
 * IOCtl handler for the Win32k part of the driver.
 * @returns   Gen IOCtl return code.
 * @param     pRpIOCtl  Pointer to 32-bit request packet. (not to the original packet)
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
USHORT _loadds _Far32 _Pascal Win32kIOCtl(PRP32GENIOCTL pRpIOCtl)
{
    /* validate parameter pointer */
    if (pRpIOCtl == NULL || pRpIOCtl->ParmPacket == NULL)
        return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;

    switch (pRpIOCtl->Category)
    {
        case IOCTL_W32K_K32:
            switch (pRpIOCtl->Function)
            {
                case K32_ALLOCMEMEX:
                {
                    PK32ALLOCMEMEX pParm = (PK32ALLOCMEMEX)pRpIOCtl->ParmPacket;
                    pParm->rc = k32AllocMemEx(&pParm->pv, pParm->cb, pParm->flFlags,
                                              pParm->ulCS, pParm->ulEIP);
                    return STATUS_DONE;
                }

                case K32_QUERYOTES:
                {
                    PK32QUERYOTES pParm = (PK32QUERYOTES)pRpIOCtl->ParmPacket;
                    pParm->rc = k32QueryOTEs((HMTE)pParm->hMTE, pParm->pQOte, pParm->cbQOte);
                    return STATUS_DONE;
                }

                case K32_QUERYOPTIONSSTATUS:
                {
                    PK32QUERYOPTIONSSTATUS pParm = (PK32QUERYOPTIONSSTATUS)pRpIOCtl->ParmPacket;
                    pParm->rc = k32QueryOptionsStatus(pParm->pOptions, pParm->pStatus);
                    return STATUS_DONE;
                }

                case K32_SETOPTIONS:
                {
                    PK32SETOPTIONS pParm = (PK32SETOPTIONS)pRpIOCtl->ParmPacket;
                    pParm->rc = k32SetOptions(pParm->pOptions);
                    return STATUS_DONE;
                }

                case K32_PROCESSREADWRITE:
                {
                    PK32PROCESSREADWRITE pParm = (PK32PROCESSREADWRITE)pRpIOCtl->ParmPacket;
                    pParm->rc = k32ProcessReadWrite(pParm->pid, pParm->cb, pParm->pvSource, pParm->pvTarget, pParm->fRead);
                    return STATUS_DONE;
                }

                case K32_HANDLESYSTEMEVENT:
                {
                    PK32HANDLESYSTEMEVENT pParm = (PK32HANDLESYSTEMEVENT)pRpIOCtl->ParmPacket;
                    //pParm->rc = k32HandleSystemEvent(pParm->ulEvent, pParm->hev, pParm->fHandle);
                    pParm->rc = ERROR_NOT_SUPPORTED;
                    return STATUS_DONE;
                }

                case K32_QUERYSYSTEMMEMINFO:
                {
                    PK32QUERYSYSTEMMEMINFO pParm = (PK32QUERYSYSTEMMEMINFO)pRpIOCtl->ParmPacket;
                    pParm->rc = k32QuerySystemMemInfo(pParm->pMemInfo);
                    return STATUS_DONE;
                }

            }
            break;
    }

    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}

