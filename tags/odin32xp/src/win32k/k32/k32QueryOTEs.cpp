/* $Id: k32QueryOTEs.cpp,v 1.3 2001-02-10 11:11:44 bird Exp $
 *
 * k32QueryOTEs  -  Get's the object table entries (OTEs) for a given
 *                  module (given by a module handle).
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS

#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_LDR

#define NO_WIN32K_LIB_FUNCTIONS

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "OS2Krnl.h"
#include "win32k.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"
#include "log.h"


/**
 * Gets the object table entries for a module.
 * @returns     OS2 returncode.
 * @param       hMTE    Module handle (HMTE) of the module.
 * @param       pQOte   Pointer to output buffer.
 * @param       cbQOte  Size (in bytes) of the output buffer.
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
APIRET k32QueryOTEs(HMTE hMTE, PQOTEBUFFER pQOte, ULONG cbQOte)
{
    APIRET  rc;
    PMTE    pMTE;

    /*
     * Validate parameters.
     *  Ensure that the buffer pointer is sensible.
     *  Ensure that the buffer not less than minimum size.
     */
    if ((ULONG)pQOte < 0x10000 || cbQOte < sizeof(QOTEBUFFER))
        return ERROR_INVALID_PARAMETER;

    /*
     * Take loader semaphore. (We are accessing LDR structures.)
     */
    rc = LDRRequestSem();
    if (rc != NO_ERROR)
    {
        kprintf(("k32QueryOTEs: LDRRequestSem failed with rc = %d\n", rc));
        return rc;
    }

    /*
     * Validate and get the MTE pointer.
     */
    pMTE = ldrValidateMteHandle(hMTE);
    if (pMTE != NULL && pMTE->mte_swapmte != NULL)
    {
        /*
         * Copy data to the output buffer.
         * 1) First we'll copy the object number.
         *    If this failes or no object we'll bailout/return.
         * 2) Then we'll check if the buffer is large enough to hold the
         *    object info.
         * 3) Check if LX executable and copy the OTEs to the output buffer.
         *    If not LX fail.
         */
        rc = TKSuULongNF(&pQOte->cOTEs, &pMTE->mte_swapmte->smte_objcnt);
        if (rc != NO_ERROR || pQOte->cOTEs == 0)
            goto bailout;

        if ((pMTE->mte_swapmte->smte_objcnt * sizeof(QOTE)) + (sizeof(QOTEBUFFER) - sizeof(QOTE))
            > cbQOte)
        {
            rc = ERROR_BUFFER_OVERFLOW;
            goto bailout;
        }

        if (pMTE->mte_flags2 & MTEFORMATLX)
        {
            rc = TKSuBuff(pQOte->aOTE,
                          pMTE->mte_swapmte->smte_objtab,
                          pMTE->mte_swapmte->smte_objcnt * sizeof(OTE),
                          TK_FUSU_NONFATAL);
        }
        else
            rc = ERROR_BAD_EXE_FORMAT;
    }
    else
        rc = ERROR_INVALID_HANDLE;

bailout:
    /*
     * Felease loader semaphore and return
     */
    LDRClearSem();

    return rc;
}

