/* $Id: myldrCheckInternalName.cpp,v 1.4 2001-07-06 19:22:35 bird Exp $
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
#define INCL_OS2KRNL_LDR


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
 *              support ELF shared libraries (ie. .so-files).
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
    PCHAR   pachResName;                /* Pointer to the internal name - resname.0 */
    APIRET  rc;                         /* Return code. */


    /*
     * Return successfully if not library module.
     */
    if (!(pMTE->mte_flags1 & LIBRARYMOD))
        return NO_ERROR;


    /*
     * Uppercase and parse filename in ldrpFileNameBuf
     */
    cchName = (int)ldrGetFileName2(ldrpFileNameBuf, (PCHAR*)SSToDS(&pachName), (PCHAR*)SSToDS(&pachExt));
    cchExt = (pachExt) ? strlen(pachExt) : 0;
    ldrUCaseString(pachName, cchName + cchExt + 1);
    pachResName = (PCHAR)pMTE->mte_swapmte->smte_restab;

    /*
     * Do the compare - DllFix case or standard case.
     */
    if (   (cchName > 8 && *pachResName > 8)
        || (   (pMTE->mte_flags1 & CLASS_MASK) == CLASS_GLOBAL
            && (cchExt != 3 || strcmp(pachExt, "DLL"))  /* Extention != DLL. */
            )
        )
    {   /*
         * Rules for long DLL names or GLOBAL dlls with extention <> DLL:
         *  1. If DLL extention, the internal name don't need to have an extention,
         *     but it could have.
         *  2. If not DLL extention, then internal name must have an extention.
         *  3. If no extention the internal name should end with a '.'.
         */
        if (pachExt != NULL && cchExt == 3 && !memcmp(pachExt, "DLL", 3))   /* DLL extention. */
        {   /* (1) */
            rc =(   (   *pachResName == cchName
                     || *pachResName == cchName + cchExt + 1)
                 && !memcmp(pachResName + 1, pachName, *pachResName)
                 );
        }
        else if (cchExt > 0)            /* Extention. */
        {   /* (2) */
            rc =(   *pachResName == cchName + cchExt + 1
                 && !memcmp(pachResName + 1, pachName, *pachResName)
                 );
        }                               /* No extetion. */
        else
        {   /* (3) */
            rc =(   *pachResName == cchName + 1
                 && pachResName[cchName + 1] == '.'
                 && !memcmp(pachResName + 1, pachName, cchName)
                 );
        }
        rc = (rc) ? NO_ERROR : ERROR_INVALID_NAME;
    }
    else
    {   /*
         * Rules for short DLL names. ( < 8 chars):
         *  1.  The internal name must match the DLL name.
         *  2b. If the DLL name is 8 chars the internal name could have extra chars (but we don't check).
         *          (This is a feature/bug.)
         *  2a. If the DLL name is less than 8 chars the internal name should match exactly.
         */
        #if 0
            /* This was the way it should be implemented, but code is buggy.
             * Current code works like this:
             *  rc =(   memcmp(pachName, pMTE->mte_modname, cchName)
             *       && (   cchName == 8
             *           || pMTE->mte_modname[cchName] == '\0'
             *           )
             *       ) ? ERROR_INVALID_NAME : NO_ERROR;
             *
             * This is so old that it has become an persistant bug in some ways.
             * The correct check will break Lotus Freelance for example.
             * But, the applications which are broken all seems to include the
             * .DLL extention in the internal name (and have length which is
             * shorter than 8 chars).
             * So, a correction will simply be to remove any .DLL extention
             * of the internal name before setting it ldrCreateMte. This fix
             * could always be done here too.
             *
             * BTW. I managed to exploit this bug to replace doscall1.dll.
             *      Which is very nasty!
             */
        rc =(   !memcmp(pachName, pMTE->mte_modname, cchName)   /* (1) */
             && (   cchName == 8                                /* (2a) */
                 || pMTE->mte_modname[cchName] == '\0'          /* (2b) */
                 )
             ) ? NO_ERROR : ERROR_INVALID_NAME;
        #else
        /* For the issue of compatibly with the bug we'll call the real function. */
        rc = ldrCheckInternalName(pMTE);
        #endif
    }


    /*
     * Log answer and return it.
     */
    kprintf(("myldrCheckInternalName: pMTE=0x%08x intname=%.*s path=%s rc=%d\n",
             pMTE, *(PCHAR)pMTE->mte_swapmte->smte_restab, (PCHAR)pMTE->mte_swapmte->smte_restab + 1, ldrpFileNameBuf, rc));

    return rc;
}

