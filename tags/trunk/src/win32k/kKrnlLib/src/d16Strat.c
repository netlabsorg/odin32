/* $Id: d16Strat.c,v 1.1 2002-03-10 02:45:53 bird Exp $
 *
 * d16strat.c - 16-bit strategy routine, device headers, device_helper (ptr)
 *              and 16-bit IOClts.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

#define NO_WIN32K_LIB_FUNCTIONS

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <devhdr.h>
#include <devcmd.h>
#include <strat2.h>
#include <reqpkt.h>
#include <dhcalls.h>

/* Note that C-library function are only allowed during init! */

#include "devSegDf.h"
#include "dev1632.h"
#include "dev16.h"
#include "kKLInitHlp.h"

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern DDHDR _far aDevHdrs[2];
DDHDR aDevHdrs[2] = /* This is the first piece data in the driver!!!!!!! */
{
    {
        &aDevHdrs[1], /* NextHeader */
        DEVLEV_3 | DEV_30 | DEV_CHAR_DEV,           /* SDevAtt */
        (unsigned short)(void _near *)strategyAsm0, /* StrategyEP */
        0,                                          /* InterruptEP */
        "$KrnlHlp",                                 /* DevName */
        0,                                          /* SDevProtCS */
        0,                                          /* SDevProtDS */
        0,                                          /* SDevRealCS */
        0,                                          /* SDevRealDS */
        DEV_16MB | DEV_IOCTL2                       /* SDevCaps */
    },
    {
        ~0UL,                                       /* NextHeader */
        DEVLEV_3 | DEV_30 | DEV_CHAR_DEV,           /* SDevAtt */
        (unsigned short)(void _near *)strategyAsm1, /* StrategyEP */
        0,                                          /* InterruptEP */
        "$KrnlLib",                                 /* DevName */
        0,                                          /* SDevProtCS */
        0,                                          /* SDevProtDS */
        0,                                          /* SDevRealCS */
        0,                                          /* SDevRealDS */
        DEV_16MB | DEV_IOCTL2                       /* SDevCaps */
    }
};



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
USHORT NEAR devGenIOCtl(PRP_GENIOCTL pRp);


/**
 * Strategy routine.
 * @returns   Status word.
 * @param     pRpH   Pointer to request packed header. (Do not change the pointer!)
 * @parma     usDev  Device number.
 * @remark    This function is called from the entrypoint in dev1st.asm
 */
USHORT NEAR strategy(PRPH pRpH, unsigned short usDev)
{
    switch (pRpH->Cmd)
    {
        case CMDInit:                   /* INIT command */
            if (fInitTime)
            {
                if (usDev == 0)
                    return dev0Init((PRPINITIN)pRpH, (PRPINITOUT)pRpH);
                return dev1Init((PRPINITIN)pRpH, (PRPINITOUT)pRpH);
            }
            break;

        case CMDGenIOCTL:               /* Generic IOCTL */
            if (fInitTime)
                return devGenIOCtl((PRP_GENIOCTL)pRpH);
            break;

        case CMDOpen:                   /* device open */
        case CMDClose:                  /* device close */
            return STATUS_DONE;

        case CMDDeInstall:              /* De-Install driver */
        case CMDShutdown:
            return STATUS_DONE;

        case CMDInitBase:
        {
            MSGTABLE    msg = { 1178, 1, "kKrnlLib.sys is not yet capable of being a BASEDEV." };
            DevHelp_Save_Message( &msg );
            break;
        }
    }

    return STATUS_DONE | STATUS_ERR_UNKCMD;
}


/**
 * Generic I/O Control.
 * This will only handle the request for Ring-0 initiation.
 * @returns   Status word.
 * @param     pRp  Request packet.
 */
USHORT NEAR devGenIOCtl(PRP_GENIOCTL pRp)
{
    USHORT rc;

    if (pRp->Category == KKL_IOCTL_CAT)
    {
        switch (pRp->Function)
        {
            /*
             * This is the IOCtl which does the R0-initiation of the device driver.
             * It is allways available since it's an interface for calling 32-bit init
             * functions for other device drivers and IFSes.
             *
             * Since this IOCtl is issued before R0-Init is done, we'll have to
             * init TKSSBase for both 16-bit and 32-bit code and be a bit carefull.
             */
            case KKL_IOCTL_RING0INIT:
                if (TKSSBase16 == 0)
                    initGetDosTableData();
                /*
                 * Verify intput.
                 */
                if (   pRp->ParmPacket
                    && pRp->DataPacket
                    && !DevHelp_VerifyAccess(((PUSHORT)&pRp->ParmPacket)[1], sizeof(KKLR0INITPARAM), *(PUSHORT)&pRp->ParmPacket, VERIFY_READONLY)
                    && !DevHelp_VerifyAccess(((PUSHORT)&pRp->DataPacket)[1], sizeof(KKLR0INITPARAM), *(PUSHORT)&pRp->DataPacket, VERIFY_READWRITE)
                    )
                {
                    volatile PKKLR0INITPARAM pParm = (PKKLR0INITPARAM)pRp->ParmPacket; /* volatile is used of debugging reasons. */
                    volatile PKKLR0INITDATA  pData = (PKKLR0INITDATA)pRp->DataPacket;

                    pData->ulRc = CallR0Addr32bit(pParm->pfn, pParm->ulParam);
                    return STATUS_DONE;
                }
        }
    }

    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}

