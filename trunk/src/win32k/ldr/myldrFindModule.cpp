/* $Id: myldrFindModule.cpp,v 1.1 2000-12-11 06:32:06 bird Exp $
 *
 * ldrFindModule - ldrFindModule replacement with support for long DLL names
 *                  and .DLL-extention dependency.
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
#include "ModuleBase.h"
#include "options.h"

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern  PPMTE pmte_h;
extern  PPMTE pprogram_h;
extern  PPMTE pprogram_l;
extern  PPMTE pglobal_h;
extern  PPMTE pglobal_l;
extern  PPMTE pspecific_h;
extern  PPMTE pspecific_l;


/**
 * Finds a module if it's loaded.
 *
 * This replacement will support DLLs with non DLL extention
 * and names longer than 8 chars.
 *
 * @returns     NO_ERROR on success.
 *              OS/2 errorcode on error.
 * @param       pachFilename    Pointer to module filename.
 *                              If usClass isn't CLASS_GLOBAL then the string
 *                              will have to be null terminated or in ldrpFileNameBuf.
 *                              If it's in the ldrpFileNameBuf we assume a fullly qualified name.
 * @param       cchFilename     Length of modulefilename. This should including the terminator
 *                              character if usClass isn't CLASS_GLOBAL.
 * @param       usClass         Module class. (CLASS_*)
 * @param       ppMTE           Pointer to pMTE found.
 * @sketch
 *              Check that parameters are supported - backout to original ldrFindModule.
 *              Check for empty name.
 *              Decide upon where to start search for the module - depends on usClass.
 *              *TODO*
 */
ULONG LDRCALL myldrFindModule(PCHAR pachFilename, USHORT cchFilename, USHORT usClass, PPMTE ppMTE)
{
    /* Check if this feature is enabled */
    if (isDllFixesDisabled())
    {
        #ifdef DEBUG
        APIRET rc = ldrFindModule(pachFilename, cchFilename, usClass, ppMTE);
        kprintf(("ldrFindModule: fn=%.*s  len=%d  class=0x%02x  ppMTE=0x%08x rc=%d (original)\n", cchFilename, pachFilename, cchFilename, usClass, ppMTE, rc));
        return rc;
        #else
        return ldrFindModule(pachFilename, cchFilename, usClass, ppMTE);
        #endif
    }

    /* Static variables */
    static PMTE  pmteNULL = NULL;
    static PPMTE ppmteNULL = &pmteNULL;
    static PPMTE * apppmteHeadTail[] =
    {/* head           tail */
        &pmte_h,        &ppmteNULL,     /* CLASS_ALL        0x00000000 */
        &pprogram_h,    &pprogram_l,    /* CLASS_PROGRAM    0x00000040 */
        &pglobal_h,     &pglobal_l,     /* CLASS_GLOBAL     0x00000080 */
        &pspecific_h,   &pspecific_l,   /* CLASS_SPECIFIC   0x000000c0 */
    };

    /* Local variables */
    PMTE    pmte;                       /* Current MTE. */
    PMTE    pmteEnd;                    /* The MTE top stop at. */
    ULONG   rc;                         /* Return code. */
    PCHAR   pachName;                   /* Pointer to the name part of pachFilename. */
    int     cchName;                    /* Length of the name part of pachFilename. (No extention.) */
    PCHAR   pachExt;                    /* Pointer to the extention part of pachFilename. (not dot!) */
    int     cchExt;                     /* Length of the extention part of pachFilename. (not dot!) */
    int     cchName8;                   /* Length of modname in MTE. */

    /*
     * Check if everything is the way it used to be... (unsupported kernel changes, etc.)
     */
    #ifdef DEBUG
    if (usClass & ~(CLASS_ALL | CLASS_PROGRAM | CLASS_GLOBAL | CLASS_SPECIFIC | SEARCH_FULL_NAME))
    {
        kprintf(("myldrFindModule: Unknown class flag! usClass=%d\n", usClass));
        return ldrFindModule(pachFilename, cchFilename, usClass, ppMTE);
    }
    #endif

    if (pachFilename < (PCHAR)0x10000 || ppMTE < (PPMTE)0x10000)
    {
        kprintf(("myldrFindModule: Invalid pointer(s); pachFilename=0x%08x  ppMTE=0x%08x", pachFilename, ppMTE));
        return ERROR_INVALID_ACCESS;
    }

    /*
     * Log.
     */
    kprintf(("myldrFindModule: fn=%.*s  len=%d  class=0x%02x  ppMTE=0x%08x\n", cchFilename, pachFilename, cchFilename, usClass, ppMTE));

    /*
     * Can't find an empty name.
     */
    if (cchFilename == 0)
    {
        *ppMTE = NULL;
        kprintf(("myldrFindModule: Not found; cchFilename = 0\n"));
        return ERROR_FILE_NOT_FOUND;
    }


    /*
     * Find start and end mte node
     */
    pmte = **apppmteHeadTail[usClass >> 5];
    pmteEnd = **apppmteHeadTail[(usClass >> 5) + 1];
    if (pmteEnd != NULL)                /* Advance one node - see loop condition. */
        pmteEnd = pmteEnd->mte_link;


    /*
     * Translate filename path if we're searching for a full name.
     */
    if (usClass != CLASS_GLOBAL)
    {
        if (pachFilename != ldrpFileNameBuf)
        {
            rc = ldrTransPath(pachFilename);
            if (rc)
                return rc;
            pachFilename = ldrpFileNameBuf;
            cchFilename = (USHORT)(strlen(pachFilename) + 1);
            ldrUCaseString(pachFilename, cchFilename);
        }
        #ifdef DEBUG
        else
        {
            if (cchFilename != strlen(pachFilename) + 1)
                kprintf(("myldrFindModule: %.*s cchFilename=%d  strlen+1=%d\n",
                         cchFilename, pachFilename, cchFilename, strlen(pachFilename) + 1));
            cchFilename = (USHORT)(strlen(pachFilename) + 1);
        }
        #endif
    }


    /*
     * Depending on the search type we'll have to find the name and
     * extention of the filename passed in to us.
     */
    if (usClass == CLASS_GLOBAL)
    {   /*
         * Dll, assumes modulename only (no path!).
         *  Find the extention.
         *  Determin name and extention length.
         * (NB! Can't use ldrGetFileName since it's looking for a slash.)
         */
        pachName = pachFilename;
        pachExt = pachFilename + cchFilename - 1;
        while (pachExt >= pachName && *pachExt != '.')
             pachExt--;
        if (pachExt < pachFilename)
        {
            cchName = cchFilename;
            pachExt = "DLL";
            cchExt = 3;
        }
        else
        {
            cchName = pachExt - pachName;
            pachExt++;
            cchExt = cchFilename - cchName - 1;
        }

        cchName8 = cchName > 8 ? 8 : cchName;

        #ifdef DEBUG
        if (    memchr(pachFilename, '\\', cchFilename)
            ||  memchr(pachFilename, '/', cchFilename)
            ||  (cchFilename > 2 && pachFilename[1] == ':')
            )
        {
            kprintf(("myldrFindModule: Invalid name in class global; name=%.*s\n", cchFilename, pachFilename));
            return ERROR_INVALID_NAME;
        }
        #endif
    } else if (    (usClass & ~SEARCH_FULL_NAME) == CLASS_SPECIFIC
               ||  usClass & CLASS_GLOBAL
               )
    {   /*
         * Find name and extention, and the length of those.
         */
        cchName = (int)ldrGetFileName2(pachFilename, (PCHAR*)SSToDS(&pachName), (PCHAR*)SSToDS(&pachExt));
        cchName8 = cchName > 8 ? 8 : cchName;
        cchExt = (pachExt) ? strlen(pachExt) : 0;
        #ifdef DEBUG
        if (cchName <= 0)
        {
            kprintf(("myldrFindModule: Invalid name! ldrGetFileName2 failed; name=%.*s\n", cchFilename, pachFilename));
            return ERROR_INVALID_NAME;
        }
        #endif
    }
    else
    {   /*
         * Search the whole path no need for extention or name.
         */
        cchName8 = cchExt = cchName = 0;
        pachName = pachExt = NULL;
    }


    /*
     * Search loop.
     */
    for (pmte = pmte; pmte != pmteEnd; pmte = pmte->mte_link)
    {
        /*
         * Check that we're still searching within mte of the correct class.
         * (Isn't this a debug sanity check? It's present in release kernel too!
         *  If this test is true the MTE list is corrupted!)
         */
        if (   (usClass & CLASS_MASK) != CLASS_ALL
            && (pmte->mte_flags1 & CLASS_MASK) != (usClass & CLASS_MASK))
        {
            kprintf(("myldrFindModule: Bad MTE list? Stopped on wrong class test. pmte=0x%08x usClass=0x%04x\n",
                     pmte, usClass));
            break;
        }


        /*
         * Look if this module matches. Lookup on global (DLL) modules
         * are special case.
         */
        if (usClass == CLASS_GLOBAL)
        {   /*
             * DLLs, match name only:
             *  Check the module name from the resident MTE.
             *  Check the filename part of the fullname in the SwapMTE.
             *  Check extention.
             */
            PCHAR   pachName2;
            PCHAR   pachExt2;
            PSMTE   pSMTE = pmte->mte_swapmte;

            if (    !memcmp(pmte->mte_modname, pachFilename, cchName8)
                &&  (   /* Filename < 8 char, check modname only. */
                        (cchFilename < 8 && pmte->mte_modname[cchFilename] == '\0')
                     || /* Filename >= 8 chars, we'll have to check the fullname. */
                        (   ldrGetFileName2(pSMTE->smte_path, (PCHAR*)SSToDS(&pachName2), (PCHAR*)SSToDS(&pachExt2)) == cchName
                         && !memcmp(pachName2, pachName, cchName)
                         && (cchExt == 0
                             ? pachExt2 == NULL || *pachExt2 == '\0'    /* If no extention the internal name contains a '.'. The filename may also contain a dot or it should not have an extention! This works for both cases. */
                             : cchExt == pSMTE->smte_path - pachExt2 +  /* This works when pachExt2 is NULL too. */
                                         (pSMTE->smte_pathlen           /* DOSCALLS don't have smte_pathlen set correctly  */
                                          ? pSMTE->smte_pathlen
                                          : strlen(pSMTE->smte_path)
                                          )
                               &&  !memcmp(pachExt2, pachExt, cchExt)
                             )
                         )
                     )
                )
            {
                *ppMTE = pmte;
                kprintf(("myldrFindModule: Found pmte=0x%08x  hmte=0x%04x  modname=%.8s  path=%s (GLOBAL)\n",
                         pmte, pmte->mte_handle, pmte->mte_modname, pSMTE->smte_path));
                return NO_ERROR;
            }
        }
        else
        {   /*
             * All, match full name:
             *  Check the module name before checking the
             *  fullname in the SwapMTE.
             */
            PSMTE   pSMTE = pmte->mte_swapmte;
            if ((   cchName8 == 0           /* This is 0 if we should not check the mte_modname. */
                 || !memcmp(pmte->mte_modname, pachName, cchName8)
                 )
                &&  pSMTE->smte_pathlen == cchFilename - 1
                &&  !memcmp(pSMTE->smte_path, pachFilename, cchFilename)    /* checks the '\0' too. */
                )
            {
                *ppMTE = pmte;
                kprintf(("myldrFindModule: Found pmte=0x%08x  hmte=0x%04x  modname=%.8s  path=%s (%x)\n",
                         pmte, pmte->mte_handle, pmte->mte_modname, pSMTE->smte_path, pmte->mte_flags1 & CLASS_MASK));
                return NO_ERROR;
            }
        }
    }   /* Search loop */


    /*
     * Not found. (*ppMte indicates this).
     */
    *ppMTE = NULL;
    kprintf(("myldrFindModule: Not Found\n"));
    return NO_ERROR;
}

