/* $Id: kdtInit.c,v 1.1 2002-09-30 23:53:54 bird Exp $
 *
 * Ring-3 Device Testing: Init
 *
 *
 * Copyright (c) 2002 knut st. osmundsen <bird@anduin.net>
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
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "kDevTest.h"

#include <stdio.h>
#include <string.h>


/**
 * Validates the device header.
 * @returns Success indicator.
 * @param   pDevHdr     Pointer to the device header in question.
 * @param   pModInfo    Pointer to the module which contains the device header.
 */
BOOL kdtInitValidateHdr(PDEVHDR pDevHdr, PMODINFO pModInfo)
{
    int i;

    if (pDevHdr->offStrat >= pModInfo->aObjs[1].ote_size)
    {
        printf("kDevTest: strategy offset is invalid. %04x / %04x\n",
               pDevHdr->offStrat, pModInfo->aObjs[1].ote_size);
        return FALSE;
    }

    if (pDevHdr->offInt >= pModInfo->aObjs[1].ote_size)
    {
        printf("kDevTest: int offset is invalid. %04x / %04x\n",
               pDevHdr->offInt, pModInfo->aObjs[1].ote_size);
        return FALSE;
    }

    for (i = 0; i < sizeof(pDevHdr->achName); i++)
    {
        char ch = pDevHdr->achName[i];
        if (    ch == '\0'
            /*||  ch == ' - todo */
            )
        {
            printf("kDevTest: strategy offset is invalid. %04x / %04x\n",
                   pDevHdr->offStrat, pModInfo->aObjs[1].ote_size);
            return FALSE;
        }
    }

    return TRUE;
}




/**
 * Initialize a device driver.
 *
 * I.e. calling the strategy entry points with an init packet
 * for all the device headers in the module.
 *
 * @returns Success indicator.
 * @param   pModInfo    Pointer to the driver module which is to be initiated.
 * @param   pszArgs     Argument string.
 * @param   fBaseDev    Flag to distinguish between basedevs and other defines.
 * @remark  Called on 32-bit stack.
 */
BOOL kdtInitDriver(PMODINFO pModInfo, const char *pszArgs, BOOL fBaseDev)
{
    PDEVHDR     pDevHdr;

    /*
     * Get the first device header.
     */
    pDevHdr = (PDEVHDR)pModInfo->aObjs[0].ote_base;
    do
    {
        RP32INIT    rpinit = {0};
        ULONG       fpRpInit = FlatToSel(&rpinit);

        /*
         * Validate the device header.
         */
        if (!kdtInitValidateHdr(pDevHdr, pModInfo))
        {
            printf("kDevTest: Invalid device header!\n");
            return FALSE;
        }


        /*
         * Setup init request packet and send it.
         */
        rpinit.in.rph.Cmd  = fBaseDev ? CMDInitBase : CMDInit;
        rpinit.in.rph.Len  = sizeof(rpinit);
        rpinit.in.DevHlpEP = kdtDHGetRouterFP();
        rpinit.in.InitArgs = (ULONG)FlatToSel(pszArgs);
        printf("debug: fpRpInit=%08x\n", fpRpInit);
        if (!kdtStrategySend((ULONG)pDevHdr->offStrat | (pModInfo->aObjs[1].ote_sel << 16), fpRpInit))
        {
            printf("kDevTest: Failed to send strategy packet\n");
            return FALSE;
        }


        /*
         * Check return.
         */
        if (    rpinit.out.Status != STDON
            ||  rpinit.out.rph.Status != STDON)
        {
            printf("kDevTest: Init of %.8s failed status=%04x / %04x\n",
                   &pDevHdr->achName[0], rpinit.out.Status, rpinit.out.rph.Status);
            return FALSE;
        }


        /*
         * Validate code and data offsets.
         */
        if (    rpinit.out.CodeEnd > pModInfo->aObjs[1].ote_size
            ||  rpinit.out.DataEnd > pModInfo->aObjs[0].ote_size)
        {
            /* TODO: there might be some quirks here */
            printf("kDevTest: Bad code and/or data ends. code=%04x/%04x data=%04x/x%04x\n",
                   rpinit.out.CodeEnd, pModInfo->aObjs[1].ote_size,
                   rpinit.out.DataEnd, pModInfo->aObjs[0].ote_size);
            return FALSE;
        }


        /*
         * Next Device header;
         */
        if (pDevHdr->fpNext != 0xffffffff)
            pDevHdr = FPToFlat(pDevHdr->fpNext);
        else
            pDevHdr = NULL;
    } while (pDevHdr);

    return TRUE;
}
