/* $Id: d16strat.c,v 1.12.2.1 2001-09-27 03:08:11 bird Exp $
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
#include "win32k.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
DDHDR aDevHdrs[1] = /* This is the first piece data in the driver!!!!!!! */
{
    {
        ~0UL,                                       /* NextHeader */
        DEVLEV_3 | DEV_30 | DEV_CHAR_DEV,           /* SDevAtt */
        (unsigned short)(void _near *)strategyAsm,  /* StrategyEP */
        0,                                          /* InterruptEP */
        "win32k$ ",                                 /* DevName */
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
 * @remark    This function is called from the entrypoint in dev1st.asm
 */
USHORT NEAR strategy(PRPH pRpH)
{
    switch (pRpH->Cmd)
    {
        case CMDInit:                   /* INIT command */
            if (fInitTime)
                return devInit((PRPINITIN)pRpH, (PRPINITOUT)pRpH);
            break;

        case CMDGenIOCTL:               /* Generic IOCTL */
            return devGenIOCtl((PRP_GENIOCTL)pRpH);

        case CMDOpen:                   /* device open */
        case CMDClose:                  /* device close */
        {
            RP32OPENCLOSE rp32OpenClose = {0};
            rp32OpenClose.rph.Len = pRpH->Len;
            rp32OpenClose.rph.Unit = pRpH->Unit;
            rp32OpenClose.rph.Cmd = pRpH->Cmd;
            rp32OpenClose.rph.Status = pRpH->Status;
            rp32OpenClose.rph.Flags = pRpH->Flags;
            rp32OpenClose.rph.Link = (ULONG)pRpH->Link;
            rp32OpenClose.sfn = ((PRP_OPENCLOSE)pRpH)->sfn;
            if (pRpH->Cmd == CMDOpen)
                return CallWin32kOpen(SSToDS_16a(&rp32OpenClose));
            return CallWin32kClose(SSToDS_16a(&rp32OpenClose));
        }

        case CMDDeInstall:              /* De-Install driver */
        case CMDShutdown:
            return STATUS_DONE;
    }

    return STATUS_DONE | STATUS_ERR_UNKCMD;
}

extern char end;


/**
 * Generic I/O Control.
 * This will forward requests to 32-bit counterpart.
 * @returns   Status word.
 * @param     pRp  Request packet.
 */
USHORT NEAR devGenIOCtl(PRP_GENIOCTL pRp)
{
    if (pRp->Category == IOCTL_W32K_K32)
    {
        RP32GENIOCTL    rp32Init = {0};
        rp32Init.rph.Len = pRp->rph.Len;
        rp32Init.rph.Unit = pRp->rph.Unit;
        rp32Init.rph.Cmd = pRp->rph.Cmd;
        rp32Init.rph.Status = pRp->rph.Status;
        rp32Init.rph.Flags = pRp->rph.Flags;
        rp32Init.rph.Link = (ULONG)pRp->rph.Link;
        rp32Init.Category = pRp->Category;
        rp32Init.Function = pRp->Function;
        rp32Init.sfn = pRp->sfn;
        rp32Init.DataLen = pRp->DataLen;
        rp32Init.ParmLen = pRp->ParmLen;

        if (DevHelp_VirtToLin(SELECTOROF(pRp->DataPacket), OFFSETOF(pRp->DataPacket),
                              (PLIN)&rp32Init.DataPacket) != NO_ERROR)
            return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
        if (DevHelp_VirtToLin(SELECTOROF(pRp->ParmPacket), OFFSETOF(pRp->ParmPacket),
                              (PLIN)&rp32Init.ParmPacket) != NO_ERROR)
            return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;

        return CallWin32kIOCtl(SSToDS_16a(&rp32Init));
    }

    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}

