/* $Id: myldrCheckInternalName.cpp,v 1.1 2000-12-11 06:31:52 bird Exp $
 *
 * ldrCheckInternalName - ldrCheckInternalName replacement with support for
 *                  long DLL names and no .DLL-extention dependency.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
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
#include <string.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "log.h"
#include "avl.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "dev32.h"
#include "ldr.h"
#include "ldrCalls.h"
#include "options.h"


/**
 * Checks if the internal name (first name in the resident nametable) matches
 * the filename.
 *
 * This replacement will support DLLs with non DLL extention
 * and names that are longer than 8 chars.
 *
 * @returns     NO_ERROR on success (the name matched).
 *              ERROR_INVALID_NAME if mismatch.
 * @param       pMTE    Pointer to the MTE of the module to check.<br>
 *                      Assumes! that the filename for this module is present in ldrpFileNameBuf.
 * @sketch      Check if library module - this check only applies to library modules (ie. DLLs).
 *              Uppercase filename.
 *              Parse filename - get the length of the name including any extention different from .DLL.
 *              Compare internal name and filename returning NO_ERROR if matches and errorcode if mismatch.
 *                  If this module is not in the GLOBAL CLASS we don't compare the extention.
 *
 * @remark      This function will have to change slightly when we're starting to
 *              support ELF shared libraries.
 */
ULONG LDRCALL myldrCheckInternalName(PMTE pMTE)
{
    /* Check if this feature is enabled */
    if (isDllFixesDisabled())
    {
        #ifdef DEBUG
        APIRET  rc = ldrCheckInternalName(pMTE);
        kprintf(("ldrCheckInternalName: pMTE=0x%08x intname=%.*s path=%s rc=%d (original)\n",
                 pMTE, *(PCHAR)pMTE->mte_swapmte->smte_restab, (PCHAR)pMTE->mte_swapmte->smte_restab + 1, ldrpFileNameBuf, rc));
        return rc;
        #else
        return ldrCheckInternalName(pMTE);
        #endif
    }

    /* Local Variables */
    PCHAR   pachName;                   /* Pointer to the name part of pachFilename. */
    int     cchName;                    /* Length of the name part of pachFilename.
                                         * Includes extention if extention is not .DLL.
                                         * this is the length relative to pachName used to match the internal name. */
    PCHAR   pachExt;                    /* Pointer to the extention part of pachFilename. (not dot!) */
    int     cchExt;                     /* Length of the extention part of pachFilename. (not dot!) */
    PSMTE   pSMTE;                      /* Pointer to swap mte. */

    /* Return successfully if not library module. */
    if (!(pMTE->mte_flags1 & LIBRARYMOD))
        return NO_ERROR;

    /* Uppercase and parse filename in ldrpFileNameBuf */
    cchName = (int)ldrGetFileName2(ldrpFileNameBuf, (PCHAR*)SSToDS(&pachName), (PCHAR*)SSToDS(&pachExt));
    cchExt = (pachExt) ? strlen(pachExt) : 0;
    ldrUCaseString(pachName, cchName + cchExt + 1);
    if ((pMTE->mte_flags1 & CLASS_MASK) == CLASS_GLOBAL && (cchExt != 3 || memcmp(pachExt, "DLL", 3)))
        cchName += cchExt + 1;          /* Internal name includes extention if the extention is not .DLL! */
                                        /* If no extention the '.' should still be there! */

    /* Compare the internal name with the filename and return accordingly. */
    pSMTE = pMTE->mte_swapmte;
    #ifdef DEBUG
    APIRET rc =
    #else
    return
    #endif
            (   pSMTE->smte_restab != NULL
             && *(PCHAR)pSMTE->smte_restab == cchName
             && (cchExt == 0 && (pMTE->mte_flags1 & CLASS_MASK) == CLASS_GLOBAL
                 ?      !memcmp(pachName, (PCHAR)pSMTE->smte_restab + 1, cchName - 1)   /* No extention. Internal name should have a signle dot at the end then. */
                    &&  ((PCHAR)pSMTE->smte_restab)[cchName] == '.'
                 : !memcmp(pachName, (PCHAR)pSMTE->smte_restab + 1, cchName)            /* Extention, .DLL or not global class. */
                )
             )
            ? NO_ERROR
            : ERROR_INVALID_NAME;
    #ifdef DEBUG
    kprintf(("myldrCheckInternalName: pMTE=0x%08x intname=%.*s path=%s rc=%d\n",
             pMTE, *(PCHAR)pMTE->mte_swapmte->smte_restab, (PCHAR)pMTE->mte_swapmte->smte_restab + 1, ldrpFileNameBuf, rc));
    return rc;
    #endif
}

