/* $Id: ldr.cpp,v 1.7.4.3 2000-08-22 03:00:20 bird Exp $
 *
 * ldr.cpp - Loader helpers.
 *
 * Copyright (c)  1999 knut  St.  osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_SEM

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "malloc.h"
#include "new.h"
#include <memory.h>
#include <stdlib.h>
#include <stddef.h>

#include "log.h"
#include "avl.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "ldr.h"
#include "ldrCalls.h"
#include "ModuleBase.h"
#include "pe2lx.h"
#include "options.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static PAVLNODECORE    pSFNRoot = NULL;
static PAVLNODECORE    pMTERoot = NULL;


/*
 * Loader State. (See ldr.h for more info.)
 */
ULONG          ulLdrState = LDRSTATE_UNKNOWN;


/*
 * Pointer to the executable module being loaded.
 * This pointer is set by ldrOpen and cleared by tkExecPgm.
 * It's hence only valid at tkExecPgm time. (isLdrStateExecPgm() == TRUE).
 */
PMODULE         pExeModule = NULL;


/*
 * Filehandle bitmap.
 */
unsigned char   achHandleStates[MAX_FILE_HANDLES/8];




/**
 * Gets a module by the give hFile.
 * @returns   Pointer to module node. If not found NULL.
 * @param     hFile  File handle of the module to be found.
 * @sketch    return a AVLGet on the pSFNRoot-tree.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PMODULE     getModuleBySFN(SFN hFile)
{
    return (PMODULE)AVLGet(&pSFNRoot, (AVLKEY)hFile);
}


/**
 * Gets a module by the MTE.
 * @returns   Pointer to module node. If not found NULL.
 * @param     pMTE  Pointer a Module Table Entry.
 * @sketch    Try find it in the MTE tree.
 *            IF not found THEN
 *            BEGIN
 *                DEBUG: validate pMTE pointer.
 *                Get the SFN from the MTE.
 *                IF found in the SFN-tree THEN
 *                BEGIN
 *                    Update the pMTE in the node.
 *                    Add the node to the MTE-tree.
 *                END
 *                ELSE return NULL
 *            END
 *            return pointer to module node.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PMODULE     getModuleByMTE(PMTE pMTE)
{
    #if 0
        /* Not 100% sure that this will work correctly! */
        PMODULE pMod = (PMODULE)AVLGet(&pMTERoot, (AVLKEY)pMTE);
        if (pMod == NULL)
        {
            #ifdef DEBUG
                if (pMTE <= (PMTE)0x10000)
                {
                    kprintf(("getModuleByMTE: invalid pMTE pointer - %#8x\n", pMTE));
                    return NULL;
                }
            #endif
            pMod = (PMODULE)AVLGet(&pSFNRoot, (AVLKEY)pMTE->mte_sfn);
            if (pMod != NULL)
            {
                pMod->coreMTE.Key = (AVLKEY)pMTE;
                pMod->fFlags |= MOD_FLAGS_IN_MTETREE;
                AVLInsert(&pMTERoot, (PAVLNODECORE)((unsigned)pMod + offsetof(MODULE, coreMTE)));
            }
        }
        else
            pMod = (PMODULE)((unsigned)pMod - offsetof(MODULE, coreMTE));
        return pMod;
    #else
        /* Use this for the time being. */
        #ifdef DEBUG
            if (pMTE <= (PMTE)0x10000)
            {
                kprintf(("getModuleByMTE: invalid pMTE pointer - %#8x\n", pMTE));
                return NULL;
            }
        #endif
        if (GetState(pMTE->mte_sfn) == HSTATE_OUR)
            return (PMODULE)AVLGet(&pSFNRoot, (AVLKEY)pMTE->mte_sfn);

        return NULL;
    #endif
}


/**
 * Gets a module by the hMTE.
 * @returns   Pointer to module node. If not found NULL.
 * @param     hMTE  Handle to a Module Table Entry.
 * @sketch    Convert hMte to an pMTE (pointer to MTE).
 *            Call getModuleByMTE with MTE pointer.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PMODULE     getModuleByhMTE(HMTE hMTE)
{
    PMTE pMTE;

    pMTE = ldrValidateMteHandle(hMTE);
    if (pMTE != NULL)
        return getModuleByMTE(pMTE);

    return NULL;
}


/**
 * Get a module by filename.
 * @returns   Pointer to module node. If not found NULL.
 * @param     pszFilename  Pointer to the filename which we are search by.
 * @sketch    Not implemented.
 * @status    Stub.
 * @author    knut st. osmundsen
 */
PMODULE     getModuleByFilename(PCSZ pszFilename)
{
    pszFilename = pszFilename;
    return NULL;
}


/**
 * Adds a module to the SFN-tree, if pMTE is not NULL it is added to the MTE-tree too.
 * @returns   NO_ERROR on success. Appropriate errorcode on failiure.
 * @param     hFile   System file number for the module.
 * @param     pMTE    Pointer to MTE. NULL is valid.
 * @param     fFlags  Type flags for the fFlags field in the node.
 * @param     pModObj Pointer to module object.
 * @sketch    DEBUG: check that the module doesn't exists and parameter check.
 *            (try) Allocate a new node. (return errorcode on failure)
 *            Fill in the node.
 *            Add the node to the SFN-tree.
 *            IF valid MTE pointer THEN add it to the MTE tree and set the in MTE-tree flag.
 *            return successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG       addModule(SFN hFile, PMTE pMTE, ULONG fFlags, ModuleBase *pModObj)
{
    PMODULE pMod;
    #ifdef DEBUG
        if (AVLGet(&pSFNRoot, (AVLKEY)hFile) != NULL)
            kprintf(("addModule: Module allready present in the SFN-tree!\n"));
        if (hFile == 0)
        {
            kprintf(("addModule: invalid parameter: hFile = 0\n"));
            return ERROR_INVALID_PARAMETER;
        }
        if ((fFlags & MOD_TYPE_MASK) == 0 || (fFlags  & ~MOD_TYPE_MASK) != 0UL)
        {
            kprintf(("addModule: invalid parameter: fFlags = 0x%#8x\n", fFlags));
            return ERROR_INVALID_PARAMETER;
        }
    #endif

    /* try allocate memory for the node. */
    pMod = (PMODULE)malloc(sizeof(MODULE));
    if (pMod == NULL)
    {
        kprintf(("addModule: out of memory!\n"));
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    /* fill in the module node. */
    pMod->coreKey.Key = (AVLKEY)hFile;
    pMod->hFile = hFile;
    pMod->pMTE = pMTE;
    pMod->fFlags = fFlags;
    pMod->Data.pModule = pModObj;

    /* insert the module node into the tree(s) */
    AVLInsert(&pSFNRoot, (PAVLNODECORE)pMod);
    if (pMTE != NULL)
    {
        pMod->coreMTE.Key = (AVLKEY)pMTE;
        pMod->fFlags |= MOD_FLAGS_IN_MTETREE;
        AVLInsert(&pMTERoot, (PAVLNODECORE)((unsigned)pMod + offsetof(MODULE, coreMTE)));
    }

    return NO_ERROR;
}


/**
 * Removes and frees a module node (including the data pointer).
 * @returns   NO_ERROR on success. Appropriate error code on failure.
 * @param     hFile  System filehandle of the module.
 * @sketch    Remove the node from the SFN-tree.
 *            IF present in the MTE-tree THEN remove it from the tree.
 *            Delete the datapointer.
 *            Free the module node.
 *            return successfully.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG      removeModule(SFN hFile)
{
    PMODULE pMod = (PMODULE)AVLRemove(&pSFNRoot, (AVLKEY)hFile);
    if (pMod == NULL)
    {
        kprintf(("removeModule: Module not found! hFile=%#4x\n", hFile));
        return ERROR_INVALID_PARAMETER;
    }

    /* In MTE-tree too? */
    if (pMod->fFlags & MOD_FLAGS_IN_MTETREE)
    {
        if (AVLRemove(&pMTERoot, (AVLKEY)pMod->pMTE) == NULL)
        {
            kprintf(("removeModule: MOD_FLAGS_IN_MTETREE set but AVLRemove returns NULL\n"));
        }
    }

    /* Delete the datapointer. */
    switch (pMod->fFlags & MOD_TYPE_MASK)
    {
        case MOD_TYPE_PE2LX:
            delete pMod->Data.pPe2Lx;
            break;
/*
        case MOD_TYPE_ELF2LX:
            break;
*/
#ifdef DEBUG
        default:
            kprintf(("removeModule: Unknown type, %#x\n", pMod->fFlags & MOD_TYPE_MASK));
#endif
    }

    /* Free the module node. */
    free(pMod);

    return NO_ERROR;
}


/**
 * Initiate the loader "sub-system".
 * @returns   NO_ERROR on success. !0 on error.
 */
ULONG ldrInit(void)
{
    int rc = NO_ERROR;

    /* init state table */
    memset(&achHandleStates[0], 0, sizeof(achHandleStates));

    /* init the tree roots */
    pSFNRoot = NULL;
    pMTERoot = NULL;

    /* ModuleBase logging. */
    ModuleBase::ulInfoLevel = options.ulInfoLevel;

    return rc;
}

