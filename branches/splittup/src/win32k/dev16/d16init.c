/* $Id: d16init.c,v 1.12.2.1 2001-09-27 03:08:10 bird Exp $
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
/*#undef  DATA16_INIT
#define DATA16_INIT
#undef  CODE16_INIT
#define CODE16_INIT*/
#include "dev1632.h"
#include "dev16.h"
#include "vprntf16.h"
#include "options.h"

#include "kKLInitHlp.h"


/**
 * init function - device 1.
 * We will send an IOCtl request to the elf$ (device 0) which will
 * perform the Ring-0 initiation of the driver.
 * @returns   Status word.
 * @param     pRpIn   Pointer to input request packet.  Actually the same memory as pRpOut but another struct.
 * @param     pRpOut  Pointer to output request packet. Actually the same memory as pRpIn  but another struct.
 * @remark    pRpIn and pRpOut points to the same memory.
 */
USHORT NEAR devInit(PRPINITIN pRpIn, PRPINITOUT pRpOut)
{
    ULONG           rc;
    KKLR0INITPARAM  param;
    KKLR0INITDATA   data = {0UL};
    HFILE           hKrnlLib = 0;
    USHORT          usAction = 0;
    NPSZ            npszErrMsg = NULL;

    /*
     * Open and send a Ring-0 init packet to kKrnlHlp.
     * If this succeeds win32k$ init succeeds.
     */
    rc = DosOpen(KKL_DEVICE_NAME, &hKrnlLib, &usAction, 0UL, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                 0UL);
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
            rc = DosDevIOCtl(&data, &param, KKL_IOCTL_RING0INIT, KKL_IOCTL_CAT, hKrnlLib);
            if (rc == NO_ERROR)
            {
                if (data.ulRc == NO_ERROR)
                {
                    if (!options.fQuiet)
                        printf16("Win32k.sys successfully initiated!\n");
                    pRpOut->Status = pRpOut->rph.Status = STATUS_DONE;
                }
                else
                {
                    /* set correct error message and rc */
                    rc = data.ulRc;
                    npszErrMsg = "Ring-0 initiation failed. rc=%ld\n";
                }
            }
            else
                npszErrMsg = "Ring-0 init: DosDevIOCtl failed. rc=%ld\n";
        }
        else
            npszErrMsg = "Ring-0 init: Argument thunking failed. rc=%ld\n";

        DosClose(hKrnlLib);
    }
    else
        npszErrMsg = "Ring-0 init: DosOpen failed. rc=%ld\n";


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
        pRpOut->Status = pRpOut->rph.Status = STATUS_DONE | STERR | ERROR_I24_GEN_FAILURE;
    }
    printf16("\n");

    /* Init is completed. */
    fInitTime = FALSE;

    /* successful return */
    return pRpOut->rph.Status;
}

