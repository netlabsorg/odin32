/* $Id: d32init.c,v 1.19.4.13 2000-09-02 20:49:11 bird Exp $
 *
 * d32init.c - 32-bits init routines.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define MAXSIZE_PROLOG 0x18             /* Note that this must be synced with */
                                        /* the one used in calltab.asm.       */
#if  0                                  /* Enable this to have extra debug logging. */
    #define kprintf2(a) kprintf
#else
    #define kprintf2(a) (void)0
#endif

#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define LDR_INCL_INITONLY
#define INCL_OS2KRNL_ALL

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <string.h>

#include "devSegDf.h"
#include "OS2Krnl.h"
#include "options.h"
#include "dev1632.h"
#include "dev32.h"
#include "dev32hlp.h"
#include "probkrnl.h"
#include "log.h"
#include "asmutils.h"
#include "malloc.h"
#include "ldr.h"
#include "ldrCalls.h"
#include "macros.h"
#include "errors.h"

#ifdef R3TST
    #include "test.h"
#endif


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifdef DEBUG
static char * apszPE[] = {"FLAGS_PE_NOT", "FLAGS_PE_PE2LX", "FLAGS_PE_PE", "FLAGS_PE_MIXED", "!invalid!"};
static char * apszInfoLevel[] = {"INFOLEVEL_QUIET", "INFOLEVEL_ERROR", "INFOLEVEL_WARNING", "INFOLEVEL_INFO", "INFOLEVEL_INFOALL", "!invalid!"};
#endif
PMTE    pKrnlMTE = NULL;
PSMTE   pKrnlSMTE = NULL;
POTE    pKrnlOTE = NULL;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
 ULONG          readnum(const char *pszNum);
_Inline int     ModR_M_32bit(char bModRM);
_Inline int     ModR_M_16bit(char bModRM);
int             interpretFunctionProlog32(char *pach, BOOL fOverload);
int             interpretFunctionProlog16(char *pach, BOOL fOverload);
int             importTabInit(void);
#ifdef R3TST
PMTE            GetOS2KrnlMTETst(void);
void            R3TstFixImportTab(void);
#endif
PSZ SECCALL     nopSecPathFromSFN(SFN hFile);



/* externs located in 16-bit data segement in ProbKrnl.c */
extern ULONG    _TKSSBase16;
extern USHORT   _R0FlatCS16;
extern USHORT   _R0FlatDS16;

/* extern(s) located in calltab.asm */
extern char     callTab[NBR_OF_KRNLIMPORTS][MAXSIZE_PROLOG];

/* extern(s) located in mytkExecPgm.asm  */
extern char     mytkExecPgm;
extern char     mytkStartProcess;


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
USHORT _loadds _Far32 _Pascal R0Init32(RP32INIT *pRpInit)
{
    char *      pszTmp2;
    char *      pszTmp;
    ULONG       ul;
    APIRET      rc;
    LOCKHANDLE  lockhandle;

    pulTKSSBase32 = (PULONG)_TKSSBase16;

    /*---------------------*/
    /* commandline options */
    /*---------------------*/
    kprintf(("Options start\n"));
    pszTmp = strpbrk(pRpInit->InitArgs, "-/");
    while (pszTmp != NULL)
    {
        int cch;
        pszTmp++; //skip [-/]
        cch = strlen(pszTmp);
        switch (*pszTmp)
        {
            case 'c':
            case 'C': /* -C[1|2] or -Com:[1|2]  -  com-port no, def:-C2 */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
                    pszTmp2++;
                else
                    pszTmp2 = pszTmp + 1;
                options.usCom = (USHORT)(*pszTmp2 == '1' ? OUTPUT_COM1 : OUTPUT_COM2);
                break;

            case 'e':
            case 'E':/* ELF */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL
                    && (pszTmp2[1] == 'N' ||pszTmp2[1] == 'n' || pszTmp2[1] == 'D' || pszTmp2[1] == 'd')
                    )
                    options.fElf = FALSE;
                else
                    options.fElf = TRUE;
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

            case 'j':
            case 'J': /* -Java:<Yes|No> */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                options.fJava =
                    pszTmp2 != NULL
                    && (int)(pszTmp2-pszTmp) < cch-1
                    && (*pszTmp2 == ':' || *pszTmp2 == '=')
                    && (pszTmp2[1] == 'Y' || pszTmp2[1] == 'y');
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

            case 'n':
            case 'N': /* NoLoader */
                options.fNoLoader = TRUE;
                break;

            case 'p':
            case 'P': /* PE */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
                {
                    pszTmp2++;
                    if (strnicmp(pszTmp2, "pe2lx", 5) == 0)
                        options.fPE = FLAGS_PE_PE2LX;
                    else if (strnicmp(pszTmp2, "pe", 2) == 0)
                        options.fPE = FLAGS_PE_PE;
                    else if (strnicmp(pszTmp2, "mixed", 2) == 0)
                        options.fPE = FLAGS_PE_MIXED;
                    else if (strnicmp(pszTmp2, "not", 2) == 0)
                        options.fPE = FLAGS_PE_NOT;
                    else
                        kprintf(("R0Init32: invalid parameter -PE:...\n"));
                }
                else
                    kprintf(("R0Init32: invalid parameter -PE...\n"));
                break;

            case 'q':
            case 'Q': /* quiet initialization */
                options.fQuiet = TRUE;
                break;

            case 'r':
            case 'R': /* ResHeap options or REXX option */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (   (pszTmp[1] == 'E' || pszTmp[1] == 'e')
                    && (pszTmp[2] == 'X' || pszTmp[2] == 'x'))
                {   /* REXX */
                    options.fREXXScript =
                        pszTmp2 != NULL
                        && (int)(pszTmp2-pszTmp) < cch-1
                        && (*pszTmp2 == ':' || *pszTmp2 == '=')
                        && (pszTmp2[1] == 'Y' || pszTmp2[1] == 'y');
                }
                else
                {   /* ResHeap options */
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
                }
                break;

            case 's':
            case 'S': /* Sym:<filename> or Script:<Yes|No> or Smp */
                /* SMP kernel */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp[1] == 'c' || pszTmp[1] == 'C')
                {
                    options.fUNIXScript =
                        pszTmp2 != NULL
                        && (int)(pszTmp2-pszTmp) < cch-1
                        && (*pszTmp2 == ':' || *pszTmp2 == '=')
                        && (pszTmp2[1] == 'Y' || pszTmp2[1] == 'y');
                }
                break;

            case 'v':
            case 'V': /* verbose initialization */
                options.fQuiet = FALSE;
                break;

            case 'w':
            case 'W': /* ModuleBase info level; -W<n> or -Warning:<n> */
                if (pszTmp[1] >= '0' && pszTmp[1] <= '4')
                    options.ulInfoLevel = pszTmp[1] - '0';
                else
                {
                    pszTmp2 = strpbrk(pszTmp, ":=/- ");
                    if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
                        pszTmp2++;
                    else
                        pszTmp2 = pszTmp + 1;

                    if (*pszTmp2 >= '0' && *pszTmp2 <= '4')
                        options.ulInfoLevel = *pszTmp2 - '0';
                }
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

    kprintf(("\tKernel: v%d.%d  build %d  type ",
                options.usVerMajor,
                options.usVerMinor,
                options.ulBuild));
    if (options.fKernel & KF_SMP)
        kprintf(("SMP "));
    else if (options.fKernel & KF_W4)
        kprintf(("W4 "));
    else
        kprintf(("UNI "));
    if (options.fKernel & KF_DEBUG)
        kprintf(("DEBUG\n"));
    else
        kprintf(("\n"));

    kprintf(("\tfPE=%d (%s)\n",     options.fPE, apszPE[MIN(options.fPE, 5)]));
    kprintf(("\tulInfoLevel=%d (%s)\n", options.ulInfoLevel, apszInfoLevel[MIN(options.ulInfoLevel, 5)]));
    kprintf(("\tfElf=%d\n",         options.fElf));
    kprintf(("\tfUNIXScript=%d\n",  options.fUNIXScript));
    kprintf(("\tfREXXScript=%d\n",  options.fREXXScript));
    kprintf(("\tfJAVA=%d\n",        options.fJava));
    kprintf(("\tfNoLoader=%d\n",    options.fNoLoader));
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
    /* heap */
    if (heapInit(options.cbResHeapInit, options.cbResHeapMax,
                 options.cbSwpHeapInit, options.cbSwpHeapMax) != NO_ERROR)
        return STATUS_DONE | STERR | ERROR_I24_QUIET_INIT_FAIL;

    /* loader */
    if (!options.fNoLoader)
        if (ldrInit() != NO_ERROR)
            return STATUS_DONE | STERR | ERROR_I24_QUIET_INIT_FAIL;

    /* functionoverrides */
    if (!options.fNoLoader)
        if (importTabInit() != NO_ERROR)
            return STATUS_DONE | STERR | ERROR_I24_QUIET_INIT_FAIL;

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
    rc = D32Hlp_VMLock2(callTab,
                        &CONST32_ROEND - (char*)callTab,
                        VMDHL_LONG | VMDHL_WRITE,
                        SSToDS(&lockhandle));
    if (rc != NO_ERROR)
        kprintf(("data segment lock failed with with rc=%d\n", rc));

    /* 16-bit data segment - is this necessary? */
    memset(SSToDS(&lockhandle), 0, sizeof(lockhandle));
    rc = D32Hlp_VMLock2(&DATA16START,
                        &DATA16END - &DATA16START,
                        VMDHL_LONG | VMDHL_WRITE,
                        SSToDS(&lockhandle));
    if (rc != NO_ERROR)
        kprintf(("16-bit data segment lock failed with with rc=%d\n", rc));

    return STATUS_DONE;
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
USHORT _loadds _Far32 _Pascal GetKernelInfo32(PKRNLINFO pKrnlInfo)
{
    int     i;
    USHORT  usRc;

    /* VerifyImporTab32 is called before the initroutine! */
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
                    /*
                     * Thats all?
                     */
                    if (pKrnlInfo == NULL)
                        return NO_ERROR;

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
                    usRc = 0;

                    /*
                     * Search for internal revision stuff in the two first objects.
                     */
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
                                    && !(pKrnlInfo->ulBuild >= 14039 && pKrnlInfo->ulBuild < 14080) /* Warp 4.5 GA -> fp 40 */
                                    && !(pKrnlInfo->ulBuild >=  6600 && pKrnlInfo->ulBuild <= 6678) /* Warp 2.1x fix?? (just for fun!) */
                                      )
                                {
                                    kprintf(("GetKernelInfo32: info summary: Build %d is invalid - invalid fixpack?\n", pKrnlInfo->ulBuild));
                                    usRc = ERROR_D32_INVALID_BUILD;
                                    break;
                                }

                                /* If this is an Aurora/Warp 4.5 or Warp 3 kernel there is more info! */
                                if ((psz[0] != ',' && psz[1] == '_' && (psz[2] == 'S' || psz[2] == 's'))  /* F_SMP */
                                    || (psz[0] == '_' && (psz[1] == 'S' || psz[1] == 's'))  /* _SMP  */
                                    )
                                    pKrnlInfo->fKernel = KF_SMP;
                                else
                                    if (*psz != ','
                                        && (   (psz[0] == '_' && psz[1] == 'W' && psz[2] == '4')  /* _W4 */
                                            || (psz[1] == '_' && psz[2] == 'W' && psz[3] == '4')  /* A_W4 */
                                            || (psz[0] == '_' && psz[1] == 'U' && psz[2] == 'N' && psz[3] == 'I' && psz[4] == '4')  /* _UNI4 */
                                            || (psz[1] == '_' && psz[2] == 'U' && psz[3] == 'N' && psz[4] == 'I' && psz[5] == '4')  /* A_UNI4 */
                                            )
                                        )
                                    pKrnlInfo->fKernel = KF_W4 | KF_UNI;
                                else
                                    pKrnlInfo->fKernel = KF_UNI;


                                /* Check if its a debug kernel (look for DEBUG at start of object 3-5) */
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

                                /* Display info */
                                kprintf(("GetKernelInfo32: info summary: Build %d, fKernel=%d\n",
                                         pKrnlInfo->ulBuild, pKrnlInfo->fKernel));

                                /* Break out */
                                break;
                            }

                            /* next */
                            psz++;
                        } /* while loop searching for "Internal revision " */
                    } /* for loop on objects 0-1. */

                    /* Set error code if not found */
                    if (pKrnlInfo->ulBuild == 0)
                    {
                        usRc = ERROR_D32_BUILD_INFO_NOT_FOUND;
                        kprintf(("GetKernelInfo32: Internal revision was not found!\n"));
                    }
                }
                else
                    usRc = ERROR_D32_NO_OBJECT_TABLE;
            }
            else
                usRc = ERROR_D32_TOO_MANY_OBJECTS;
        }
        else
            usRc = ERROR_D32_NO_SWAPMTE;
    }
    else
        usRc = ERROR_D32_GETOS2KRNL_FAILED;

    if (usRc != NO_ERROR)
        kprintf(("GetKernelInfo32: failed. usRc = %d\n", usRc));

    return (USHORT)(usRc | (usRc != NO_ERROR ? STATUS_DONE | STERR : STATUS_DONE));
}



/**
 * Functions which cacluates the instructionsize given a ModR/M byte.
 * @returns   Number of bytes to add to cb and pach.
 * @param     bModRM  ModR/M byte.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int ModR_M_32bit(char bModRM)
{
    if ((bModRM & 0xc0) == 0x80  /* ex. mov ax,[ebp+11145543h] */
        || ((bModRM & 0xc0) == 0 && (bModRM & 0x07) == 5)) /* ex. mov ebp,[0ff231234h] */
    {   /* 32-bit displacement */
        return 5 + ((bModRM & 0x7) == 0x4); // + SIB
    }
    else if ((bModRM & 0xc0) == 0x40) /* ex. mov ecx,[esi]+4fh */
    {   /* 8-bit displacement */
        return 2 + ((bModRM & 0x7) == 0x4); // + SIB
    }
    /* no displacement (only /r byte) */
    return 1;
}


/**
 * Functions which cacluates the instructionsize given a ModR/M byte.
 * @returns   Number of bytes to add to cb and pach.
 * @param     bModRM  ModR/M byte.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int ModR_M_16bit(char bModRM)
{
    if ((bModRM & 0xc0) == 0x80  /* ex. mov ax,[ebp+11145543h] */
        || ((bModRM & 0xc0) == 0 && (bModRM & 0x07) == 5)) /* ex. mov ebp,[0ff231234h] */
    {   /* 16-bit displacement */
        return 4;
    }
    else if ((bModRM & 0xc0) == 0x40) /* ex. mov ecx,[esi]+4fh */
    {   /* 8-bit displacement */
        return 2;
    }
    /* no displacement (only /r byte) */
    return 1;
}





/**
 * 32-bit! Interpret function prolog to find where to jmp back.
 * @returns   Length of prolog need to be copied - which is also the offset of
 *            where the jmp instr should be placed.
 *            On error it returns 0.
 * @param     pach       Pointer to prolog.
 * @param     fOverload  TRUE:  Function is to be overloaded.
 *                       FALSE: Function is to be imported.
 */
int interpretFunctionProlog32(char *pach, BOOL fOverload)
{
    int cb = -3;
    kprintf2(("interpretFunctionProlog32(0x%08x, %d):\n"
              "\t%02x %02x %02x %02x - %02x %02x %02x %02x\n"
              "\t%02x %02x %02x %02x - %02x %02x %02x %02x\n",
              pach, fOverload,
              pach[0], pach[1], pach[2], pach[3], pach[4], pach[5], pach[6], pach[7],
              pach[8], pach[9], pach[10],pach[11],pach[12],pach[13],pach[14],pach[15]));

    /*
     * check for the well known prolog (the only that is supported now)
     * which is:
     *     push ebp
     *     mov ebp,esp
     *  or
     *     push ebp
     *     mov eax, dword ptr [xxxxxxxx]
     *
     * These are allowed when not overloading:
     *     mov eax, imm32
     *     jmp short
     *  or
     *     mov eax, imm32
     *     push ebp
     *  or
     *     mov ecx, r/m32
     *  or
     *     jmp dword
     *  or
     *     sub esp, imm8
     *  or
     *     call ptr16:32
     *  or
     *     enter imm16, imm8    (2.1x)
     *  or
     *     mov eax, imm32       (2.1x)
     *     <anything>
     *  or
     *     xor r32, r/m32
     *  or
     *     mov eax, msoff32
     *
     */
    if ((pach[0] == 0x55 && (pach[1] == 0x8b || pach[1] == 0xa1)) /* the two first prologs */
        ||
        (pach[0] == 0xB8 && (pach[5] == 0xEB || pach[5] == 0x55) && !fOverload) /* the two next prologs */
        ||
        (pach[0] == 0x8B && !fOverload) /* the next prolog */
        ||
        (pach[0] == 0xFF && !fOverload) /* the next prolog */
        ||
        (pach[0] == 0x83 && !fOverload) /* the next prolog */
        ||
        (pach[0] == 0x9a && !fOverload) /* the next prolog */
        ||
        (pach[0] == 0xc8)               /* the next prolog */
        ||
        (pach[0] == 0xB8 && !fOverload) /* the next prolog */
        ||
        (pach[0] == 0x33 && !fOverload) /* the next prolog */
        ||
        (pach[0] == 0xa1 && !fOverload) /* last prolog */
        )
    {
        BOOL fForce = FALSE;
        int  cbWord = 4;
        cb = 0;
        while (cb < 5 || fForce)                  /* 5 is the size of a jump instruction. */
        {
            int cb2;
            if (!fForce && cbWord != 4)
                cbWord = 4;
            fForce = FALSE;
            switch (*pach)
            {
                /* simple one byte prefixes */
                case 0x2e:              /* cs segment override */
                case 0x36:              /* ss segment override */
                case 0x3e:              /* ds segment override */
                case 0x26:              /* es segment override */
                case 0x64:              /* fs segment override */
                case 0x65:              /* gs segment override */
                    fForce = TRUE;
                    break;

                case 0x66:              /* 16 bit */
                    fForce = TRUE;
                    cbWord = 2;
                    break;

                /* simple one byte instructions */
                case 0x50:              /* push ax */
                case 0x51:              /* push cx */
                case 0x52:              /* push dx */
                case 0x53:              /* push bx */
                case 0x54:              /* push sp */
                case 0x55:              /* push bp */
                case 0x56:              /* push si */
                case 0x57:              /* push di */
                    break;

                /* simple two byte instructions */
                case 0xb0:              /* mov al, imm8 */
                case 0xb1:              /* mov cl, imm8 */
                case 0xb2:              /* mov dl, imm8 */
                case 0xb3:              /* mov bl, imm8 */
                case 0xb4:              /* mov ah, imm8 */
                case 0xb5:              /* mov ch, imm8 */
                case 0xb6:              /* mov dh, imm8 */
                case 0xb7:              /* mov bh, imm8 */
                case 0x2c:              /* sub al, imm8 */
                case 0x34:              /* xor al, imm8 */
                case 0x3c:              /* cmp al, imm8 */
                case 0x6a:              /* push <byte> */
                case 0xa0:              /* mov al, moffs8 */
                case 0xa2:              /* mov moffs8, al */
                    pach++;
                    cb++;
                    break;

                /* simple five byte instructions */
                case 0xb8:              /* mov eax, imm32 */
                case 0xb9:              /* mov ecx, imm32 */
                case 0xba:              /* mov edx, imm32 */
                case 0xbb:              /* mov ebx, imm32 */
                case 0xbc:              /* mov esx, imm32 */
                case 0xbd:              /* mov ebx, imm32 */
                case 0xbe:              /* mov esi, imm32 */
                case 0xbf:              /* mov edi, imm32 */
                case 0x2d:              /* sub eax, imm32 */
                case 0x35:              /* xor eax, imm32 */
                case 0x3d:              /* cmp eax, imm32 */
                case 0x68:              /* push <dword> */
                case 0xa1:              /* mov eax, moffs16 */
                case 0xa3:              /* mov moffs16, eax */
                    pach += cbWord;
                    cb += cbWord;
                    break;

                /* complex sized instructions -  "/r" */
                case 0x30:              /* xor r/m8,  r8 */
                case 0x31:              /* xor r/m32, r32 */
                case 0x32:              /* xor r8,  r/m8 */
                case 0x33:              /* xor r32, r/m32 */
                case 0x38:              /* cmp r/m8, r8 */
                case 0x39:              /* cmp r/m32, r32 */
                case 0x3a:              /* cmp r8, r/m8 */
                case 0x3b:              /* cmp r32, r/m32 */
                case 0x28:              /* sub r/m8, r8 */
                case 0x29:              /* sub r/m32, r32 */
                case 0x2a:              /* sub r8, r/m8 */
                case 0x2b:              /* sub r32, r/m32 */
                case 0x8b:              /* mov /r */
                case 0x8d:              /* lea /r */
                    cb += cb2 = ModR_M_32bit(pach[1]);
                    pach += cb2;
                    break;

                /* complex sized instruction - "/5 ib" */
                case 0x80:              /* 5: sub r/m8, imm8  7: cmp r/m8, imm8 */
                case 0x83:              /* 5: sub r/m32, imm8 7: cmp r/m32, imm8 */
                    if ((pach[1] & 0x38) == (5<<3)
                        || (pach[1] & 0x38) == (7<<3)
                        )
                    {
                        cb += cb2 = 1 + ModR_M_32bit(pach[1]); /* 1 is the size of the imm8 */
                        pach += cb2;
                    }
                    else
                    {
                        kprintf(("interpretFunctionProlog32: unknown instruction (-3) 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        return -3;
                    }
                    break;

                /* complex sized instruction - "/digit id" */
                case 0x81:              /* sub r/m32, imm32 + more instructions! */
                    if ((pach[1] & 0x38) == (5<<3)       /* sub r/m32, imm32  */
                        || (pach[1] & 0x38) == (7<<3)    /* cmp r/m32, imm32  */
                        )
                    {
                        cb += cb2 = cbWord + ModR_M_32bit(pach[1]); /* cbWord is the size of the imm32/imm16 */
                        pach += cb2;
                    }
                    else
                    {
                        kprintf(("interpretFunctionProlog32: unknown instruction (-2) 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        return -2;
                    }
                    break;

                case 0x9a:              /* call ptr16:32 */
                    cb += cb2 = 6;
                    pach += cb2;
                    break;

                case 0xc8:              /* enter imm16, imm8 */
                    cb += cb = 3;
                    pach += cb2;
                    break;

                /*
                 * jmp /digit
                 */
                case 0xff:
                    cb += cb2 = cbWord + ModR_M_32bit(pach[1]); /* cbWord is the size of the imm32/imm16 */
                    pach += cb2;
                    break;

                default:
                    kprintf(("interpretFunctionProlog32: unknown instruction 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                    return 0;
            }
            pach++;
            cb++;
        }
    }
    else
    {
        kprintf(("interpretFunctionProlog32: unknown prolog start. 0x%x 0x%x 0x%x 0x%x 0x%x\n",
                 pach[0], pach[1], pach[2], pach[3], pach[4]));
        cb = 0;
    }
    return cb;
}


/**
 * 16-bit! Interpret function prolog to find where to jmp back.
 * @returns   Length of prolog need to be copied - which is also the offset of
 *            where the jmp instr should be placed.
 *            On error it returns 0.
 * @param     pach       Pointer to prolog.
 * @param     fOverload  TRUE:  Function is to be overloaded.
 *                       FALSE: Function is to be imported.
 */
int interpretFunctionProlog16(char *pach, BOOL fOverload)
{
    int cb = -7;

    kprintf2(("interpretFunctionProlog16(0x%08x, %d):\n"
              "\t%02x %02x %02x %02x - %02x %02x %02x %02x\n"
              "\t%02x %02x %02x %02x - %02x %02x %02x %02x\n",
              pach, fOverload,
              pach[0], pach[1], pach[2], pach[3], pach[4], pach[5], pach[6], pach[7],
              pach[8], pach[9], pach[10],pach[11],pach[12],pach[13],pach[14],pach[15]));
    /*
     * Check for the well known prolog (the only that is supported now)
     * which is:
     *     push 2
     */
    if (*pach == 0x6A)                  /* push 2 (don't check for the 2) */
    {
        BOOL fForce;
        int  cOpPrefix = 0;
        cb = 0;
        while (cb < 8 || fForce)        /* 8 is the size of a 66h prefixed far jump instruction. */
        {
            int cb2;
            fForce = FALSE;
            switch (*pach)
            {
                case 0x06:              /* push es */
                case 0x0e:              /* push cs */
                case 0x1e:              /* push ds */
                case 0x16:              /* push ss */
                    break;

                case 0x0f:              /* push gs and push fs */
                    if (pach[1] != 0xA0 && pach[1] != 0xA8)
                    {
                        kprintf(("interpretFunctionProlog16: unknown instruction 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        return -11;
                    }
                    pach++;
                    cb++;
                    break;

                case 0x50:              /* push ax */
                case 0x51:              /* push cx */
                case 0x52:              /* push dx */
                case 0x53:              /* push bx */
                case 0x54:              /* push sp */
                case 0x55:              /* push bp */
                case 0x56:              /* push si */
                case 0x57:              /* push di */
                    break;

                case 0x2e:              /* cs segment override */
                case 0x36:              /* ss segment override */
                case 0x3e:              /* ds segment override */
                case 0x26:              /* es segment override */
                case 0x64:              /* fs segment override */
                case 0x65:              /* gs segment override */
                    fForce = TRUE;
                    if (cOpPrefix > 0)
                        cOpPrefix++;
                    break;

                case 0x66:
                    cOpPrefix = 2;      /* it's decremented once before it's used. */
                    fForce = TRUE;
                    break;

                case 0x6a:              /* push <byte> */
                    pach++;
                    cb++;
                    break;

                case 0x68:              /* push <word> */
                    if (cOpPrefix > 0)
                    {
                        pach += 2;
                        cb += 2;
                    }
                    pach += 2;
                    cb += 2;
                    break;

                case 0x8b:              /* mov /r */
                    if ((pach[1] & 0xc0) == 0x80  /* ex. mov ax,bp+1114h */
                        || ((pach[1] & 0xc0) == 0 && (pach[1] & 0x7) == 6)) /* ex. mov bp,0ff23h */
                    {   /* 16-bit displacement */
                        if (cOpPrefix > 0)
                        {
                            pach += 2;
                            cb += 2;
                        }
                        pach += 3;
                        cb += 3;
                    }
                    else
                        if ((pach[1] & 0xc0) == 0x40) /* ex. mov ax,[si]+4fh */
                    {   /* 8-bit displacement */
                        pach += 2;
                        cb += 2;
                    }
                    else
                    {   /* no displacement (only /r byte) */
                        pach++;
                        cb++;
                    }
                    break;

                /* complex sized instruction - "/5 ib" */
                case 0x80:              /* 5: sub r/m8, imm8  7: cmp r/m8, imm8 */
                case 0x83:              /* 5: sub r/m16, imm8 7: cmp r/m16, imm8 */
                    if ((pach[1] & 0x38) == (5<<3)
                        || (pach[1] & 0x38) == (7<<3)
                        )
                    {
                        cb += cb2 = 1 + ModR_M_16bit(pach[1]); /* 1 is the size of the imm8 */
                        pach += cb2;
                    }
                    else
                    {
                        kprintf(("interpretFunctionProlog16: unknown instruction (-3) 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        return -3;
                    }
                    break;


                default:
                    kprintf(("interpretFunctionProlog16: unknown instruction 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                    return 0;
            }
            pach++;
            cb++;
            if (cOpPrefix > 0)
                cOpPrefix--;
        }
    }

    fOverload = fOverload;
    return cb;
}


/**
 * Verifies the aImportTab.
 * @returns   0 if ok. !0 if not ok.
 * @remark    Called from IOCtl.
 *            WARNING! This function is called before the initroutine (R0INIT)!
 */
USHORT _loadds _Far32 _Pascal VerifyImportTab32(void)
{
    USHORT  usRc;
    int     i;
    int     cb;
    int     cbmin;

    /* VerifyImporTab32 is called before the initroutine! */
    pulTKSSBase32 = (PULONG)_TKSSBase16;

    /* Check that pKrnlOTE is set */
    usRc = GetKernelInfo32(NULL);
    if (usRc != NO_ERROR)
        return (USHORT)(STATUS_DONE | STERR | (usRc & STECODE));

    /*
     * Verify aImportTab.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        /*
         * Debug info
         */
        kprintf2(("VerifyImportTab32: procedure no.%d is being checked: %s addr=0x%08x iObj=%d offObj=%d\n",
                  i, &aImportTab[i].achName[0], aImportTab[i].ulAddress,
                  aImportTab[i].iObject, aImportTab[i].offObject));

        /* Verify that it is found */
        if (!aImportTab[i].fFound)
        {
            if (aImportTab[i].fType & EPT_NOT_REQ)
                continue;
            else
            {
                kprintf(("VerifyImportTab32: procedure no.%d was not fFound!\n", i));
                return STATUS_DONE | STERR | ERROR_D32_PROC_NOT_FOUND;
            }
        }

        /* Verify read/writeable. */
        if (aImportTab[i].iObject >= pKrnlSMTE->smte_objcnt                                /* object index valid? */
            || aImportTab[i].ulAddress < pKrnlOTE[aImportTab[i].iObject].ote_base          /* address valid? */
            || aImportTab[i].ulAddress + 16 > (pKrnlOTE[aImportTab[i].iObject].ote_base +
                                                pKrnlOTE[aImportTab[i].iObject].ote_size)  /* address valid? */
            || aImportTab[i].ulAddress - aImportTab[i].offObject
               != pKrnlOTE[aImportTab[i].iObject].ote_base                                 /* offObject ok?  */
            )
        {
            kprintf(("VerifyImportTab32: procedure no.%d has an invalid address or object number.!\n"
                     "                   %s  addr=0x%08x iObj=%d offObj=%d\n",
                     i, &aImportTab[i].achName[0], aImportTab[i].ulAddress,
                     aImportTab[i].iObject, aImportTab[i].offObject));
            return STATUS_DONE | STERR | ERROR_D32_INVALID_OBJ_OR_ADDR;
        }


        #ifndef R3TST
        if (aImportTab[i].ulAddress < 0xff400000UL)
        {
            kprintf(("VerifyImportTab32: procedure no.%d has an invalid address, %#08x!\n",
                     i, aImportTab[i].ulAddress));
            return STATUS_DONE | STERR | ERROR_D32_INVALID_ADDRESS;
        }
        #endif

        switch (aImportTab[i].fType & ~(EPT_BIT_MASK | EPT_NOT_REQ))
        {
            case EPT_PROC:
            case EPT_PROCIMPORT:
                /*
                 * Verify known function prolog.
                 */
                if (EPT32BitEntry(aImportTab[i]))
                {
                    cb = interpretFunctionProlog32((char*)aImportTab[i].ulAddress,
                                                   aImportTab[i].fType == EPT_PROC32);
                    cbmin = 5; /* Size of the jump instruction */
                }
                else
                {
                    cb = interpretFunctionProlog16((char*)aImportTab[i].ulAddress,
                                                   aImportTab[i].fType == EPT_PROC16);
                    cbmin = 7; /* Size of the far jump instruction */
                }

                /*
                 * Check result of the function prolog interpretations.
                 */
                if (cb <= 0 || cb + cbmin >= MAXSIZE_PROLOG)
                {   /* failed, too small or too large. */
                    kprintf(("VerifyImportTab32: verify failed for procedure no.%d (cb=%d)\n", i, cb));
                    return STATUS_DONE | STERR | ERROR_D32_TOO_INVALID_PROLOG;
                }
                break;

            case EPT_VARIMPORT:
                /* do nothing! */
                break;

            default:
                kprintf(("VerifyImportTab32: invalid type/type not implemented\n",i));
                Int3(); /* temporary fix! */
                return STATUS_DONE | STERR | ERROR_D32_NOT_IMPLEMENTED;
        }
    }

    return STATUS_DONE;
}


/**
 * Initiates the overrided functions.
 * @returns   NO_ERROR on success. !0 on error.
 */
int importTabInit(void)
{
    /* This table must be updated with the overloading functions.
     * It should also hold NOP functions for functions which are of the
     * not required type.
     */
    static unsigned auFuncs[NBR_OF_KRNLIMPORTS] =
    {
        (unsigned)myldrRead,            /* 0 */
        (unsigned)myldrOpen,            /* 1 */
        (unsigned)myldrClose,           /* 2 */
        (unsigned)myLDRQAppType,        /* 3 */
        (unsigned)myldrEnum32bitRelRecs,/* 4 */
        0,                              /* 5 */
        0,                              /* 6 */
        0,                              /* 7 */
        0,                              /* 8 */
        0,                              /* 9 */
        0,                              /* 10 */
        0,                              /* 11 */
        0,                              /* 12 */
        (unsigned)&mytkExecPgm,         /* 13 */
        (unsigned)&mytkStartProcess,    /* 14 */
        0,                              /* 15 */
        0,                              /* 16 */
        0,                              /* 17 */
        0,                              /* 18 */
        0,                              /* 19 */
        (unsigned)myldrOpenPath,        /* 20 */
        0,                              /* 21 */
        0,                              /* 22 */
        0,                              /* 23 */
        0,                              /* 24 */
        0,                              /* 25 */
        0,                              /* 26 */
        0,                              /* 27 */
        0,                              /* 28 */
        0,                              /* 29 */
        0,                              /* 30 */
        0,                              /* 31 */
        0,                              /* 32 */
        0,                              /* 33 */
        0,                              /* 34 */
        0,                              /* 35 */
        0,                              /* 36 */
        0,                              /* 37 */
        0,                              /* 38 */
        0,                              /* 39 */
        0,                              /* 40 */
        (unsigned)nopSecPathFromSFN,    /* 41 */
    };
    int i;
    int cb;
    int cbmin;

#ifdef R3TST
    R3TstFixImportTab();
#endif

    /*
     * verify proctable
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        /* EPT_VARIMPORTs are skipped */
        if ((aImportTab[i].fType & ~EPT_BIT_MASK) == EPT_VARIMPORT)
            continue;
        /* EPT_NOT_REQ which is not found are set pointing to the nop function provided. */
        if (!aImportTab[i].fFound && (aImportTab[i].fType & EPT_NOT_REQ))
        {
            aImportTab[i].ulAddress = auFuncs[i];
            continue;
        }

        if (EPT32BitEntry(aImportTab[i]))
        {
            cb = interpretFunctionProlog32((char*)aImportTab[i].ulAddress, aImportTab[i].fType == EPT_PROC32);
            cbmin = 5; /* Size of the jump instruction */
        }
        else
        {
            cb = interpretFunctionProlog16((char*)aImportTab[i].ulAddress, aImportTab[i].fType == EPT_PROC16);
            cbmin = 7; /* Size of the far jump instruction */
        }
        if (cb <= 0 || cb + cbmin >= MAXSIZE_PROLOG)
        {
            kprintf(("ImportTabInit: Verify failed for procedure no.%d, cb=%d\n", i, cb));
            return ERROR_D32_VERIFY_FAILED;
        }
    }

    /*
     * rehook / import
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        switch (aImportTab[i].fType & ~EPT_NOT_REQ)
        {
            /*
             * 32-bit procedure overload.
             * The overloading procedure is found in the auFuncs table (at the same index
             *   as the overloaded procedure has in aImportTab).
             * The overloaded procedure is called by issuing a call to the callTab entry.
             */
            case EPT_PROC32:
            {
                cb = interpretFunctionProlog32((char*)aImportTab[i].ulAddress, TRUE);
                aImportTab[i].cbProlog = (char)cb;
                if (cb >= 5 && cb + 5 < MAXSIZE_PROLOG) /* 5(1st): size of jump instruction in the function prolog which jumps to my overloading function */
                {                                       /* 5(2nd): size of jump instruction which jumps back to the original function after executing the prolog copied to the callTab entry for this function. */
                    /*
                     * Copy function prolog which will be overwritten by the jmp to calltabl.
                     */
                    memcpy(callTab[i], (void*)aImportTab[i].ulAddress, (size_t)cb);

                    /*
                     * Make jump instruction which jumps from calltab to original function.
                     * 0xE9 <four bytes displacement>
                     * Note: the displacement is relative to the next instruction
                     */
                    callTab[i][cb] = 0xE9; /* jmp */
                    *(unsigned long*)(void*)&callTab[i][cb+1] = aImportTab[i].ulAddress + cb - (unsigned long)&callTab[i][cb+5];

                    /*
                     * Jump from original function to my function - an cli(?) could be needed here
                     */
                    *(char*)aImportTab[i].ulAddress = 0xE9; /* jmp */
                    *(unsigned long*)(aImportTab[i].ulAddress + 1) = auFuncs[i] - (aImportTab[i].ulAddress + 5);
                }
                else
                {   /* !fatal! - this could never happen really... */
                    kprintf(("ImportTabInit: FATAL verify failed for procedure no.%d when rehooking it!\n", i));
                    Int3(); /* ipe - later! */
                    return ERROR_D32_VERIFY_FAILED;
                }
                break;
            }


            /*
             * 16-bit procedure overload.
             * Currently disabled due to expected problems when calltab is a 32-bit segment.
             */
            case EPT_PROC16:
            {
                kprintf(("ImportTabInit: Overloading 16-bit procedures are not supported yet!!! Calltable in 32-bit segment!\n", i));
                Int3();

                cb = interpretFunctionProlog16((char*)aImportTab[i].ulAddress, TRUE);
                aImportTab[i].cbProlog = (char)cb;
                if (cb >= 8 && cb + 7 < MAXSIZE_PROLOG) /* 8: size of a 16:32 jump which jumps to my overloading function (prefixed with 66h in a 16-bit segment) */
                {                                       /* 7: size of a 16:32 jump which is added to the call tab */
                    /*
                     * Copy function prolog which is to be overwritten.
                     */
                    memcpy(callTab[i], (void*)aImportTab[i].ulAddress, (size_t)cb);

                    /*
                     * Create far jump from calltab to original function.
                     * 0xEA <four byte target address> <two byte target selector>
                     */
                    callTab[i][cb] = 0xEA; /* jmp far ptr */
                    *(unsigned long*)(void*)&callTab[i][cb+1] = aImportTab[i].offObject;
                    *(unsigned short*)(void*)&callTab[i][cb+5] = aImportTab[i].usSel;

                    /*
                     * jump from original function to my function - an cli(?) could be needed here
                     * 0x66 0xEA <four byte target address> <two byte target selector>
                     */
                    *(char*)(aImportTab[i].ulAddress    ) = 0x66;    /* operandsize prefix */
                    *(char*)(aImportTab[i].ulAddress + 1) = 0xEA;    /* jmp far ptr */
                    *(unsigned long*)(aImportTab[i].ulAddress + 2) = auFuncs[i];   /* FIXME? */
                    *(unsigned short*)(aImportTab[i].ulAddress + 6) = _R0FlatCS16; /* FIXME */
                }
                else
                {   /* !fatal! - this could never happen really... */
                    kprintf(("ImportTabInit: FATAL verify failed for procedure no.%d when rehooking it!\n", i));
                    Int3(); /* ipe - later! */
                    return ERROR_D32_VERIFY_FAILED;
                }
                break;
            }


            /*
             * 32-bit imported procedure
             * This is called by issuing a near call to the callTab entry.
             */
            case EPT_PROCIMPORT32:
            {
                cb = interpretFunctionProlog32((char*)aImportTab[i].ulAddress, FALSE);
                aImportTab[i].cbProlog = (char)cb;
                if (cb > 0) /* Since no prolog part is copied to the function table, it's ok as long as the prolog has been recognzied. */
                {
                    /*
                     * Make jump instruction which jumps from calltab to original function.
                     * 0xE9 <four bytes displacement>
                     * Note: the displacement is relative to the next instruction
                     */
                    callTab[i][0] = 0xE9; /* jmp */
                    *(unsigned*)(void*)&callTab[i][1] = aImportTab[i].ulAddress - (unsigned)&callTab[i][5];
                }
                else
                {   /* !fatal! - this should never really happen... */
                    kprintf(("ImportTabInit: FATAL verify failed for procedure no.%d when importing it!\n", i));
                    Int3(); /* ipe - later! */
                    return ERROR_D32_VERIFY_FAILED;
                }
                break;
            }


            /*
             * 16-bit imported procedure.
             * This is called by issuing a far call to the calltab entry.
             */
            case EPT_PROCIMPORT16:
            {
                cb = interpretFunctionProlog16((char*)aImportTab[i].ulAddress, FALSE);
                aImportTab[i].cbProlog = (char)cb;
                if (cb > 0) /* Since no prolog part is copied to the function table, it's ok as long as the prolog has been recognzied. */
                {
                    /*
                     * Create far jump from calltab to original function.
                     * 0xEA <four byte target address> <two byte target selector>
                     */
                    callTab[i][0] = 0xEA; /* jmp far ptr */
                    *(unsigned long*)(void*)&callTab[i][1] = aImportTab[i].offObject;
                    *(unsigned short*)(void*)&callTab[i][5] = aImportTab[i].usSel;
                }
                else
                {   /* !fatal! - this should never really happen... */
                    kprintf(("ImportTabInit: FATAL verify failed for procedure no.%d when importing it!\n", i));
                    Int3(); /* ipe - later! */
                    return ERROR_D32_VERIFY_FAILED;
                }
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
            case EPT_VARIMPORT32:
            case EPT_VARIMPORT16:
                aImportTab[i].cbProlog = (char)0;
                *(unsigned long*)(void*)&callTab[i][0] = aImportTab[i].ulAddress;
                *(unsigned long*)(void*)&callTab[i][4] = aImportTab[i].offObject;
                *(unsigned short*)(void*)&callTab[i][8] = aImportTab[i].usSel;
                *(unsigned short*)(void*)&callTab[i][0xa] = (unsigned short)aImportTab[i].offObject;
                *(unsigned short*)(void*)&callTab[i][0xc] = aImportTab[i].usSel;
                break;

            default:
                kprintf(("ImportTabInit: unsupported type. (procedure no.%d, cb=%d)\n", i, cb));
                Int3(); /* ipe - later! */
                return ERROR_D32_VERIFY_FAILED;
        } /* switch - type */
    }   /* for */

    return NO_ERROR;
}


#ifdef R3TST
/**
 * Creates a fake kernel MTE, SMTE and OTE for use while testing in Ring3.
 * @returns Pointer to the fake kernel MTE.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
PMTE GetOS2KrnlMTETst(void)
{
    static MTE    KrnlMTE;
    static SMTE   KrnlSMTE;

    KrnlMTE.mte_swapmte = &KrnlSMTE;
    KrnlSMTE.smte_objtab = &aKrnlOTE[0];
    KrnlSMTE.smte_objcnt = cObjectsFake;

    return &KrnlMTE;
}

/**
 * -Ring-3 testing-
 * Changes the entries in aImportTab to point to their fake equivalents.
 * @returns void
 * @param   void
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark  Called before the aImportTab array is used/verified.
 */
VOID R3TstFixImportTab(VOID)
{
    struct _TstFaker
    {
        unsigned   uAddress;
        int        fObj;                   /* 1 = CODE32, 2 = CODE16, 3 = DATA32, 4 = DATA16 */
    }
    aTstFakers[NBR_OF_KRNLIMPORTS] =
    {
        {(unsigned)fakeldrRead,             1},
        {(unsigned)fakeldrOpen,             1},
        {(unsigned)fakeldrClose,            1},
        {(unsigned)fakeLDRQAppType,         1},
        {(unsigned)fakeldrEnum32bitRelRecs, 1},
        {(unsigned)fakeIOSftOpen,           1},
        {(unsigned)fakeIOSftClose,          1},
        {(unsigned)fakeIOSftTransPath,      1},
        {(unsigned)fakeIOSftReadAt,         1},
        {(unsigned)fakeIOSftWriteAt,        1},
        {(unsigned)fakeSftFileSize,         1},
        {(unsigned)fakeVMAllocMem,          1},
        {(unsigned)fakeVMGetOwner,          1},
        {(unsigned)fakeg_tkExecPgm,         1},
        {(unsigned)fake_tkStartProcess,     1},
        {(unsigned)fakef_FuStrLenZ,         2},
        {(unsigned)fakef_FuStrLen,          2},
        {(unsigned)fakef_FuBuff,            2},
        {(unsigned)fakeVMObjHandleInfo,     1},
        {(unsigned)fakeldrASMpMTEFromHandle,1},
        {(unsigned)fakeldrOpenPath,         1},
        {(unsigned)fakeLDRClearSem,         1},
        {(unsigned)fakeldrFindModule,       1},
        {(unsigned)fakeKSEMRequestMutex,    1},
        {(unsigned)fakeKSEMReleaseMutex,    1},
        {(unsigned)fakeKSEMQueryMutex,      1},
        {(unsigned)fakeKSEMInit,            1},
        {(unsigned)&fakeLDRSem,             3},
        {(unsigned)&fakeLDRLibPath,         3},
        {(unsigned)fakeTKSuBuff,            1},
        {(unsigned)fakeTKFuBuff,            1},
        {(unsigned)fakeTKFuBufLen,          1},
        {(unsigned)fakeldrValidateMteHandle,1},
        {(unsigned)&fakepTCBCur,            4},
        {(unsigned)&fakepPTDACur,           4},
        {(unsigned)&fakeptda_start,         4},
        {(unsigned)&fakeptda_environ,       4},
        {(unsigned)&fakeptda_ptdasem,       4},
        {(unsigned)&fakeptda_module,        4},
        {(unsigned)&fakeptda_pBeginLIBPATH, 4},
        {(unsigned)&fakeldrpFileNameBuf,    3},
        {(unsigned)&fakeSecPathFromSFN,     3}
    };
    int i;

    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        switch (aImportTab[i].fType)
        {
            case EPT_PROC32:
                if (aTstFakers[i].fObj != 1)
                    kprintf(("R3TstFixImportTab: invalid segment config for entry %i. (PROC32)\n", i));
                break;
            case EPT_PROCIMPORT32:
                if (aTstFakers[i].fObj != 1)
                    kprintf(("R3TstFixImportTab: invalid segment config for entry %i. (PROCIMPORT32)\n", i));
                break;
            case EPT_PROCIMPORT16:
                if (aTstFakers[i].fObj != 2)
                    kprintf(("R3TstFixImportTab: invalid segment config for entry %i. (PROCIMPORT16)\n", i));
                break;
            case EPT_VARIMPORT32:
            case EPT_VARIMPORT16:
                if (aTstFakers[i].fObj != 3 && aTstFakers[i].fObj != 4)
                    kprintf(("R3TstFixImportTab: invalid segment config for entry %i. (VARIMPORT32/16)\n", i));
                break;
        } /* switch - type */

        aImportTab[i].ulAddress = aTstFakers[i].uAddress;
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
                kprintf(("R3TstFixImportTab: invalid segment config for entry %i.\n", i));
        }
    } /* for */
}
#endif

/**
 * Dummy nop function if SecPathFromSFN isn't found.
 */
PSZ SECCALL nopSecPathFromSFN(SFN hFile)
{
    NOREF(hFile);
    return NULL;
}