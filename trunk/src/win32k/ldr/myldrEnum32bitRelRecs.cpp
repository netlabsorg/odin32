/* $Id: myldrEnum32bitRelRecs.cpp,v 1.1 2000-01-24 10:11:48 bird Exp $
 *
 * myldrEnum32bitRelRecs - ldrEnum32bitRelRecs
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
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
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "avl.h"
#include "ModuleBase.h"
#include "ldr.h"
#include "ldrCalls.h"


/**
 * Applies relocation fixups to a page which is being loaded.
 * @returns    NO_ERROR on success?
 *             error code on error?
 * @param      pMTE           Pointer Module Table Entry.
 * @param      iObject        Index into the object table. (0-based)
 * @param      iPageTable     Index into the page table. (0-based)
 * @param      pvPage         Pointer to the page which is being loaded.
 * @param      ulPageAddress  Address of page.
 * @param      pvPTDA         Pointer to Per Task Data Aera
 *
 * @sketch     Check if one of our handles.
 */
ULONG LDRCALL myldrEnum32bitRelRecs(
    PMTE pMTE,
    ULONG iObject,
    ULONG iPageTable,
    PVOID pvPage,
    ULONG ulPageAddress,
    PVOID pvPTDA
    )
{
    PMODULE pMod;

    kprintf(("ldrEnum32BitRelRecs: pMTE=0x%08x iObject=0x%02x iPageTable=0x%03x pvPage=0x%08x\n"
             "                     ulPageAddress=0x%08x pvPTDA=0x%08x\n",
             pMTE, iObject, iPageTable, pvPage, ulPageAddress, pvPTDA
             ));

    pMod = getModuleByMTE(pMTE);
    if (pMod != NULL)
    {
        APIRET rc;
        rc = pMod->Data.pModule->applyFixups(pMTE, iObject, iPageTable, pvPage, ulPageAddress, pvPTDA);
        if (rc != NO_ERROR)
            return rc;
    }

    return ldrEnum32bitRelRecs(pMTE, iObject, iPageTable, pvPage, ulPageAddress, pvPTDA);
}
