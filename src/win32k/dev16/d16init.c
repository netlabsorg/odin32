/* $Id: d16init.c,v 1.1 1999-09-06 02:19:55 bird Exp $
 *
 * d16init - init routines for both drivers.
 *
 * IMPORTANT! Code and data defined here will be discarded after init is
 *            compleated. CodeEnd and DataEnd should not be set here.?
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_16
#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES
#define INCL_DOSMISC
#define INCL_DOSERRORS
#define INCL_NOPMAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <devhdr.h>
#include <devcmd.h>
#include <strat2.h>
#include <reqpkt.h>
#include <dhcalls.h>

#include <string.h>
#include <memory.h>

#include "dev1632.h"
#include "dev16.h"


/**
 * init function - device 0.
 * Does nothing else than setting the device_help variable and
 * fill the request packet.
 * @returns   Status word.
 * @param     pRpIn   Pointer to input request packet.  Actually the same memory as pRpOut but another struct.
 * @param     pRpOut  Pointer to output request packet. Actually the same memory as pRpIn  but another struct.
 * @remark    pRpIn and pRpOut points to the same memory.
 */
USHORT _near dev0Init(PRPINITIN pRpIn, PRPINITOUT pRpOut)
{
    Device_Help = pRpIn->DevHlpEP;

    pRpOut->BPBArray = NULL;
    pRpOut->CodeEnd  = (USHORT)&CODE16END;
    pRpOut->DataEnd  = (USHORT)&DATA16END;
    pRpOut->Unit     = 0;
    pRpOut->rph.Status = STATUS_DONE;
    return STATUS_DONE;
}


/**
 * init function - device 1.
 * We will send an IOCtl request to the win32i$ (device 0) which will
 * perform the Ring-0 initiation of the driver.
 * @returns   Status word.
 * @param     pRpIn   Pointer to input request packet.  Actually the same memory as pRpOut but another struct.
 * @param     pRpOut  Pointer to output request packet. Actually the same memory as pRpIn  but another struct.
 * @remark    pRpIn and pRpOut points to the same memory.
 */
USHORT _near dev1Init(PRPINITIN pRpIn, PRPINITOUT pRpOut)
{
    APIRET          rc;
    D16R0INITPARAM  param;
    D16R0INITDATA   data = {0};
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;
    NPSZ            npszErrMsg = NULL;

    rc = DosOpen("\\dev\\win32i$", &hDev0, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
    if (rc == NO_ERROR)
    {
        param.pRpInitIn = pRpIn;
        rc = DosDevIOCtl(&data, &param, D16_IOCTL_RING0INIT, D16_IOCTL_CAT, hDev0);
/*        _asm int 3; */
        if (rc == NO_ERROR)
        {
            if (data.Status != STATUS_DONE)
                npszErrMsg = "Ring-0 initiation failed\n\r";
            else
            {
                register NPSZ npsz = "Test.sys succesfully initiated!\n\r";
                DosPutMessage(1, strlen(npsz)+1, npsz);
                pRpOut->Status = data.Status;
            }
        }
        else
            npszErrMsg = "DosDevIOCtl failed\n\r";
        DosClose(hDev0);
    }
    else
        npszErrMsg = "DosOpen failed\n\r";

    pRpOut->BPBArray = NULL;
    pRpOut->CodeEnd = (USHORT)&CODE16END;
    pRpOut->DataEnd = (USHORT)&DATA16END;
    pRpOut->Unit    = 0;

    if (npszErrMsg)
    {
        DosPutMessage(1, strlen(npszErrMsg) + 1, npszErrMsg);
        return pRpOut->rph.Status = STATUS_DONE | STERR | ERROR_I24_QUIET_INIT_FAIL;
    }

    return pRpOut->rph.Status;
}



/**
 * R0 16-bit initiation function.
 * This gets TKSSBase, thunks parameters and calls R0 32-bit initiation function.
 * @returns   Status word.
 * @param     pRp  Generic IO Control request packet.
 */
USHORT R0Init16(PRP_GENIOCTL pRp)
{
    USHORT usRc = STATUS_DONE;
    APIRET rc;
    PDOSTABLE  pDT;
    PDOSTABLE2 pDT2;

    /* First we're to get the DosTable2 stuff. */
    rc = DevHelp_GetDOSVar(9, 0, &pDT);
    if (rc == NO_ERROR)
    {
        ULONG cPages;
        char  hLockParm[12] = {0};
        ULONG ulLinParm;
        char  hLockData[12] = {0};
        ULONG ulLinData;

        pDT2 = (PDOSTABLE2)((char FAR *)pDT + pDT->cul*4 + 1);
        TKSSBase16 = pDT2->TKSSBase;
        R0FlatCS16 = (USHORT)pDT2->R0FlatCS;
        R0FlatDS16 = (USHORT)pDT2->R0FlatDS;
        if (!DevHelp_VirtToLin(SELECTOROF(pRp->ParmPacket), OFFSETOF(pRp->ParmPacket), &ulLinParm)
            &&
            !DevHelp_VirtToLin(SELECTOROF(pRp->DataPacket), OFFSETOF(pRp->DataPacket), &ulLinData)
            )
        {
            if (!(rc = DevHelp_VMLock(VMDHL_LONG | VMDHL_WRITE | VMDHL_VERIFY,
                                ulLinParm, sizeof(D16R0INITPARAM),
                                (LIN)~0UL, SSToDS_16(&hLockParm[0]), &cPages))
                &&
                !DevHelp_VMLock(VMDHL_LONG | VMDHL_WRITE | VMDHL_VERIFY,
                                ulLinData, sizeof(D16R0INITDATA),
                                (LIN)~0UL, SSToDS_16(&hLockData[0]), &cPages)
                )
            {   /* data and param is locked (do we need to lock the request packet too ?). */
                /* create new 32-bit packet */
                RP32INIT rp32init;

                _fmemcpy(&rp32init, ((PD16R0INITPARAM)pRp->ParmPacket)->pRpInitIn, sizeof(RPINITIN));
                if (((PD16R0INITPARAM)pRp->ParmPacket)->pRpInitIn->InitArgs == NULL ||
                    !DevHelp_VirtToLin(SELECTOROF(rp32init.InitArgs), OFFSETOF(rp32init.InitArgs), (PLIN)&rp32init.InitArgs)
                   )
                {
                    /*_asm int 3;*/
                    usRc = CallR0Init32(SSToDS_16(&rp32init));
                    /*_asm int 3;*/
                }
                else
                    usRc |= ERROR_I24_INVALID_PARAMETER;

             ((PD16R0INITDATA)pRp->DataPacket)->Status = usRc;

                /* finished - unlock data and parm */
                DevHelp_VMUnLock((LIN)SSToDS_16(&hLockParm[0]));
                DevHelp_VMUnLock((LIN)SSToDS_16(&hLockData[0]));
            }
            else
                usRc |= ERROR_I24_INVALID_PARAMETER;
        }
        else
            usRc |= ERROR_I24_INVALID_PARAMETER;
    }
    else
        usRc |= ERROR_I24_GEN_FAILURE;


    #if 0
    rc = DevHelp_VMLock(VMDHL_LONG | VMDHL_WRITE | VMDHL_VERIFY,
                        &DATA32START,
                        (ULONG)(&end) - (ULONG)&DATA32START),
                        (void*)-1,
                        &lock[0],
                        &cPages);

    rc = DevHelp_VMLock(VMDHL_LONG | VMDHL_VERIFY,
                        &CODE32START,
                        (ULONG)(&CODE32END) - (ULONG)&CODE32START),
                        (void*)-1,
                        &lock[0],
                        &cPages);
    #endif

    return usRc;
}
