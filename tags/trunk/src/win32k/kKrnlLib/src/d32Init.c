/* $Id: d32Init.c,v 1.2 2002-04-01 12:48:41 bird Exp $
 *
 * d32init.c - 32-bits init routines.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: d32Init.c,v 1.2 2002-04-01 12:48:41 bird Exp $";
#endif


/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#if  0
    #define kprintf2(a) kprintf
#else
    #define kprintf2(a) (void)0
#endif

#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define LDR_INCL_INITONLY
#define INCL_OS2KRNL_ALL
#define INCL_OS2KRNL_LDR
#define INCL_KKL_HEAP
#define INCL_KKL_MISC
#define INCL_KKL_LOG

#define DWORD   ULONG                   /* exe386.h fixes */
#define WORD    USHORT

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <exe386.h>
#include <string.h>

#include "OS2Krnl.h"
#include "kKrnlLib.h"

#include "devSegDf.h"
#include "options.h"
#include "dev1632.h"
#include "dev32.h"
#include "dev32Hlp.h"
#include "ProbKrnl.h"
#include "ProbKrnlErrors.h"

#ifdef R3TST
    #include "testcase.h"
#endif


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
PMTE        pKrnlMTE = NULL;
PSMTE       pKrnlSMTE = NULL;
POTE        pKrnlOTE = NULL;
extern char callTab[1];


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
ULONG          readnum(const char *pszNum);
#ifdef R3TST
PMTE           GetOS2KrnlMTETst(void);
#endif

/* Note! these two have to be visible to some assembly programs. */
ULONG _System R0Init(RP32INIT *pRpInit);
ULONG _System R0Addr32bit(ULONG ulParam, PFN pfn);


/* externs located in 16-bit data segement in ProbKrnl.c */
extern ULONG    _TKSSBase16;


/**
 * Ring-0, 32-bit, init function.
 * @returns   Status word.
 * @param     pRpInit  Pointer init request packet.
 * @sketch    Set TKSSBase32.
 *            Set default parameters.
 *            Parse command line options.
 *            Show (kprint) configuration.
 *            Init heap.
 *            Init ldr.
 *            Init procs. (overloaded ldr procedures)
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG R0Init(RP32INIT *pRpInit)
{
    KLOGENTRY1("USHORT","RP32INIT * pRpInit", pRpInit);
    char *      pszTmp2;
    char *      pszTmp;
    ULONG       ul;
    APIRET      rc;
    LOCKHANDLE  lockhandle;

    pulTKSSBase32 = (PULONG)_TKSSBase16;

    /*
     * Commandline options
     */
    kprintf(("Options start\n"));
    pszTmp = strpbrk(pRpInit->InitArgs, "-/");
    while (pszTmp != NULL)
    {
        pszTmp++; //skip [-/]
        switch (*pszTmp)
        {
            case 'c':
            case 'C': /* -C[1|2|3|4] or -Com:[1|2|3|4]  -  com-port no, def:-C2 */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
                    pszTmp2++;
                else
                    pszTmp2 = pszTmp + 1;
                ul = readnum(pszTmp2);
                switch (ul)
                {
                    case 1: options.usCom = OUTPUT_COM1; break;
                    case 2: options.usCom = OUTPUT_COM2; break;
                    case 3: options.usCom = OUTPUT_COM3; break;
                    case 4: options.usCom = OUTPUT_COM4; break;
                }
                break;

            case 'h':
            case 'H': /* Heap options */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
                {
                    ul = readnum(pszTmp2 + 1);
                    if (ul > 0x1000UL && ul < 0x2000000UL) /* 4KB < ul < 32MB */
                    {
                        if (strnicmp(pszTmp, "heapm", 5) == 0)
                            options.cbSwpHeapMax = ul;
                        else
                            options.cbSwpHeapInit = ul;
                    }
                }
                break;

            case 'l':
            case 'L': /* -L[..]<:|=| >[<Y..|E..| > | <N..|D..>] */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL
                    && (pszTmp2[1] == 'Y' ||pszTmp2[1] == 'y' || pszTmp2[1] == 'E' || pszTmp2[1] == 'e')
                    )
                    options.fLogging = TRUE;
                else
                    options.fLogging = FALSE;
                break;

            case 'q':
            case 'Q': /* quiet initialization */
                options.fQuiet = TRUE;
                break;

            case 'r':
            case 'R': /* ResHeap options */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
                {
                    ul = readnum(pszTmp2 + 1);
                    if (ul > 0x1000UL && ul < 0x700000UL) /* 4KB < ul < 7MB */
                    {
                        if (strnicmp(pszTmp, "resheapm", 8) == 0)
                            options.cbResHeapMax = ul;
                        else
                            options.cbResHeapInit = ul;
                    }
                }
                break;

            case 'v':
            case 'V': /* verbose initialization */
                options.fQuiet = FALSE;
                break;
        }
        pszTmp = strpbrk(pszTmp, "-/");
    }

    /* heap min/max corrections */
    if (options.cbSwpHeapInit > options.cbSwpHeapMax)
        options.cbSwpHeapMax = options.cbSwpHeapInit;
    if (options.cbResHeapInit > options.cbResHeapMax)
        options.cbResHeapMax = options.cbResHeapInit;

    /* Log option summary */
    #ifdef DEBUG
    kprintf(("Options - Summary - Start\n"));
    if (options.fQuiet)
        kprintf(("\tQuiet init\n"));
    else
        kprintf(("\tVerbose init\n"));

    if (options.fLogging)
        kprintf(("\tlogging enabled\n"));
    else
        kprintf(("\tlogging disabled\n"));
    kprintf(("\tCom port no.%03xh\n", options.usCom));

    kprintf(("\tKernel: v%d.%d  build %d%c type ",
                usVerMajor,
                usVerMinor,
                ulKernelBuild,
                (fKernel & KF_REV_MASK)
                    ? ((fKernel & KF_REV_MASK) >> KF_REV_SHIFT) + 'a'-1
                    : ' '
                ));
    if (fKernel & KF_SMP)
        kprintf(("SMP "));
    else if (fKernel & KF_W4)
        kprintf(("W4 "));
    else
        kprintf(("UNI "));
    if (fKernel & KF_DEBUG)
        kprintf(("DEBUG\n"));
    else
        kprintf(("\n"));

    kprintf(("\tcbSwpHeapInit=0x%08x  cbSwpHeapMax=0x%08x\n",
             options.cbSwpHeapInit, options.cbSwpHeapMax));
    kprintf(("\tcbResHeapInit=0x%08x  cbResHeapMax=0x%08x\n",
             options.cbResHeapInit, options.cbResHeapMax));
    kprintf(("Options - Summary - End\n"));
    #endif /* debug */
    /* end option summary */


    /*
     * init sub-parts
     */
    /* functionimports. */
    if ((rc = krnlInit()) != NO_ERROR)
    {
        KLOGEXIT(rc);
        return rc;
    }

    /* logging facility */
    if ((rc = kLogInit(0)) != NO_ERROR)
    {
        KLOGEXIT(rc);
        return rc;
    }

    /* logging for kKrnlLib */
    if ((rc = InitkKrnlLibLog()) != NO_ERROR)
    {
        KLOGEXIT(rc);
        return rc;
    }

    /* heap */
    if (heapInit(options.cbResHeapInit, options.cbResHeapMax,
                 options.cbSwpHeapInit, options.cbSwpHeapMax) != NO_ERROR)
    {
        KLOGEXIT(ERROR_D32_HEAPINIT_FAILED);
        return ERROR_D32_HEAPINIT_FAILED;
    }

    /* ... */

    /* fake MTE. */
    if ((rc = krnlInit2()) != NO_ERROR)
    {
        KLOGEXIT(rc);
        return rc;
    }

    /*
     * Lock the 32-bit objects/segments and 16-bit datasegment in memory
     */
    /* 32-bit code segment */
    memset(SSToDS(&lockhandle), 0, sizeof(lockhandle));
    rc = D32Hlp_VMLock2(&CODE32START,
                        ((unsigned)&CODE32END & ~0xFFF) - (unsigned)&CODE32START, /* Round down so we don't overlap with the next request. */
                        VMDHL_LONG,
                        SSToDS(&lockhandle));
    if (rc != NO_ERROR)
        kprintf(("code segment lock failed with with rc=%d\n", rc));

    /* 32-bit data segment */
    memset(SSToDS(&lockhandle), 0, sizeof(lockhandle));
    rc = D32Hlp_VMLock2(&DATA32START,
                        &EH_DATAEND - &DATA32START,
                        VMDHL_LONG | VMDHL_WRITE,
                        SSToDS(&lockhandle));
    if (rc != NO_ERROR)
        kprintf(("data segment lock failed with with rc=%d\n", rc));

    KLOGEXIT(NO_ERROR);
    return NO_ERROR;
}


/**
 * Reads a number (unsigned long integer) for a string.
 * @returns   number read, ~0UL on error / no number read.
 * @param     pszNum  Pointer to the string containing the number.
 * @status    competely implemented.
 * @author    knut st. osmundsen
 */
ULONG    readnum(const char *pszNum)
{
    KLOGENTRY1("ULONG","const char * pszNum", pszNum);
    ULONG ulRet = 0;
    ULONG ulBase = 10;
    int   i = 0;

    /* determin ulBase */
    if (*pszNum == '0')
        if (pszNum[1] == 'x' || pszNum[1] == 'X')
        {
            ulBase = 16;
            pszNum += 2;
        }
        else
        {
            ulBase = 8;
            i = 1;
        }

    /* read digits */
    while (ulBase == 16 ? (pszNum[i] >= '0' && pszNum[i] <= '9') || (pszNum[i] >= 'a' && pszNum[i] <= 'f') || (pszNum[i] >= 'A' && pszNum[i] <= 'F')
           : (pszNum[i] >= '0' && pszNum[i] <= (ulBase == 10 ? '9' : '7'))
           )
    {
        ulRet *= ulBase;
        if (ulBase <= 10)
            ulRet += pszNum[i] - '0';
        else
            ulRet += pszNum[i] - (pszNum[i] >= 'A' ? 'A' - 10 : (pszNum[i] >= 'a' ? 'a' + 9 : '0'));

        i++;
    }

    KLOGEXIT(i > 0 ? ulRet : ~0UL);
    return i > 0 ? ulRet : ~0UL;
}


/**
 * Get kernel OTEs
 * This function set pKrnlMTE, pKrnlSMTE and pKrnlOTE.
 * @returns   Strategy return code:
 *            STATUS_DONE on success.
 *            STATUS_DONE | STERR | errorcode on failure.
 * @param     pKrnlInfo  Pointer to output buffer.
 *                       If NULL only the three global variables are set.
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 * @remark    Called from IOCtl.
 *            WARNING! This function is called before the initroutine (R0INIT)!
 */
ULONG GetKernelInfo32(PKRNLINFO pKrnlInfo)
{
    KLOGENTRY1("ULONG","PKRNLINFO pKrnlInfo", pKrnlInfo);
    int     i;
    ULONG   rc;

    /* VerifyImportTab32 is called before the initroutine! */
    pulTKSSBase32 = (PULONG)_TKSSBase16;

    /* Find the kernel OTE table */
#ifndef R3TST
    pKrnlMTE = GetOS2KrnlMTE();
#else
    pKrnlMTE = GetOS2KrnlMTETst();
#endif
    if (pKrnlMTE != NULL)
    {
        pKrnlSMTE = pKrnlMTE->mte_swapmte;
        if (pKrnlSMTE != NULL)
        {
            if (pKrnlSMTE->smte_objcnt <= MAXKRNLOBJECTS)
            {
                pKrnlOTE = pKrnlSMTE->smte_objtab;
                if (pKrnlOTE != NULL)
                {
                    BOOL    fKrnlTypeOk;

                    /*
                     * Thats all?
                     */
                    if (pKrnlInfo == NULL)
                    {
                        KLOGEXIT(NO_ERROR);
                        return NO_ERROR;
                    }

                    pKrnlInfo->cObjects = (unsigned char)pKrnlSMTE->smte_objcnt;

                    /*
                     * Copy OTEs
                     */
                    for (i = 0; i < pKrnlInfo->cObjects; i++)
                    {
                        memcpy((void*)&pKrnlInfo->aObjects[i], &pKrnlOTE[i], sizeof(OTE));
                        kprintf2(("GetKernelInfo32: %d base=0x%08x size=0x%08x flags=0x%08x\n",
                                  i, pKrnlOTE[i].ote_base, pKrnlOTE[i].ote_size, pKrnlOTE[i].ote_flags));
                    }
                    rc = 0;

                    /*
                     * Search for internal revision stuff AND 'SAB KNL?' signature in the two first objects.
                     */
                    fKrnlTypeOk = FALSE;
                    pKrnlInfo->fKernel = 0;
                    pKrnlInfo->ulBuild = 0;
                    for (i = 0; i < 2 && pKrnlInfo->ulBuild == 0; i++)
                    {
                        const char *psz = (const char*)pKrnlOTE[i].ote_base;
                        const char *pszEnd = psz + pKrnlOTE[i].ote_size - 50; /* Last possible search position. */

                        while (psz < pszEnd)
                        {
                            if (strncmp(psz, "Internal revision ", 18) == 0 && (psz[18] >= '0' && psz[18] <= '9'))
                            {
                                int j;
                                kprintf2(("GetKernelInfo32: found internal revision: '%s'\n", psz));

                                /* skip to end of "Internal revision " string. */
                                psz += 18;

                                /* Read number*/
                                while ((*psz >= '0' && *psz <= '9') || *psz == '.')
                                {
                                    if (*psz != '.')
                                        pKrnlInfo->ulBuild = (unsigned short)(pKrnlInfo->ulBuild * 10 + (*psz - '0'));
                                    psz++;
                                }

                                /* Check if build number seems valid. */
                                if (   !(pKrnlInfo->ulBuild >=  8254 && pKrnlInfo->ulBuild <  8383) /* Warp 3 fp 32 -> fp 60 */
                                    && !(pKrnlInfo->ulBuild >=  9023 && pKrnlInfo->ulBuild <= 9036) /* Warp 4 GA -> fp 12 */
                                    && !(pKrnlInfo->ulBuild >= 14039 && pKrnlInfo->ulBuild < 14150) /* Warp 4.5 GA -> ?? */
                                    && !(pKrnlInfo->ulBuild >=  6600 && pKrnlInfo->ulBuild <= 6678) /* Warp 2.1x fix?? (just for fun!) */
                                      )
                                {
                                    kprintf(("GetKernelInfo32: info summary: Build %d is invalid - invalid fixpack?\n", pKrnlInfo->ulBuild));
                                    rc = ERROR_D32_INVALID_BUILD;
                                    break;
                                }

                                /* Check for any revision flag */
                                if ((*psz >= 'A' && *psz <= 'Z') || (*psz >= 'a' && *psz <= 'z'))
                                {
                                    pKrnlInfo->fKernel |= (USHORT)((*psz - (*psz >= 'a' ? 'a'-1 : 'A'-1)) << KF_REV_SHIFT);
                                    psz++;
                                }
                                if (*psz == ',') /* This is ignored! */
                                    *psz++;

                                /* If this is an Aurora/Warp 4.5 or Warp 3 kernel there is more info! */
                                if (psz[0] == '_' && (psz[1] == 'S' || psz[1] == 's'))  /* _SMP  */
                                    pKrnlInfo->fKernel |= KF_SMP;
                                else
                                    if (*psz != ','
                                        && (   (psz[0] == '_' && psz[1] == 'W' && psz[2] == '4')  /* _W4 */
                                            || (psz[0] == '_' && psz[1] == 'U' && psz[2] == 'N' && psz[3] == 'I' && psz[4] == '4')  /* _UNI4 */
                                            )
                                        )
                                    pKrnlInfo->fKernel |= KF_W4 | KF_UNI;
                                else
                                    pKrnlInfo->fKernel |= KF_UNI;


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
                                            pKrnlInfo->fKernel |= KF_DEBUG;
                                            break;
                                        }
                                        j++;
                                    }
                                }

                                /* Display info */
                                kprintf(("GetKernelInfo32: info summary: Build %d, fKernel=0x%x\n",
                                         pKrnlInfo->ulBuild, pKrnlInfo->fKernel));

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
                                    pKrnlInfo->fKernel |= KF_ALLSTRICT;
                                else if (psz[7] == 'B')
                                    pKrnlInfo->fKernel |= KF_HALFSTRICT;
                                else if (psz[7] != 'R')
                                    fKrnlTypeOk = FALSE;
                            }

                            /* next */
                            psz++;
                        } /* while loop searching for "Internal revision " */
                    } /* for loop on objects 0-1. */

                    /* Set error code if not found */
                    if (pKrnlInfo->ulBuild == 0)
                    {
                        rc = ERROR_D32_BUILD_INFO_NOT_FOUND;
                        kprintf(("GetKernelInfo32: Internal revision was not found!\n"));
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

    if (rc != NO_ERROR)
        kprintf(("GetKernelInfo32: failed. rc = %d\n", rc));

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
 * Forwarder for ASMR0Addr32bit which only purpose is to
 * initate a logging sequence.
 * @returns what ever the function returned.
 * @param   ulParam     User argument.
 * @param   pfn         Address (32-bit FLAT) of the function to call.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
ULONG R0Addr32bit(ULONG ulParam, PFN pfn)
{
    KLOGSTART2("ULONG", "ULONG ulParam, PFN pfn", ulParam, pfn);
    ULONG rc;

    rc = pfn(ulParam);

    KLOGSTOP(rc);
    return rc;
}
