/* $Id: d16strat.c,v 1.3 1999-11-10 01:45:30 bird Exp $
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

#include "dev1632.h"
#include "dev16.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
DDHDR aDevHdrs[2] = /* This is the first piece data in the driver!!!!!!! */
{
    {
        (unsigned long)(void _far *)(&aDevHdrs[1]), /* NextHeader */
        DEVLEV_3 | DEV_30 | DEV_CHAR_DEV,           /* SDevAtt */
        (unsigned short)(void _near *)strategyAsm0, /* StrategyEP */
        0,                                          /* InterruptEP */
        "elf$    ",                                 /* DevName */
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
        "win32k$ ",                                 /* DevName */
        0,                                          /* SDevProtCS */
        0,                                          /* SDevProtDS */
        0,                                          /* SDevRealCS */
        0,                                          /* SDevRealDS */
        DEV_16MB | DEV_IOCTL2                       /* SDevCaps */
    }
};

/* Note: All global variables must be initialized!  *
 *       Uninitialized variables ends up in DATA32. */
PFN     Device_Help = NULL;
ULONG   TKSSBase16  = 0;
USHORT  R0FlatCS16  = 0;
USHORT  R0FlatDS16  = 0;
BOOL    fInitTime   = TRUE;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
USHORT NEAR dev0GenIOCtl(PRP_GENIOCTL pRp);
USHORT NEAR dev1GenIOCtl(PRP_GENIOCTL pRp);


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
                else
                    return dev1Init((PRPINITIN)pRpH, (PRPINITOUT)pRpH);
            }
            break;

        case CMDGenIOCTL:               /* Generic IOCTL */
            if (usDev == 0)
                return dev0GenIOCtl((PRP_GENIOCTL)pRpH);
            else
                return dev1GenIOCtl((PRP_GENIOCTL)pRpH);

        case CMDOpen:                   /* device open */
        case CMDClose:                  /* device close */
        case CMDDeInstall:              /* De-Install driver */
        case CMDShutdown:
            return STATUS_DONE;
    }

    return STATUS_DONE | STATUS_ERR_UNKCMD;
}

extern char end;


/**
 * Generic I/O Control - device 0.
 * This will only handle the request for Ring-0 initiation.
 * @returns   Status word.
 * @param     pRp  Request packet.
 */
USHORT dev0GenIOCtl(PRP_GENIOCTL pRp)
{
    USHORT rc;
    if (pRp->Category == D16_IOCTL_CAT)
    {
        switch (pRp->Function)
        {
            case D16_IOCTL_RING0INIT:
                if (fInitTime)
                {
                    rc = R0Init16(pRp);
                    fInitTime = FALSE;
                    return rc;
                }
                break;

            case D16_IOCTL_GETKRNLOTES:
            {
                ULONG ulLin;
                if (DevHelp_VirtToLin(SELECTOROF(pRp->DataPacket), OFFSETOF(pRp->DataPacket), &ulLin) != NO_ERROR)
                    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
                return CallGetOTEs32(ulLin);
            }

            case D16_IOCTL_VERIFYPROCTAB:
                if (fInitTime)
                    return CallVerifyProcTab32();
                break;
        }
    }

    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}


/**
 * Generic I/O Control - device 0.
 * This will forward requests to 32-bit counterpart.
 * @returns   Status word.
 * @param     pRp  Request packet.
 */
USHORT dev1GenIOCtl(PRP_GENIOCTL pRp)
{
    pRp = pRp;
    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}

