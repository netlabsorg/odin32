/* $Id: myldrClose.cpp,v 1.5.4.1 2000-07-16 22:43:35 bird Exp $
 *
 * myldrClose - ldrClose
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
#include "avl.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "ldr.h"
#include "ldrCalls.h"
#include "ModuleBase.h"


/**
 * Close a file.
 * @returns    NO_ERROR on success?
 *             error code on error?
 * @param      hFile  Handle to file which is to be closed.
 */
ULONG LDRCALL myldrClose(SFN hFile)
{
    /* closes handle */
    kprintf(("myldrClose: hFile = %.4x\n", hFile));
    if (GetState(hFile) == HSTATE_OUR)
    {
        APIRET rc;

        #ifdef DEBUG
        PMODULE pMod = getModuleBySFN(hFile);
        if (pMod != NULL)
            pMod->Data.pModule->dumpVirtualLxFile();
        else
            kprintf(("myldrClose: getModuleBySFN failed!!!"));
        #endif

        rc = removeModule(hFile);
        if (rc != NO_ERROR)
            kprintf(("myldrClose: removeModule retured rc=%d\n", rc));

        #pragma info(notrd)
        SetState(hFile, HSTATE_UNUSED);
        #pragma info(restore)
    }

    return ldrClose(hFile);
}
