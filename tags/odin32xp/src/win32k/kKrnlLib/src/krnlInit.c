/* $Id: krnlInit.c,v 1.4 2002-12-19 01:49:08 bird Exp $
 *
 * Init the OS2 Kernel facilities; identify it, find symbols, import table.
 *
 * Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NOFILEID
static const char szFileId[] = "$Id: krnlInit.c,v 1.4 2002-12-19 01:49:08 bird Exp $";
#endif


/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#if 0//def R3TST
    #define kprintf2(a) kprintf(a)
#else
    #define kprintf2(a) (void)0
#endif

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <kLib/kTypes.h>
#include <kLib/kDevHlp.h>
#include <kLib/format/LXexe.h>

#include <os2def.h>
#include <string.h>

#define INCL_OS2KRNL_ALL
#define LDR_INCL_INITONLY
#include "OS2Krnl.h"

#define INCL_KKL_HEAP
#define INCL_KKL_MISC
#define INCL_KKL_LOG
#include "kKrnlLib.h"

#include "devErrors.h"
#include "krnlImportTable.h"
#include "krnlPrivate.h"

#ifdef R3TST
    #include "testcase.h"
    #define x86DisableWriteProtect() 0
    #define x86RestoreWriteProtect(a) (a = a)
#endif


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
KSEMMTX     kmtxImports;

PMTE        pKrnlMTE = NULL;            /* Initiated by krnlGetKernelInfo */
PSMTE       pKrnlSMTE = NULL;           /* Initiated by krnlGetKernelInfo */
POTE        pKrnlOTE = NULL;            /* Initiated by krnlGetKernelInfo */
int         cKernelObjects = 0;         /* Initiated by krnlGetKernelInfo */

extern char callTab[1];


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int         krnlGetKernelInfo(void);
int         krnlLookupKernel(void);

int         krnlInitImports(void);
int         krnlInitExports(void);

#ifdef R3TST
PMTE        GetOS2KrnlMTETst(void)
void        R3TstFixImportTab(void);
#endif

/**
 * Initiates the imported functions.
 * @returns 0 on success.
 *          -1 on failure.
 */
int krnlInit(void)
{
    KLOGENTRY0("int");
    int     rc;

    /*
     * Identify the kernel.
     */
    rc = krnlGetKernelInfo();
    if (rc)
        return rc;

    /*
     * Lookup the kernel in the Database
     */
    rc = krnlLookupKernel();
    if (rc)
    {
        #if 0 /* FIXME */
        int rc2 = krnlLoadKernelSym();
        if (rc2)
        {
            kDH_SaveMessage();
            return rc;
        }
        #else
        return rc;
        #endif
    }

    /*
     * Init imports.
     */
    rc = krnlInitImports();

    /*
     * Init the import semaphore.
     */
    KSEMInit((PKSEM)(void*)&kmtxImports, KSEM_MUTEX, 0);

    KLOGEXIT(rc);
    return rc;
}


/**
 * Get kernel OTEs
 * This function set pKrnlMTE, pKrnlSMTE and pKrnlOTE, fKernel, ulKernelBuild and cKernelObjects.
 *
 * @returns   Strategy return code:
 *            STATUS_DONE on success.
 *            STATUS_DONE | STERR | errorcode on failure.
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 */
int krnlGetKernelInfo(void)
{
    KLOGENTRY0("int");
    int     i;
    ULONG   rc;

    /* Find the kernel OTE table */
#ifndef R3TST
    pKrnlMTE = krnlGetOS2KrnlMTE();
#else
    pKrnlMTE = GetOS2KrnlMTETst();
#endif
    if (pKrnlMTE != NULL)
    {
        pKrnlSMTE = pKrnlMTE->mte_swapmte;
        if (pKrnlSMTE != NULL)
        {
            if (pKrnlSMTE->smte_objcnt <= 42)
            {
                pKrnlOTE = pKrnlSMTE->smte_objtab;
                if (pKrnlOTE != NULL)
                {
                    BOOL    fKrnlTypeOk;

                    cKernelObjects = (unsigned char)pKrnlSMTE->smte_objcnt;
                    rc = 0;

                    /*
                     * Search for internal revision stuff AND 'SAB KNL?' signature in the two first objects.
                     */
                    fKrnlTypeOk = FALSE;
                    fKernel = 0;
                    ulKernelBuild = 0;
                    for (i = 0; i < 2 && ulKernelBuild == 0; i++)
                    {
                        const char *psz = (const char*)pKrnlOTE[i].ote_base;
                        const char *pszEnd = psz + pKrnlOTE[i].ote_size - 50; /* Last possible search position. */

                        while (psz < pszEnd)
                        {
                            if (strncmp(psz, "Internal revision ", 18) == 0 && (psz[18] >= '0' && psz[18] <= '9'))
                            {
                                int j;
                                kprintf2(("krnlGetKernelInfo: found internal revision: '%s'\n", psz));

                                /* skip to end of "Internal revision " string. */
                                psz += 18;

                                /* Read number*/
                                while ((*psz >= '0' && *psz <= '9') || *psz == '.')
                                {
                                    if (*psz != '.')
                                        ulKernelBuild = (unsigned short)(ulKernelBuild * 10 + (*psz - '0'));
                                    psz++;
                                }

                                /* Check if build number seems valid. */
                                if (   !(ulKernelBuild >=  8254 && ulKernelBuild <  8383) /* Warp 3 fp 32 -> fp 60 */
                                    && !(ulKernelBuild >=  9023 && ulKernelBuild <= 9036) /* Warp 4 GA -> fp 12 */
                                    && !(ulKernelBuild >= 14039 && ulKernelBuild < 14150) /* Warp 4.5 GA -> ?? */
                                    && !(ulKernelBuild >=  6600 && ulKernelBuild <= 6678) /* Warp 2.1x fix?? (just for fun!) */
                                      )
                                {
                                    kprintf(("krnlGetKernelInfo: info summary: Build %d is invalid - invalid fixpack?\n", ulKernelBuild));
                                    rc = ERROR_D32_INVALID_BUILD;
                                    break;
                                }

                                /* Check for any revision flag */
                                if ((*psz >= 'A' && *psz <= 'Z') || (*psz >= 'a' && *psz <= 'z'))
                                {
                                    fKernel |= (USHORT)((*psz - (*psz >= 'a' ? 'a'-1 : 'A'-1)) << KF_REV_SHIFT);
                                    psz++;
                                }
                                if (*psz == ',') /* This is ignored! */
                                    *psz++;

                                /* If this is an Aurora/Warp 4.5 or Warp 3 kernel there is more info! */
                                if (psz[0] == '_' && (psz[1] == 'S' || psz[1] == 's'))  /* _SMP  */
                                    fKernel |= KF_SMP;
                                else
                                    if (*psz != ','
                                        && (   (psz[0] == '_' && psz[1] == 'W' && psz[2] == '4')  /* _W4 */
                                            || (psz[0] == '_' && psz[1] == 'U' && psz[2] == 'N' && psz[3] == 'I' && psz[4] == '4')  /* _UNI4 */
                                            )
                                        )
                                    fKernel |= KF_W4 | KF_UNI;
                                else
                                    fKernel |= KF_UNI;


                                /* Check if its a debug kernel (look for DEBUG at start of object 3-5) */
                                if (!fKrnlTypeOk)
                                {
                                    j = 3;
                                    while (j < 5)
                                    {
                                        /* There should be no iopl object preceding the debugger data object. */
                                        if ((pKrnlOTE[j].ote_flags & OBJIOPL) != 0)
                                            break;
                                        /* Is this is? */
                                        if ((pKrnlOTE[j].ote_flags & OBJINVALID) == 0
                                            && (pKrnlOTE[j].ote_flags & (OBJREAD | OBJWRITE)) == (OBJREAD | OBJWRITE)
                                            && strncmp((char*)pKrnlOTE[j].ote_base, "DEBUG", 5) == 0)
                                        {
                                            fKernel |= KF_DEBUG;
                                            break;
                                        }
                                        j++;
                                    }
                                }

                                /* Display info */
                                kprintf(("krnlGetKernelInfo: info summary: Build %d, fKernel=0x%x\n",
                                         ulKernelBuild, fKernel));

                                /* Break out */
                                break;
                            }

                            /*
                             * Look for the SAB KNL? signature to check which kernel type we're
                             * dealing with. This could also be reached thru the selector found
                             * in the first element for the SAS_tables_area array.
                             */
                            if (!fKrnlTypeOk && strncmp(psz, "SAB KNL", 7) == 0)
                            {
                                fKrnlTypeOk = TRUE;
                                if (psz[7] == 'D')
                                    fKernel |= KF_ALLSTRICT;
                                else if (psz[7] == 'B')
                                    fKernel |= KF_HALFSTRICT;
                                else if (psz[7] != 'R')
                                    fKrnlTypeOk = FALSE;
                            }

                            /* next */
                            psz++;
                        } /* while loop searching for "Internal revision " */
                    } /* for loop on objects 0-1. */

                    /* Set error code if not found */
                    if (ulKernelBuild == 0)
                    {
                        rc = ERROR_D32_BUILD_INFO_NOT_FOUND;
                        kprintf(("krnlGetKernelInfo: Internal revision was not found!\n"));
                    }
                }
                else
                    rc = ERROR_D32_NO_OBJECT_TABLE;
            }
            else
                rc = ERROR_D32_TOO_MANY_OBJECTS;
        }
        else
            rc = ERROR_D32_NO_SWAPMTE;
    }
    else
        rc = ERROR_D32_GETOS2KRNL_FAILED;

    if (!rc)
        kprintf(("krnlGetKernelInfo: failed. rc = %d\n", rc));

    KLOGEXIT(rc);
    return (rc);
}

#ifdef R3TST
/**
 * Creates a fake kernel MTE, SMTE and OTE for use while testing in Ring3.
 * @returns Pointer to the fake kernel MTE.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
PMTE GetOS2KrnlMTETst(void)
{
    KLOGENTRY0("PMTE");
    static MTE    KrnlMTE;
    static SMTE   KrnlSMTE;

    KrnlMTE.mte_swapmte = &KrnlSMTE;
    KrnlSMTE.smte_objtab = &aKrnlOTE[0];
    KrnlSMTE.smte_objcnt = cObjectsFake;

    KLOGEXIT(&KrnlMTE);
    return &KrnlMTE;
}

#endif


/**
 * Lookups a kernel in the 32-bit symbol database.
 *
 * @returns NO_ERROR on success.
 * @returns ERROR_PROB_SYMDB_KRNL_NOT_FOUND if not found.
 * @returns Low word: error code. High word: function number. Other errors.
 *
 * @remark  Supports multiple kernels with same build, type and object count.
 *          Hence all kernels are searched.
 * @remark  Uses ulKernelBuild, fKernel, cKernelObjects.
 */
int krnlLookupKernel(void)
{
    KLOGENTRY0("int");
    int     i;
    ULONG   rc = ERROR_PROB_SYMDB_KRNL_NOT_FOUND;


    /*
     * Loop tru the DB entries until a NULL pointer is found.
     */
    for (i = 0; aKrnlSymDB32[i].usBuild != 0; i++)
    {
        if (   aKrnlSymDB32[i].usBuild  == (USHORT)ulKernelBuild
            && aKrnlSymDB32[i].fKernel  == fKernel
            && aKrnlSymDB32[i].cObjects == (char)cKernelObjects)
        {   /* found matching entry! */
            int                 j;
            const KRNLDBENTRY * pEntry = &aKrnlSymDB32[i];

            kprintf(("Found entry for this kernel!\n"));

            /*
             * Copy symbol data from the DB to aImportTab.
             */
            for (j = 0; j < NBR_OF_KRNLIMPORTS; j++)
            {
                aImportTab[j].offObject  = pEntry->aSyms[j].offObject;
                aImportTab[j].iObject    = pEntry->aSyms[j].iObject;
                aImportTab[j].chOpcode   = pEntry->aSyms[j].chOpcode;
                aImportTab[j].ulAddress  = pKrnlOTE[pEntry->aSyms[j].iObject].ote_base
                                           + pEntry->aSyms[j].offObject;
                aImportTab[j].usSel      = pKrnlOTE[pEntry->aSyms[j].iObject].ote_sel;
                aImportTab[j].fFound     = (char)((aImportTab[j].offObject != 0xFFFFFFFFUL) ? 1 : 0);

                kprintf2(("  %-3d addr=0x%08lx off=0x%08lx  %s\n",
                          j, aImportTab[j].ulAddress, aImportTab[j].offObject,
                          aImportTab[j].achName));
            }

            /*
             * Verify prologs and return if successful.
             */
            rc = krnlVerifyImportTab();
            if (!rc)
            {
                KLOGEXIT(rc);
                return rc;
            }
        }
    }

    KLOGEXIT(rc);
    return rc;
}


/**
 * Verifies the aImportTab.
 *
 * @returns   16-bit errorcode where the high byte is the procedure number which
 *            the error occured on and the low byte the error code.
 * @remark    Called from IOCtl.
 *            WARNING! This function is called before the initroutine (R0INIT)!
 */
int krnlVerifyImportTab(void)
{
    KLOGENTRY0("int");
    int     i;
    int     cb;
    int     cbmax;

    /*
     * Verify aImportTab.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        /*
         * Debug info
         */
        kprintf2(("procedure no.%d is being checked: %s addr=0x%08x iObj=%d offObj=%d\n",
                  i, &aImportTab[i].achName[0], aImportTab[i].ulAddress,
                  aImportTab[i].iObject, aImportTab[i].offObject));

        /* Verify that it is found */
        if (!aImportTab[i].fFound)
        {
            if (EPTNotReq(aImportTab[i]))
                continue;
            else
            {
                kprintf(("procedure no.%d was not fFound!\n", i));
                KLOGEXIT(ERROR_D32_PROC_NOT_FOUND | (i << ERROR_D32_PROC_SHIFT));
                return ERROR_D32_PROC_NOT_FOUND | (i << ERROR_D32_PROC_SHIFT);
            }
        }

        /* Verify read/writeable. */
        if (   aImportTab[i].iObject >= pKrnlSMTE->smte_objcnt                             /* object index valid? */
            || aImportTab[i].ulAddress < pKrnlOTE[aImportTab[i].iObject].ote_base          /* address valid? */
            || aImportTab[i].ulAddress + 16 > (pKrnlOTE[aImportTab[i].iObject].ote_base +
                                                pKrnlOTE[aImportTab[i].iObject].ote_size)  /* address valid? */
            || aImportTab[i].ulAddress - aImportTab[i].offObject
               != pKrnlOTE[aImportTab[i].iObject].ote_base                                 /* offObject ok?  */
            )
        {
            kprintf(("procedure no.%d has an invalid address or object number.!\n"
                     "    %s  addr=0x%08x iObj=%d offObj=%d\n",
                     i, &aImportTab[i].achName[0], aImportTab[i].ulAddress,
                     aImportTab[i].iObject, aImportTab[i].offObject));
            KLOGEXIT(ERROR_D32_INVALID_OBJ_OR_ADDR | (i << ERROR_D32_PROC_SHIFT));
            return ERROR_D32_INVALID_OBJ_OR_ADDR | (i << ERROR_D32_PROC_SHIFT);
        }


        #ifndef R3TST
        if (aImportTab[i].ulAddress < 0xff400000UL)
        {
            kprintf(("procedure no.%d has an invalid address, %#08x!\n",
                     i, aImportTab[i].ulAddress));
            KLOGEXIT(ERROR_D32_INVALID_ADDRESS | (i << ERROR_D32_PROC_SHIFT));
            return ERROR_D32_INVALID_ADDRESS | (i << ERROR_D32_PROC_SHIFT);
        }
        #endif

        switch (aImportTab[i].fType & ~(EPT_BIT_MASK | EPT_NOT_REQ | EPT_WRAPPED | EPT_PROCH))
        {
            case EPT_PROC:
                /*
                 * Verify known function prolog.
                 */
                if (EPT32BitEntry(aImportTab[i]))
                {
                    cb = krnlInterpretProlog32((char*)aImportTab[i].ulAddress);
                    cbmax = OVERLOAD32_ENTRY - 5; /* 5 = Size of the jump instruction */
                }
                else
                {
                    cb = krnlInterpretProlog16((char*)aImportTab[i].ulAddress);
                    cbmax = OVERLOAD16_ENTRY - 5; /* 5 = Size of the jump instruction */
                }

                /*
                 * Check result of the function prolog interpretations.
                 */
                if (    cb < 5
                    ||  cb > cbmax
                    ||  (aImportTab[i].chOpcode != OPCODE_IGNORE
                         && aImportTab[i].chOpcode != *(unsigned char*)aImportTab[i].ulAddress)
                    )
                {   /* verify failed. */
                    kprintf(("verify failed for procedure no.%d (cb=%d), %s\n", i, cb, aImportTab[i].achName));
                    KLOGEXIT(ERROR_D32_TOO_INVALID_PROLOG | (i << ERROR_D32_PROC_SHIFT));
                    return ERROR_D32_TOO_INVALID_PROLOG | (i << ERROR_D32_PROC_SHIFT);
                }
                break;

            case EPT_VAR:
                /* do nothing! */
                break;

            default:
                kprintf(("invalid type/type not implemented. Proc no.%d, %s\n",i, aImportTab[i].achName));
                INT3(); /* temporary fix! */
                KLOGEXIT(ERROR_D32_NOT_IMPLEMENTED | (i << ERROR_D32_PROC_SHIFT));
                return ERROR_D32_NOT_IMPLEMENTED | (i << ERROR_D32_PROC_SHIFT);
        }
    }

    KLOGEXIT(0);
    return 0;
}


/**
 * Initiates the overrided functions.
 * @returns   16-bit errorcode where the high byte is the procedure number which
 *            the error occured on and the low byte the error code.
 */
int krnlInitImports(void)
{
    KLOGENTRY0("int");
    int         rc;
    int         i;
    int         cb;
    char *      pchCTEntry;             /* Pointer to current 32-bit calltab entry. */
    char *      pchCTEntry16;           /* Pointer to current 16-bit calltab entry. */
    KDHVMLOCK   lhCT16;                 /* Lock handle for the 16-bit calltable */


    /*
     * verify aImportTab
     */
    rc = krnlVerifyImportTab();
    if (rc)
    {
        kprintf(("krnlVerifyImportTab failed with rc=0x%x\n", rc));
        INT3();
        KLOGEXIT(rc);
        return rc;
    }


    /*
     * Lock the 16-bit calltab segment.
     */
    rc = kDH_VMLock2(&callTab16[0], &callTab16END[0] - &callTab16[0], VMDHL_WRITE | VMDHL_LONG, SSToDS(&lhCT16));
    if (rc)
    {
        kprintf(("D32Hlp_VMLock2(%x, %x, ..) -> rc=%d\n",
                 pchCTEntry16, &callTab16END[0] - pchCTEntry16, rc));
        INT3();
    }

#ifdef R3TST
    /*
     * Insert fake functions.
     * ----
     * For some testcases we need real functions...
     * But that apparently doesn't work for all testcases (for some mysterious reason!).
     */
    if (iTestcaseNo != 2)
        R3TstFixImportTab();
#endif

    /*
     * import and copy prologs.
     */
    pchCTEntry = &callTab[0];
    pchCTEntry16 = &callTab16[0];
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        switch (aImportTab[i].fType & ~EPT_WRAPPED)
        {
            /*
             * 32-bit procedure.
             */
            case EPT_PROCNR32:     /* Not required */
                if (!aImportTab[i].fFound) /* set the f<name> flag. */
                    aImportTab[i].ulAddress = auNopFuncs[i];
                *(unsigned long *)(void*)pchCTEntry = aImportTab[i].fFound;
                pchCTEntry += 4;
            case EPT_PROC32:
            {
                cb = krnlInterpretProlog32((char*)aImportTab[i].ulAddress);
                aImportTab[i].cbProlog = (char)cb;
                if (   cb >= 5                      /* 5(1st): size of jump instruction in the function prolog which jumps to my overloading function */
                    && cb + 5 <= OVERLOAD32_ENTRY   /* 5(2nd): size of jump instruction which jumps back to the original function after executing the prolog copied to the callTab entry for this function. */
                    && (   aImportTab[i].chOpcode == OPCODE_IGNORE
                        || aImportTab[i].chOpcode == *(unsigned char*)aImportTab[i].ulAddress)
                    )
                {
                    /*
                     * Copy function prolog which will be overwritten by the jmp to calltabl.
                     */
                    memcpy(pchCTEntry, (void*)aImportTab[i].ulAddress, (size_t)cb);

                    /*
                     * Make jump instruction which jumps from calltab to original function.
                     * 0xE9 <four bytes displacement>
                     * Note: the displacement is relative to the next instruction
                     */
                    pchCTEntry[cb] = 0xE9; /* jmp */
                    *(unsigned long*)(void*)&pchCTEntry[cb+1] = aImportTab[i].ulAddress + cb - (unsigned long)&pchCTEntry[cb+5];
                }
                else
                {   /* !fatal! - this could never happen really... */
                    #ifndef R3TST
                    INT3(); /* ipe - later! */
                    #endif
                    kprintf(("FATAL verify failed for procedure no.%d(%s) when rehooking it!\n",
                             i, aImportTab[i].achName));
                    INT3(); /* ipe - later! */
                    KLOGEXIT(ERROR_D32_IPE | (i << ERROR_D32_PROC_SHIFT));
                    return ERROR_D32_IPE | (i << ERROR_D32_PROC_SHIFT);
                }
                pchCTEntry += OVERLOAD32_ENTRY;
                break;
            }


            /*
             * 16-bit procedure overload.
             */
            case EPT_PROCNR16:    /* Not required */
                if (!aImportTab[i].fFound) /* set the f<name> flag. */
                {
                    aImportTab[i].ulAddress = auNopFuncs[i];
                    INT3();
                    break;
                }
                *(unsigned long *)(void*)pchCTEntry16 = aImportTab[i].fFound;
                pchCTEntry16 += 4;
            case EPT_PROC16:
            case EPT_PROCH16:
            {
                cb = krnlInterpretProlog16((char*)aImportTab[i].ulAddress);
                aImportTab[i].cbProlog = (char)cb;
                if (   cb >= 5                          /* 5:      size of a 16:16 jump which jumps to my overloading function */
                    && cb + 5 + 4 < OVERLOAD16_ENTRY    /* cb+5+4: size of a 16:16 jump which is added to the call tab and the far ptr. and far address at end */
                    && (   aImportTab[i].chOpcode == OPCODE_IGNORE
                        || aImportTab[i].chOpcode == *(unsigned char*)aImportTab[i].ulAddress)
                    )
                {
                    if (!EPT16ProcH(aImportTab[i]))
                        /*
                         * Copy function prolog which is to be overwritten.
                         */
                        memcpy(pchCTEntry16, (void*)aImportTab[i].ulAddress, (size_t)cb);
                    else
                        cb = 0;

                    /*
                     * Create far jump from calltab to original function.
                     * 0xEA <two byte target address> <two byte target selector>
                     */
                    pchCTEntry16[cb] = 0xEA; /* jmp far ptr */
                    *(unsigned short*)(void*)&pchCTEntry16[cb+1] = (unsigned short)(aImportTab[i].offObject + cb);
                    *(unsigned short*)(void*)&pchCTEntry16[cb+3] = aImportTab[i].usSel;

                    /*
                     * We store the far 16:16 pointer to the function in the last four
                     * bytes of the entry. Set them!
                     */
                    *(unsigned short*)(void*)&pchCTEntry16[OVERLOAD16_ENTRY-4] = (unsigned short)aImportTab[i].offObject;
                    *(unsigned short*)(void*)&pchCTEntry16[OVERLOAD16_ENTRY-2] = aImportTab[i].usSel;
                }
                else
                {   /* !fatal! - this could never happen really... */
                    #ifndef R3TST
                    INT3(); /* ipe - later! */
                    #endif
                    kprintf(("FATAL verify failed for procedure no.%d(%s) when rehooking it!\n",
                             i, aImportTab[i].achName));
                    INT3(); /* ipe - later! */
                    KLOGEXIT(ERROR_D32_IPE | (i << ERROR_D32_PROC_SHIFT));
                    return ERROR_D32_IPE | (i << ERROR_D32_PROC_SHIFT);
                }
                pchCTEntry16 += OVERLOAD16_ENTRY;
                break;
            }


            /*
             * 16/32-bit importe variable.
             * This is used by accessing the 32-bit flat address in the callTab.
             * callTab-entry + 4 holds the offset of the variable into the object.
             * callTab-entry + 8 holds the selector for the object. (These two fields is the 16:32-bit pointer to the variable.)
             * callTab-entry + a holds the 16-bit offset for the variable.
             * callTab-entry + c holds the selector for the object. (These two fields is the 16:16-bit pointer to the variable.)
             */
            case EPT_VARNR32:
            case EPT_VARNR16:
                if (!aImportTab[i].fFound)
                {
                    memset(pchCTEntry, 0, VARIMPORT_ENTRY);
                    pchCTEntry += VARIMPORT_ENTRY;
                    break;
                }
            case EPT_VAR32:
            case EPT_VAR16:
                aImportTab[i].cbProlog = (char)0;
                *(unsigned long*)(void*)&pchCTEntry[0] = aImportTab[i].ulAddress;
                *(unsigned long*)(void*)&pchCTEntry[4] = aImportTab[i].offObject;
                *(unsigned short*)(void*)&pchCTEntry[8] = aImportTab[i].usSel;
                *(unsigned short*)(void*)&pchCTEntry[0xa] = (unsigned short)aImportTab[i].offObject;
                *(unsigned short*)(void*)&pchCTEntry[0xc] = aImportTab[i].usSel;
                pchCTEntry += VARIMPORT_ENTRY;
                break;

            default:
                #ifndef R3TST
                INT3(); /* ipe - later! */
                #endif
                kprintf(("unsupported type. (procedure no.%d (%s), cb=%d)\n",
                         i, aImportTab[i].achName, cb));
                INT3(); /* ipe - later! */
                KLOGEXIT(ERROR_D32_IPE | (i << ERROR_D32_PROC_SHIFT));
                return ERROR_D32_IPE | (i << ERROR_D32_PROC_SHIFT);
        } /* switch - type */
    }   /* for */


    /*
     * Now switch the 16-bit calltab segment to a CODE segment.
     */
    #ifndef R3TST
    if (krnlMakeCalltab16CodeSegment())
    {
        kprintf(("krnlMakeCalltab16CodeSegment failed\n"));
        INT3();
    }
    #endif

    KLOGEXIT(0);
    return 0;
}


#ifdef R3TST
/**
 * -Ring-3 testing-
 * Changes the entries in aImportTab to point to their fake equivalents.
 * @returns void
 * @param   void
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Called before the aImportTab array is used/verified.
 */
VOID R3TstFixImportTab(VOID)
{
    KLOGENTRY0("void");
    int i;

    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        switch (aImportTab[i].fType & ~(EPT_NOT_REQ | EPT_WRAPPED))
        {
            case EPT_PROC32:
                if (aTstFakers[i].fObj != 1)
                    kprintf(("invalid segment config for entry %i. (PROC32)\n", i));
                break;
            case EPT_PROC16:
            case EPT_PROCH16:
                if (aTstFakers[i].fObj != 2)
                    kprintf(("invalid segment config for entry %i. (PROC16/PROCH16)\n", i));
                break;
            case EPT_VAR16:
            case EPT_VAR32:
                if (aTstFakers[i].fObj != 3 && aTstFakers[i].fObj != 4)
                    kprintf(("invalid segment config for entry %i. (VAR32/16)\n", i));
                break;
        } /* switch - type */

        aImportTab[i].ulAddress = aTstFakers[i].uAddress;
        aImportTab[i].chOpcode = *(char *)aImportTab[i].ulAddress;
        switch (aTstFakers[i].fObj)
        {
            case 1:
                aImportTab[i].usSel = GetSelectorCODE32();
                aImportTab[i].offObject = aTstFakers[i].uAddress - (unsigned)&CODE32START;
                break;
            case 2:
                aImportTab[i].usSel = GetSelectorCODE16();
                aImportTab[i].offObject = aTstFakers[i].uAddress - (unsigned)&CODE16START;
                break;
            case 3:
                aImportTab[i].usSel = GetSelectorDATA32();
                aImportTab[i].offObject = aTstFakers[i].uAddress - (unsigned)&DATA32START;
                break;
            case 4:
                aImportTab[i].usSel = GetSelectorDATA16();
                aImportTab[i].offObject = aTstFakers[i].uAddress - (unsigned)&DATA16START;
                break;
            default:
                kprintf(("invalid segment config for entry %i.\n", i));
        }
    } /* for */
    KLOGEXITVOID();
}
#endif


/**
 * Initiates the exported functions and MTE.
 * @returns 0 on success.
 *          -1 on failure.
 */
int krnlInit2(void)
{
    KLOGENTRY0("int");
    int rc = krnlInitExports();
    KLOGEXIT(rc);
    return rc;
}


/**
 * Initiates the structures needed for exporting.
 *
 * Upon successful return the fake DLL MTE is linked into the MTE list of the kernel.
 *
 * @returns 0 on success.
 *          -1 on failure.
 * @remark
 */
int krnlInitExports(void)
{
    KLOGENTRY0("int");
    PBYTE   pbType = (PBYTE)&KKL_EntryTabFixups[0];
    int     i;

    if (!pKrnlOTE)
    {
        kprintf(("Internal Processing Error: pKrnlOte is NULL\n"));
        KLOGEXIT(-1);
        return -1;
    }

    /*
     * Init the OTEs.
     */
    for (i = 0; i < 5; i++)
        KKL_ObjTab[i].ote_size -= KKL_ObjTab[i].ote_base;
    memcpy(&KKL_ObjTab_DosCalls[0], pKrnlOTE, pKrnlSMTE->smte_objcnt * sizeof(pKrnlOTE[0]));
    KKL_SwapMTE.smte_objcnt = pKrnlSMTE->smte_objcnt + 5;


    /*
     * Apply fixups.
     */
    while (*pbType)
    {
        PUSHORT pusOffset = (PUSHORT)(void*)(pbType + 1);
        struct b32_bundle * pBundle = (struct b32_bundle *)(*pusOffset + (unsigned)&KKL_EntryTab[0]);
        struct e32_entry  * pEntry = (struct e32_entry *)((unsigned)pBundle + sizeof(*pBundle));

        /*
         * Object number fix.
         */
        switch (*pbType)
        {
            case EXP_ABSOLUTE32:
            case EXP_32:
            case EXP_ORGPROC32:
                pBundle->b32_obj = (USHORT)(KKL_ObjTab[2].ote_sel == pBundle->b32_obj ? 3 : 4);
                break;
            case EXP_ABSOLUTE16:
            case EXP_16:
                pBundle->b32_obj = (USHORT)(KKL_ObjTab[1].ote_sel == pBundle->b32_obj ? 2 : 1);
                break;

            case EXP_PROC32:
            case EXP_VAR32:
            case EXP_VAR16:
            case EXP_PROC16:
                pBundle->b32_obj = (USHORT)(aImportTab[pEntry->e32_variant.e32_offset.offset16].iObject + 5); /* Use 16-bit offset! */
                break;

            case EXP_ORGPROC16:
                pBundle->b32_obj = 5;
                break;
        }


        /*
         * Addjust the entry points.
         */
        for (i = 0; i < pBundle->b32_cnt; i++)
        {
            switch (*pbType)
            {
                case EXP_ABSOLUTE16:
                    pEntry->e32_variant.e32_offset.offset16 =
                        *(PUSHORT)(pEntry->e32_variant.e32_offset.offset16
                                  + KKL_ObjTab[pBundle->b32_obj - 1].ote_base);
                    pEntry = (struct e32_entry *)((unsigned)pEntry + 3);
                    break;
                case EXP_ABSOLUTE32:
                    pEntry->e32_variant.e32_offset.offset32 = *(PULONG)pEntry->e32_variant.e32_offset.offset32;
                    pEntry = (struct e32_entry *)((unsigned)pEntry + 5);
                    break;
                case EXP_32:
                case EXP_ORGPROC32:
                    pEntry->e32_variant.e32_offset.offset32 -= KKL_ObjTab[pBundle->b32_obj - 1].ote_base;
                    pEntry = (struct e32_entry *)((unsigned)pEntry + 5);
                    break;
                case EXP_16:
                case EXP_ORGPROC16:
                    i = pBundle->b32_cnt; /* nothing to do! */
                    break;

                case EXP_PROC32:
                case EXP_VAR32:
                    pEntry->e32_variant.e32_offset.offset32 =
                        aImportTab[pEntry->e32_variant.e32_offset.offset32].offObject;
                    pEntry = (struct e32_entry *)((unsigned)pEntry + 5);
                    break;

                case EXP_VAR16:
                case EXP_PROC16:
                    pEntry->e32_variant.e32_offset.offset16 =
                        (USHORT)aImportTab[pEntry->e32_variant.e32_offset.offset16].offObject;
                    pEntry = (struct e32_entry *)((unsigned)pEntry + 3);
                    break;
            }

        } /*for*/

        /*
         * For absolute exports zero object.
         */
        if (*pbType == EXP_ABSOLUTE16 || *pbType == EXP_ABSOLUTE32)
            pBundle->b32_obj = 0;

        /* next fixup */
        pbType += 3;
    } /*while*/


    /*
     * Insert the MTE into the linked list of MTEs.
     */
    #ifndef R3TST                       /* Just for the moment. */
    if (pglobal_h == NULL || *pglobal_h == NULL || pglobal_l == NULL || *pglobal_l == NULL)
    {
        kprintf(("Internal Processing Error: *pglobal_h is NULL\n"));
        KLOGEXIT(-1);
        return -1;
    }
    VMCreatePseudoHandle(&kKrnlLibMTE, OWNER_ldrmte, &kKrnlLibMTE.mte_handle);
    kKrnlLibMTE.mte_link = (*pglobal_l)->mte_link;
    *pglobal_l = (*pglobal_l)->mte_link = &kKrnlLibMTE;
    #endif

    KLOGEXIT(0);
    return 0;
}

