/* $Id: d16Init.c,v 1.2 2002-03-31 19:01:13 bird Exp $
 *
 * d16init - init routines for both drivers.
 *
 * IMPORTANT! Code and data defined here will be discarded after init is
 *            completed. CodeEnd and DataEnd should not be set here.?
 *
 * Copyright (c) 1999-2001 knut st. osmundsen (kosmunds@csc.com)
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
#include "kKLInitHlp.h"
#include "probkrnl.h"
#include "dev1632.h"
#include "dev16.h"
#include "d16vprintf.h"
#include "kKLlog.h"
#include "options.h"
#include "ProbKrnlErrors.h"


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
        printf16("kKrnlLib - kKrnlHlp: initGetDosTableData failed with rc=%d\n", rc);

    pRpOut->BPBArray = NULL;
    pRpOut->CodeEnd = (USHORT)&CODE16_INITSTART;
    pRpOut->DataEnd = (USHORT)&DATA16_INITSTART;
    pRpOut->Unit     = 0;
    pRpOut->rph.Status = STATUS_DONE;
    return STATUS_DONE;
}


/**
 * init function - device 1.
 * We will send an IOCtl request to the kKrnlHlp (device 0) which will
 * perform the Ring-0 initiation of the driver.
 * @returns   Status word.
 * @param     pRpIn   Pointer to input request packet.  Actually the same memory as pRpOut but another struct.
 * @param     pRpOut  Pointer to output request packet. Actually the same memory as pRpIn  but another struct.
 * @remark    pRpIn and pRpOut points to the same memory.
 */
USHORT NEAR dev1Init(PRPINITIN pRpIn, PRPINITOUT pRpOut)
{
    APIRET          rc;
    KKLR0INITPARAM  param;
    KKLR0INITDATA   data = {0UL};
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;


    /*
     * Probe kernel data.
     */
    rc = ProbeKernel(pRpIn);
    if (rc == NO_ERROR)
    {
        /*
         * Create new 32-bit init packet - Parameter pointer is thunked.
         */
        RP32INIT        rp32init;
        RP32INIT FAR *  fprp32init = &rp32init;

        _fmemcpy(fprp32init, pRpIn, sizeof(RPINITIN));
        if ((   pRpIn->InitArgs == NULL
             || !(rc = DevHelp_VirtToLin(SELECTOROF(rp32init.InitArgs), OFFSETOF(rp32init.InitArgs), (PLIN)&rp32init.InitArgs))
             )
            &&  !(rc = DevHelp_VirtToLin(SELECTOROF(fprp32init), OFFSETOF(fprp32init), (PLIN)&param.ulParam))
           )
        {   /* call 32-bit init routine and set 32 bit rc. */
            param.pfn = GetR0InitPtr();
            rc = DoDevIOCtl(&param, &data);
            if (rc == NO_ERROR)
                rc = (USHORT)data.ulRc;
        }
        else
            rc = ERROR_D16_THUNKING_FAILED;
    }

    /*
     * Fill return data.
     */
    pRpOut->CodeEnd = (USHORT)&CODE16_INITSTART;
    pRpOut->DataEnd = (USHORT)&DATA16_INITSTART;
    pRpOut->BPBArray= NULL;
    pRpOut->Unit    = 0;


    /*
     * Show Status or complain about errors.
     */
    if (!rc)
    {
        if (!options.fQuiet)
            printf16("kKrnlLib.sys successfully initiated!\n");
        pRpOut->Status = pRpOut->rph.Status = STATUS_DONE;
    }
    else
    {
        char *psz = GetErrorMsg(rc);
        printf16("kKrnlLib.sys init failed with rc=0x%x (%d)\n", rc, rc);
        if (psz)
            printf16("Explanation: %s\n\n", psz);
        else
            printf16("\n");
        pRpOut->Status = pRpOut->rph.Status = STATUS_DONE | STERR | ERROR_I24_GEN_FAILURE;
    }


    /* Init is completed. */
    fInitTime = FALSE;

    /* successful return */
    return pRpOut->rph.Status;
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

        /*
         * Convert to 32-bit addresses and put into global variables.
         */
        if (DevHelp_VirtToLin(SELECTOROF(pDT), OFFSETOF(pDT), (PLIN)&linDT))
            linDT = 0UL;
        if (DevHelp_VirtToLin(SELECTOROF(pDT2), OFFSETOF(pDT2), (PLIN)&linDT2))
            linDT2 = 0UL;
    }
    return rc;
}

