/* $Id: d16Init.c,v 1.3 2002-12-16 01:53:25 bird Exp $
 *
 * d16init - init routines for both drivers.
 *
 * Copyright (c) 1999-2002 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#include "kKLInitHlp.h"
#include "devErrors.h"
#include "dev1632.h"
#include "dev16.h"
#include "d16vprintf.h"
#include "kKLlog.h"
#include "options.h"



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int             DoDevIOCtl(KKLR0INITPARAM  *pParam, KKLR0INITDATA *pData);


/**
 * Does the dev ioctl call to the helper driver for calling
 * a 32-bit Ring-0 worker.
 * @returns error code from DosOpen or DosDevIOCtl.
 * @param   pParam  Pointer to parameter buffer.
 * @param   pData   Pointer to data buffer.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 */
int             DoDevIOCtl(KKLR0INITPARAM  *pParam, KKLR0INITDATA *pData)
{
    APIRET          rc;
    HFILE           hDev0 = 0;
    USHORT          usAction = 0;

    /* Open the kKrnlHlp device driver. */
    rc = DosOpen(KKL_DEVICE_NAME, &hDev0, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
    if (rc == NO_ERROR)
    {
        pData->ulRc = 0;
        rc = DosDevIOCtl(pData, pParam, KKL_IOCTL_RING0INIT, KKL_IOCTL_CAT, hDev0);
        DosClose(hDev0);
        if (rc != NO_ERROR)
        {
            dprintf(("DosDevIOCtl failed with rc=%d\n", rc));
            rc = ERROR_D16_IOCTL_FAILED;
        }
    }
    else
    {
        dprintf(("DosOpen Failed with rc=%d\n", rc));
        rc = ERROR_D16_OPEN_DEV_FAILED;
    }

    return rc;
}



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
    pRpOut->CodeEnd = (USHORT)&CODE16END;
    pRpOut->DataEnd = (USHORT)&DATA16_ENDEND;
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
    RP32INIT        rp32init;
    RP32INIT FAR *  fprp32init = &rp32init;

    /*
     * Create new 32-bit init packet - Parameter pointer is thunked.
     */
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


    /*
     * Fill return data.
     */
    pRpOut->CodeEnd = (USHORT)&CODE16_ENDEND;
    pRpOut->DataEnd = (USHORT)&DATA16_ENDEND;
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
        char *psz = devGetErrorMsg(rc);
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
 * (pulTKSSBase32, R0FlatCS16 and R0FlatDS16)
 *
 * @returns   Same as DevHelp_GetDosVar.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
USHORT NEAR  initGetDosTableData(void)
{
    APIRET     rc;
    PDOSTABLE  pDT;
    PDOSTABLE2 pDT2;

    if (pulTKSSBase32 != 0)
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
        pulTKSSBase32 = (ULONG)pDT2->pTKSSBase;
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

