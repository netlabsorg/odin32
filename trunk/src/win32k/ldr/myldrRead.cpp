/* $Id: myldrRead.cpp,v 1.8 2000-12-11 06:53:53 bird Exp $
 *
 * myldrRead - ldrRead.
 *
 * Copyright (c) 1998-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "log.h"
#include "avl.h"
#include "dev32.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "ldrCalls.h"
#include "ldr.h"
#include "ModuleBase.h"
#include "pe2lx.h"


/**
 * Overloads ldrRead.
 * @returns   OS/2 return code.
 * @param     hFile     Filehandle to read from.
 * @param     pvBuffer  Buffer to read into.
 * @param     fpBuffer  This is not flags as I first though, but probably a far 16-bit pointer
 *                      to the buffer.
 * @param     cbToRead  Count of bytes to read.
 * @param     pMTE
 * @sketch    IF it's our module THEN
 *                Get module pointer. (complain if this failes and backout to ldrRead.)
 *                Currently - verify that it's a Pe2Lx module. (complain and fail if not.)
 *                Invoke the read method of the module do the requested read operation.
 *                Save pMTE if present and not save allready.
 *            ENDIF
 *            - backout or not our module -
 *            forward request to the original ldrRead.
 * @status    Completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
ULONG LDRCALL myldrRead(
    SFN     hFile,
    ULONG   ulOffset,
    PVOID   pvBuffer,
    ULONG   fpBuffer,
    ULONG   cbToRead,
    PMTE    pMTE
    )
{
    ULONG   rc;

    /* Check if this is an overrided handle */
    if (GetState(hFile) == HSTATE_OUR)
    {
        PMODULE pMod;
        kprintf(("myldrRead: hF=%+04x off=%+08x pB=%+08x fp=%+08x cb=%+04x pMTE=%+08x\n",
                 hFile, ulOffset, pvBuffer, fpBuffer, cbToRead, pMTE));

        pMod = getModuleBySFN(hFile);
        if (pMod != NULL)
        {
            /* I would love to have a pointer to the MTE */
            if (pMod->pMTE == NULL && pMTE != NULL)
                pMod->pMTE = pMTE;

            if ((pMod->fFlags & MOD_TYPE_MASK) == MOD_TYPE_PE2LX)
                rc = pMod->Data.pModule->read(ulOffset, pvBuffer, fpBuffer, cbToRead, pMTE);
            else
            {
                kprintf(("myldrRead: Invalid module type, %#x\n", pMod->fFlags & MOD_TYPE_MASK));
                rc = ERROR_READ_FAULT;
            }
        }
        else
        {
            kprintf(("myldrRead:  DON'T PANIC! - but I can't get Node ptr! - This is really an IPE!\n"));
            rc = ERROR_READ_FAULT;
        }
    }
    else
    {
        rc = ldrRead(hFile, ulOffset, pvBuffer, fpBuffer, cbToRead, pMTE);
    }
    #if 0
    kprintf(("myldrRead:  hF=%+04x off=%+08x pB=%+08x fp=%+08x cb=%+04x pMTE=%+08x rc=%d\n",
             hFile, ulOffset, pvBuffer, fpBuffer, cbToRead, pMTE, rc));
    #endif

    return rc;
}
