/* $Id: d16strat.c,v 1.12 2001-07-08 02:57:42 bird Exp $
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
extern DDHDR _far aDevHdrs[2];
DDHDR aDevHdrs[2] = /* This is the first piece data in the driver!!!!!!! */
{
    {
        &aDevHdrs[1], /* NextHeader */
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
                return dev1Init((PRPINITIN)pRpH, (PRPINITOUT)pRpH);
            }
            break;

        case CMDGenIOCTL:               /* Generic IOCTL */
            if (usDev == 0)
                return dev0GenIOCtl((PRP_GENIOCTL)pRpH);
            return dev1GenIOCtl((PRP_GENIOCTL)pRpH);

        case CMDOpen:                   /* device open */
        case CMDClose:                  /* device close */
            if (usDev == 1)
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
            return STATUS_DONE;

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
USHORT NEAR dev0GenIOCtl(PRP_GENIOCTL pRp)
{
    USHORT rc;

    if (pRp->Category == D16_IOCTL_CAT)
    {
        switch (pRp->Function)
        {
            /*
             * This is the IOCtl which does the R0-initiation of the device driver.
             * Only available at init time...
             */
            case D16_IOCTL_RING0INIT:
                if (fInitTime)
                {
                    rc = R0Init16(pRp);
                    return rc;
                }
                break;

            /*
             * This is the IOCtl collects info of the running kernel.
             * Only available at init time.
             *
             * Since this IOCtl is issued before R0-Init is done, we'll have to
             * init TKSSBase for both 16-bit and 32-bit code and be a bit carefull.
             */
            case D16_IOCTL_GETKRNLINFO:
                if (fInitTime)
                {
                    ULONG ulLin;
                    if (fInitTime && TKSSBase16 == 0)
                        initGetDosTableData();
                    if (DevHelp_VirtToLin(SELECTOROF(pRp->DataPacket), OFFSETOF(pRp->DataPacket),
                                          &ulLin) != NO_ERROR)
                        return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
                    return CallGetKernelInfo32(ulLin);
                }
                break;

            /*
             * This is the IOCtl verifies the data in the ImportTab.
             * Only available at init time.
             *
             * Since this IOCtl is issued before R0-Init is done, we'll have to
             * init TKSSBase for both 16-bit and 32-bit code and be a bit carefull.
             */
            case D16_IOCTL_VERIFYIMPORTTAB:
                if (fInitTime)
                {
                    if (TKSSBase16 == 0)
                        initGetDosTableData();
                    rc = CallVerifyImportTab32();
                    if (pRp->DataPacket)
                        ((PD16VERIFYIMPORTTABDATA)pRp->DataPacket)->usRc = rc;
                    return STATUS_DONE;
                }
                break;
        }
    }
    else if (pRp->Category == IOCTL_W32K_K32 || pRp->Category == IOCTL_W32K_ELF)
    {
        RP32GENIOCTL rp32Init = {0};
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

        if (pRp->Category == IOCTL_W32K_ELF)
            rc = CallElfIOCtl(SSToDS_16a(&rp32Init));
        else
            rc = CallWin32kIOCtl(SSToDS_16a(&rp32Init));

        return rc;
    }


    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}


/**
 * Generic I/O Control - device 0.
 * This will forward requests to 32-bit counterpart.
 * @returns   Status word.
 * @param     pRp  Request packet.
 */
USHORT NEAR dev1GenIOCtl(PRP_GENIOCTL pRp)
{
    if (pRp->Category == IOCTL_W32K_K32 || pRp->Category == IOCTL_W32K_ELF)
    {
        USHORT          rc;
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

        if (pRp->Category == IOCTL_W32K_ELF)
            rc = CallElfIOCtl(SSToDS_16a(&rp32Init));
        else
            rc = CallWin32kIOCtl(SSToDS_16a(&rp32Init));

        return rc;
    }

    return STATUS_DONE | STERR | ERROR_I24_INVALID_PARAMETER;
}

