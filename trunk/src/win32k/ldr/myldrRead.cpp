/* $Id: myldrRead.cpp,v 1.2 1999-10-14 01:25:39 bird Exp $
 *
 * myldrRead - _ldrRead.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
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
#include "pe2lx.h"
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
        PPENODE pNode;

        pNode = getNodePtr(hFile);
        if (pNode != NULL)
        {
            /* I would love to have a pointer to the MTE */
            if (pNode->pMTE == NULL && pMTE != NULL)
                pNode->pMTE == pMTE;

            /* debug */
            if (ulFlags != 0)
                kprintf(("_ldrRead: Warning ulFlags = 0x%x (!= 0)\n", ulFlags));

            rc = pNode->pPe2Lx->read(ulOffset, pBuffer, ulBytesToRead, ulFlags);
            return rc;
        }
        else
            kprintf(("_ldrRead:  DON'T PANIC! - but I can't get Node ptr! - This is really an IPE!\n"));
    }

    rc = _ldrRead( hFile, ulOffset, pBuffer, ulFlags, ulBytesToRead, pMTE );

//  kprintf(("_ldrRead:  hF=%+04x off=%+08x pB=%+08x fl=%+08x cb=%+04x pMTE=%+08x rc=%d\n",hFile,ulOffset,pBuffer,ulFlags,ulBytesToRead,pMTE,rc));

    return rc;
}
