/* $Id: d16init.c,v 1.5 2000-02-21 04:45:45 bird Exp $
 *
 * d16init - init routines for both drivers.
 *
 * IMPORTANT! Code and data defined here will be discarded after init is
 *            compleated. CodeEnd and DataEnd should not be set here.?
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
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

#include "probkrnl.h"
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
USHORT NEAR dev0Init(PRPINITIN pRpIn, PRPINITOUT pRpOut)
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
 * We will send an IOCtl request to the elf$ (device 0) which will
 * perform the Ring-0 initiation of the driver.
 * @returns   Status word.
 * @param     pRpIn   Pointer to input request packet.  Actually the same memory as pRpOut but another struct.
 * @param     pRpOut  Pointer to output request packet. Actually the same memory as pRpIn  but another struct.
 * @remark    pRpIn and pRpOut points to the same memory.
 */
USHORT NEAR dev1Init(PRPINITIN pRpIn, PRPINITOUT pRpOut)
{
    APIRET          rc;
    D16R0INITPARAM  param;
    D16R0INITDATA   data = {0};
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;
    NPSZ            npszErrMsg = NULL;

    rc = ProbeKernel(pRpIn);
    if (rc == NO_ERROR)
    {
        rc = DosOpen("\\dev\\elf$", &hDev0, &usAction, 0UL, FILE_NORMAL,
                     OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                     OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                     0UL);
        if (rc == NO_ERROR)
        {
            param.pRpInitIn = pRpIn;
            rc = DosDevIOCtl(&data, &param, D16_IOCTL_RING0INIT, D16_IOCTL_CAT, hDev0);
            if (rc == NO_ERROR)
            {
                if (data.Status != STATUS_DONE)
                    npszErrMsg = "Ring-0 initiation failed\n\r";
                else
                {
                    /* FIXME quiet test! */
                    register NPSZ npsz = "Win32k.sys succesfully initiated!\n\r";
                    DosPutMessage(1, strlen(npsz)+1, npsz);
                    pRpOut->Status = data.Status;
                }
            }
            else
            {
                APIRET rc2 = rc;
                NPSZ   npsz;
                            /*0123456789012345678901234567890 1*/
                npszErrMsg = "DosDevIOCtl failed. rc=       \n\r";

                npsz  = &npszErrMsg[29];
                do
                {
                    *npsz-- = (char)((rc2 % 10) + '0');
                    rc2 = rc2/10;
                } while (rc2 > 0);
            }

            DosClose(hDev0);
        }
        else
            npszErrMsg = "DosOpen failed.\n\r";
    }
    else
        npszErrMsg = "ProbeKernel failed.\n\r";
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
USHORT NEAR R0Init16(PRP_GENIOCTL pRp)
{
    USHORT usRc = STATUS_DONE;
    APIRET rc;

    /* First we're to get the DosTable2 stuff. */
    rc = initGetDosTableData();
    if (rc == NO_ERROR)
    {
        ULONG cPages;
        char  hLockParm[12] = {0};
        ULONG ulLinParm;
        char  hLockData[12] = {0};
        ULONG ulLinData;

        if (!DevHelp_VirtToLin(SELECTOROF(pRp->ParmPacket), OFFSETOF(pRp->ParmPacket), &ulLinParm)
            &&
            !DevHelp_VirtToLin(SELECTOROF(pRp->DataPacket), OFFSETOF(pRp->DataPacket), &ulLinData)
            )
        {
            if (!(rc = DevHelp_VMLock(VMDHL_LONG | VMDHL_WRITE,
                                ulLinParm, sizeof(D16R0INITPARAM),
                                (LIN)~0UL, SSToDS_16(&hLockParm[0]), &cPages))
                &&
                !DevHelp_VMLock(VMDHL_LONG | VMDHL_WRITE,
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
                    usRc = CallR0Init32(SSToDS_16(&rp32init));
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



/**
 * Gets the data we need from the DosTables.
 * This data is TKSSBase16, R0FlatCS16 and R0FlatDS16.
 * @returns   Same as DevHelp_GetDosVar.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark    If you are not sure if TKSSBase16 is set or not, call this.
 *            After R0Init16 is called TKSSBase16 _is_ set.
 *            IMPORTANT! This function must _not_ be called after the initiation of the second device driver!!!
 */
USHORT NEAR initGetDosTableData(void)
{
    APIRET     rc;
    PDOSTABLE  pDT;
    PDOSTABLE2 pDT2;

    if (TKSSBase16 != 0)
        return NO_ERROR;

    /* First we're to get the DosTable2 stuff. */
    rc = DevHelp_GetDOSVar(9, 0, &pDT);
    if (rc == NO_ERROR)
    {
        pDT2 = (PDOSTABLE2)((char FAR *)pDT + pDT->cul*4 + 1);
        TKSSBase16 = (ULONG)pDT2->pTKSSBase;
        R0FlatCS16 = (USHORT)pDT2->R0FlatCS;
        R0FlatDS16 = (USHORT)pDT2->R0FlatDS;
    }
    return rc;
}
