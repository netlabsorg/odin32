/* $Id: d16init.c,v 1.9 2000-12-11 06:53:48 bird Exp $
 *
 * d16init - init routines for both drivers.
 *
 * IMPORTANT! Code and data defined here will be discarded after init is
 *            compleated. CodeEnd and DataEnd should not be set here.?
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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

#include "devSegDf.h"
#undef  DATA16_INIT
#define DATA16_INIT
#undef  CODE16_INIT
#define CODE16_INIT
#include "probkrnl.h"
#include "dev1632.h"
#include "dev16.h"
#include "vprntf16.h"
#include "log.h"
#include "options.h"
#include "errors.h"


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
    APIRET  rc;
    Device_Help = pRpIn->DevHlpEP;

    /*
     * Does this work at Ring-3 (inittime)?
     * If this work we could be saved from throuble!
     */
    rc = initGetDosTableData();
    if (rc != NO_ERROR)
        printf16("win32k - elf$: initGetDosTableData failed with rc=%d\n", rc);

    pRpOut->BPBArray = NULL;
    pRpOut->CodeEnd = (USHORT)&CODE16_INITSTART;
    pRpOut->DataEnd = (USHORT)&DATA16_INITSTART;
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
    NPSZ            npszErrMsg2 = NULL;

    /*
     * Probe kernel data.
     */
    rc = ProbeKernel(pRpIn);
    if (rc == NO_ERROR)
    {
        /*
         * Open and send a Ring-0 init packet to elf$.
         * If this succeeds win32k$ init succeeds.
         */
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
                if (data.usRcInit32 == NO_ERROR)
                {
                    if (!options.fQuiet)
                        printf16("Win32k.sys succesfully initiated!\n");
                    pRpOut->Status = pRpOut->rph.Status = STATUS_DONE;
                }
                else
                {
                    /* set correct error message and rc */
                    rc = data.usRcInit32;
                    npszErrMsg = "Ring-0 initiation failed. rc=%x\n";
                    if (rc >= ERROR_D32_FIRST && rc <= ERROR_D32_LAST)
                        npszErrMsg2 = GetErrorMsg(data.usRcInit32 + ERROR_PROB_SYM_D32_FIRST - ERROR_D32_FIRST);
                }
            }
            else
                npszErrMsg = "Ring-0 init: DosDevIOCtl failed. rc=%d\n";
            DosClose(hDev0);
        }
        else
            npszErrMsg = "Ring-0 init: DosOpen failed. rc=%d\n";
    }
    else
        npszErrMsg = ""; /* ProbeKrnl do its own complaining, but we need something here to indicate failure. */

    /*
     * Fill return data.
     */
    pRpOut->CodeEnd = (USHORT)&CODE16_INITSTART;
    pRpOut->DataEnd = (USHORT)&DATA16_INITSTART;
    pRpOut->BPBArray= NULL;
    pRpOut->Unit    = 0;

    /*
     * Any errors?, if so complain!
     */
    if (npszErrMsg)
    {
        printf16(npszErrMsg, rc);
        if (npszErrMsg2)
            printf16("%s\n", npszErrMsg2);
        pRpOut->Status = pRpOut->rph.Status = STATUS_DONE | STERR | ERROR_I24_GEN_FAILURE;
    }

    /* Init is completed. */
    fInitTime = FALSE;

    /* successful return */
    return pRpOut->rph.Status;
}



/**
 * R0 16-bit initiation.
 * This gets TKSSBase, thunks parameters and calls R0 32-bit initiation function.
 * @returns     Status word. We don't fail on R0Init32 but forwards the result
 *              using the usRcInit32.
 * @param       pRp     Generic IO Control request packet.
 */
USHORT NEAR  R0Init16(PRP_GENIOCTL pRp)
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

        /*
         * Thunk the request packet and lock userdata.
         */
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
            {
                /*
                 * -data and param is locked (do we need to lock the request packet too ?).-
                 * Create new 32-bit init packet - Parameter pointer is thunked.
                 */
                RP32INIT rp32init;

                _fmemcpy(&rp32init, ((PD16R0INITPARAM)pRp->ParmPacket)->pRpInitIn, sizeof(RPINITIN));
                if (((PD16R0INITPARAM)pRp->ParmPacket)->pRpInitIn->InitArgs == NULL ||
                    !DevHelp_VirtToLin(SELECTOROF(rp32init.InitArgs), OFFSETOF(rp32init.InitArgs), (PLIN)&rp32init.InitArgs)
                   )
                {   /* call 32-bit init routine and set 32 bit rc. */
                    usRc = CallR0Init32(SSToDS_16(&rp32init));
                    ((PD16R0INITDATA)pRp->DataPacket)->usRcInit32 = usRc;

                    /* set status to done (success).  (R0Init32 RC is return as usRcInit32.)  */
                    usRc = STATUS_DONE;
                }
                else
                    usRc |= STERR | ERROR_I24_INVALID_PARAMETER;


                /*
                 * finished - unlock data and parm;
                 */
                DevHelp_VMUnLock((LIN)SSToDS_16(&hLockParm[0]));
                DevHelp_VMUnLock((LIN)SSToDS_16(&hLockData[0]));
            }
            else
                usRc |= STERR | ERROR_I24_INVALID_PARAMETER;
        }
        else
            usRc |= STERR | ERROR_I24_INVALID_PARAMETER;
    }
    else
        usRc |= STERR | ERROR_I24_GEN_FAILURE;

    return usRc;
}



/**
 * Gets the data we need from the DosTables.
 * This data is TKSSBase16, R0FlatCS16 and R0FlatDS16.
 * @returns   Same as DevHelp_GetDosVar.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark    If you are not sure if TKSSBase16 is set or not, call this.
 *            After R0Init16 is called TKSSBase16 _is_ set.
 *            IMPORTANT! This function must _not_ be called after the initiation of the second device driver!!!
 *                       (Since this is init code not present after init...)
 */
USHORT NEAR  initGetDosTableData(void)
{
    APIRET     rc;
    PDOSTABLE  pDT;
    PDOSTABLE2 pDT2;

    if (TKSSBase16 != 0)
        return NO_ERROR;
    /*
    _asm {
        int 3;
    }
    */

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
