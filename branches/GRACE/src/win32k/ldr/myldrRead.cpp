/* $Id: myldrRead.cpp,v 1.6 2000-01-22 18:21:02 bird Exp $
 *
 * myldrRead - ldrRead.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <memory.h>
#include <stdlib.h>

#include "log.h"
#include "dev32.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "ModuleBase.h"
#include "pe2lx.h"
#include "avl.h"
#include "ldrCalls.h"
#include "ldr.h"


ULONG LDRCALL myldrRead(
                       SFN hFile,
                       ULONG ulOffset,
                       PVOID pBuffer,
                       ULONG ulFlags ,
                       ULONG ulBytesToRead,
                       PMTE pMTE
                       )
{
    ULONG   rc;

    /* Check if this is an overrided handle */
    if (GetState(hFile) == HSTATE_OUR)
    {
        PMODULE pMod;
        kprintf(("ldrRead+: hF=%+04x off=%+08x pB=%+08x fl=%+08x cb=%+04x pMTE=%+08x\n",hFile,ulOffset,pBuffer,ulFlags,ulBytesToRead,pMTE));

        pMod = getModuleBySFN(hFile);
        if (pMod != NULL)
        {
            /* I would love to have a pointer to the MTE */
            if (pMod->pMTE == NULL && pMTE != NULL)
                pMod->pMTE = pMTE;

            /* debug */
            if (ulFlags != 0)
                kprintf(("ldrRead: Warning ulFlags = 0x%x (!= 0)\n", ulFlags));

            if ((pMod->fFlags & MOD_TYPE_MASK) == MOD_TYPE_PE2LX)
                rc = pMod->Data.pModule->read(ulOffset, pBuffer, ulBytesToRead, ulFlags, pMTE);
            else
            {
                kprintf(("ldrRead: Invalid module type, %#x\n", pMod->fFlags & MOD_TYPE_MASK));
                rc = ERROR_READ_FAULT;
            }
            return rc;
        }
        else
            kprintf(("ldrRead:  DON'T PANIC! - but I can't get Node ptr! - This is really an IPE!\n"));
    }

    rc = ldrRead(hFile, ulOffset, pBuffer, ulFlags, ulBytesToRead, pMTE);

    #if 0
    kprintf(("ldrRead:  hF=%+04x off=%+08x pB=%+08x fl=%+08x cb=%+04x pMTE=%+08x rc=%d\n",
             hFile,ulOffset,pBuffer,ulFlags,ulBytesToRead,pMTE,rc));
    #endif

    return rc;
}
