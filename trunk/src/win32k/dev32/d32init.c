/* $Id: d32init.c,v 1.18 2000-04-17 01:56:48 bird Exp $
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
#define static                          /* just to make all symbols visible in the kernel debugger.  */
#if  0                                  /* Enable this to have extra debug logging. */
    #define kprintf2(a) kprintf
#else
    #define kprintf2(a) (void)0
#endif


#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define LDR_INCL_INITONLY

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <string.h>

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


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifdef DEBUG
static char * apszPE[] = {"FLAGS_PE_NOT", "FLAGS_PE_PE2LX", "FLAGS_PE_PE", "FLAGS_PE_MIXED", "!invalid!"};
static char * apszInfoLevel[] = {"INFOLEVEL_QUIET", "INFOLEVEL_ERROR", "INFOLEVEL_WARNING", "INFOLEVEL_INFO", "INFOLEVEL_INFOALL", "!invalid!"};
#endif
static PMTE    pKrnlMTE = NULL;
static PSMTE   pKrnlSMTE = NULL;
static POTE    pKrnlOTE = NULL;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static ULONG        readnum(const char *pszNum);
_Inline int         ModR_M_32bit(char bModRM);
static int          interpretFunctionProlog32(char *pach, BOOL fOverload);
static int          interpretFunctionProlog16(char *pach, BOOL fOverload);
static int          ImportTabInit(void);


/* externs located in 16-bit data segement in ProbKrnl.c */
extern ULONG    _TKSSBase16;
extern USHORT   _R0FlatCS16;
extern USHORT   _R0FlatDS16;

/* extern(s) located in calltab.asm */
extern char     callTab[NBR_OF_KRNLIMPORTS][MAXSIZE_PROLOG];

/* extern(s) located in mytkExecPgm.asm  */
extern char     mytkExecPgm;


#ifndef DEBUGR3
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
            case 'C': /* -C[1|2] - com-port no, def:-C2 */
                switch (pszTmp[1])
                {
                    case '1':
                        options.usCom = OUTPUT_COM1;
                        break;

                    case '2':
                    default:
                        options.usCom = OUTPUT_COM2;
                }
                break;

            case 'e':
            case 'E':/* ELF */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (int)(pszTmp2-pszTmp) < cch-1
                    && (pszTmp2[1] == 'N' ||pszTmp2[1] == 'n' || pszTmp2[1] == 'D' || pszTmp2[1] == 'd')
                    )
                    options.fElf = FALSE;
                else
                    options.fElf = TRUE;
                break;

            case 'h':
            case 'H': /* Heap options */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (int)(pszTmp2-pszTmp) < cch-1
                    && (*pszTmp2 == ':' || *pszTmp2 == '='))
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
                if (pszTmp2 != NULL && (int)(pszTmp2-pszTmp) < cch-1
                    && (pszTmp2[1] == 'N' ||pszTmp2[1] == 'n' || pszTmp2[1] == 'D' || pszTmp2[1] == 'd')
                    )
                    options.fLogging = FALSE;
                else
                    options.fLogging = TRUE;
                break;

            case 'n':
            case 'N': /* NoLoader */
                options.fNoLoader = TRUE;
                break;

            case 'p':
            case 'P': /* PE */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (int)(pszTmp2-pszTmp) < cch-1
                    && (*pszTmp2 == ':' || *pszTmp2 == '='))
                {
                    pszTmp++;
                    if (strnicmp(pszTmp, "pe2lx", 5) == 0)
                        options.fPE = FLAGS_PE_PE2LX;
                    else if (strnicmp(pszTmp, "pe", 2) == 0)
                        options.fPE = FLAGS_PE_PE;
                    else if (strnicmp(pszTmp, "mixed", 2) == 0)
                        options.fPE = FLAGS_PE_MIXED;
                    else if (strnicmp(pszTmp, "not", 2) == 0)
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
            case 'R': /* ResHeap options */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (int)(pszTmp2-pszTmp) < cch-1
                    && (*pszTmp2 == ':' || *pszTmp2 == '='))
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

            case 's':
            case 'S': /* Sym:<filename> or Script:<Yes|No> or Smp */
                /* SMP kernel */
                if (pszTmp[1] == 'm' || pszTmp[1] == 'M')
                    options.fKernel = KF_SMP;
                else
                {
                    if (pszTmp[1] == 'c' || pszTmp[1] == 'C')
                    {
                        pszTmp2 = strpbrk(pszTmp, ":=/- ");
                        options.fScript = pszTmp2 != NULL && (int)(pszTmp2-pszTmp) < cch-1
                            && (*pszTmp2 == ':' || *pszTmp2 == '=')
                            && (pszTmp2[1] == 'Y' || pszTmp2[1] == 'y');
                    }
                }
                break;

            case 'u':
            case 'U': /* UNI kernel */
                options.fKernel = KF_UNI;
                break;

            case 'v':
            case 'V': /* verbose initialization */
                options.fQuiet = FALSE;
                break;

            case 'w':
            case 'W':
                if (pszTmp[1] >= '0' && pszTmp[1] <= '4')
                    options.ulInfoLevel = pszTmp[1] - '0';
                else
                {
                    pszTmp2 = strpbrk(pszTmp, ":=/- ");
                    if (pszTmp2 != NULL && (int)(pszTmp2-pszTmp) < cch-1
                        && (*pszTmp2 == ':' || *pszTmp2 == '=')
                        && pszTmp2[1] >= '0' && pszTmp2[1] <= '4'
                        )
                        options.ulInfoLevel = pszTmp2[1] - '0';
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

    /* Transfer version and build number from 16-bit probkrnl.c */
    options.ulBuild    = _usBuild;
    options.usVerMajor = _usVerMajor;
    options.usVerMinor = _usVerMinor;

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
    kprintf(("\tCom port no.%d\n", options.usCom));

    kprintf(("\tKernel: ver%d.%d  build %d  type %s\n",
                options.usVerMajor,
                options.usVerMinor,
                options.ulBuild,
                (options.fKernel & KF_SMP) ? "SMP" : "UNI"
              ));
    kprintf(("\tfPE=%d (%s)\n", options.fPE, apszPE[MIN(options.fPE, 5)]));
    kprintf(("\tulInfoLevel=%d (%s)\n", options.ulInfoLevel, apszInfoLevel[MIN(options.ulInfoLevel, 5)]));
    kprintf(("\tfElf=%d\n", options.fElf));
    kprintf(("\tfScript=%d\n", options.fScript));
    kprintf(("\tfNoLoader=%d\n", options.fNoLoader));
    kprintf(("\tcbSwpHeapInit=0x%08x  cbSwpHeapMax=0x%08x\n",
             options.cbSwpHeapInit, options.cbSwpHeapMax));
    kprintf(("\tcbResHeapInit=0x%08x  cbResHeapMax=0x%08x\n",
             options.cbSwpHeapInit, options.cbSwpHeapMax));
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
        if (ImportTabInit() != NO_ERROR)
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
static ULONG    readnum(const char *pszNum)
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
    pKrnlMTE = GetOS2KrnlMTE();
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
                    pKrnlInfo->usBuild = 0;
                    for (i = 0; i < 2 && pKrnlInfo->usBuild == 0; i++)
                    {
                        const char *psz = (const char*)pKrnlOTE[i].ote_base;
                        const char *pszEnd = psz + pKrnlOTE[i].ote_size;

                        while (psz + 100 < pszEnd)
                        {
                            if (strncmp(psz, "Internal revision ", 18) == 0 && (psz[18] >= '0' && psz[18] <= '9'))
                            {
                                int j;
                                kprintf2(("GetOTEs32: found internal revision: '%s'\n", psz));

                                /* skip to end of "Internal revision " string. */
                                psz += 18;

                                /* Read number*/
                                while ((*psz >= '0' && *psz <= '9') || *psz == '.')
                                {
                                    if (*psz != '.')
                                        pKrnlInfo->usBuild = (unsigned short)(pKrnlInfo->usBuild * 10 + (*psz - '0'));
                                    psz++;
                                }

                                /* Check if build number seems valid. */
                                if (   !(pKrnlInfo->usBuild >=  8254 && pKrnlInfo->usBuild <  8383) /* Warp 3 fp 32 -> fp 60 */
                                    && !(pKrnlInfo->usBuild >=  9023 && pKrnlInfo->usBuild <= 9036) /* Warp 4 GA -> fp 12 */
                                    && !(pKrnlInfo->usBuild >= 14039 && pKrnlInfo->usBuild < 14080) /* Warp 4.5 GA -> fp 40 */
                                      )
                                {
                                    kprintf(("GetOTEs32: info summary: Build %d is invalid - invalid fixpack?\n", pKrnlInfo->usBuild));
                                    usRc = 6;
                                    break;
                                }

                                /* If this is an Aurora/Warp 4.5 or Warp 3 kernel there is more info! */
                                if (*psz != ','
                                    && (   (psz[1] == '_' && (psz[2] == 'S' || psz[2] == 's'))  /* F_SMP */
                                        || (psz[0] == '_' && (psz[1] == 'S' || psz[1] == 's'))  /* _SMP  */
                                       )
                                    )
                                    pKrnlInfo->fchType = TYPE_SMP;
                                else
                                    if (*psz != ','
                                            || (psz[0] == '_'
                                                && (psz[1] == 'W' || psz[1] == 'w') && psz[2] == '4')  /* _W4  */
                                        )
                                    pKrnlInfo->fchType = TYPE_W4;
                                else
                                    pKrnlInfo->fchType = TYPE_UNI;


                                /* Check if its a debug kernel (look for DEBUG at start of object 3-5) */
                                j = 3;
                                pKrnlInfo->fDebug = FALSE;
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
                                        pKrnlInfo->fDebug = TRUE;
                                        break;
                                    }
                                    j++;
                                }

                                /* Display info */
                                kprintf(("GetOTEs32: info summary: Build %d, fchType=%d, fDebug=%d\n",
                                         pKrnlInfo->usBuild, pKrnlInfo->fchType, pKrnlInfo->fDebug));

                                /* Break out */
                                break;
                            }

                            /* next */
                            psz++;
                        } /* while loop searching for "Internal revision " */
                    } /* for loop on objects 0-1. */

                    /* Set error code if not found */
                    if (pKrnlInfo->usBuild == 0)
                    {
                        usRc = 5;
                        kprintf(("GetOTEs32: Internal revision was not found!\n"));
                    }
                }
                else
                    usRc = 4;
            }
            else
                usRc = 3;
        }
        else
            usRc = 2;
    }
    else
        usRc = 1;


    if (usRc != 0)
        kprintf(("GetOTEs32: failed. usRc = %d\n", usRc));

    return (USHORT)(usRc | (usRc != NO_ERROR ? STATUS_DONE | STERR : STATUS_DONE));
}
#endif /* !DEBUGR3*/



/**
 * Functions which cacluates the instructionsize given a ModR/M byte.
 * @returns   Number of bytes to add to cb and pach.
 * @param     bModRM  ModR/M byte.
 * @status    completely implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
_Inline int ModR_M_32bit(char bModRM)
{
    if ((bModRM & 0xc0) == 0x80  /* ex. mov ax,[ebp+11145543h] */
        || ((bModRM & 0xc0) == 0 && (bModRM & 0x07) == 5)) /* ex. mov ebp,[0ff231234h] */
    {   /* 32-bit displacement */
        return 5;
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
static int interpretFunctionProlog32(char *pach, BOOL fOverload)
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
     *     mov eax, msoff32
     *
     */
    if ((pach[0] == 0x55 && (pach[1] == 0x8b || pach[1] == 0xa1)) /* two first prologs */
        ||
        (pach[0] == 0xB8 && (pach[5] == 0xEB || pach[5] == 0x55 ) && !fOverload) /* two next prologs */
        ||
        (pach[0] == 0xa1 && !fOverload) /* last prolog */
        )
    {
        BOOL fForce;
        cb = 0;
        while (cb < 5 || fForce)                  /* 5 is the size of a jump instruction. */
        {
            int cb2;
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
                    pach += 4;
                    cb += 4;
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
                    if ((pach[1] & 0x7) == 4 && (pach[1] & 0xc0) != 0xc0) /* invalid instruction!?! */
                        return -1;
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
                        cb += cb2 = 4 + ModR_M_32bit(pach[1]); /* 4 is the size of the imm32 */
                        pach += cb2;
                    }
                    else
                    {
                        kprintf(("interpretFunctionProlog32: unknown instruction (-2) 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                        return -2;
                    }
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
        kprintf(("interpretFunctionProlog32: unknown prolog start. 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
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
static int interpretFunctionProlog16(char *pach, BOOL fOverload)
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
            fForce = FALSE;
            switch (*pach)
            {
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


#ifndef DEBUGR3
/**
 * Verifies the aImportTab.
 * @returns   0 if ok. !0 if not ok.
 * @remark    Called from IOCtl.
 *            WARNING! This function is called before the initroutine (R0INIT)!
 */
USHORT _loadds _Far32 _Pascal VerifyImportTab32(void)
{
    int i;
    int cb;
    int cbmin;

    /* VerifyImporTab32 is called before the initroutine! */
    pulTKSSBase32 = (PULONG)_TKSSBase16;

    /* Check that pKrnlOTE is set */
    if (GetKernelInfo32(NULL) != NO_ERROR)
        return STATUS_DONE | STERR | 1;

    /*
     * Verify aImportTab.
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        /*
         * Debug info
         */
        kprintf2(("VerifyImportTab32: procedure no.%d is being checked: %s addr=0x%08x iObj=%d offObj=%d\n",
                  i, &_aImportTab[i].achName[0], _aImportTab[i].ulAddress,
                  _aImportTab[i].iObject, _aImportTab[i].offObject));

        /* Verify that it is found */
        if (!_aImportTab[i].fFound)
        {
            kprintf(("VerifyImportTab32: procedure no.%d was not fFound!\n", i));
            return STATUS_DONE | STERR | 2;
        }

        /* Verify read/writeable. */
        if (_aImportTab[i].iObject >= pKrnlSMTE->smte_objcnt                                /* object index valid? */
            || _aImportTab[i].ulAddress < pKrnlOTE[_aImportTab[i].iObject].ote_base         /* address valid? */
            || _aImportTab[i].ulAddress + 16 > (pKrnlOTE[_aImportTab[i].iObject].ote_base +
                                                pKrnlOTE[_aImportTab[i].iObject].ote_size)  /* address valid? */
            || _aImportTab[i].ulAddress - _aImportTab[i].offObject
               != pKrnlOTE[_aImportTab[i].iObject].ote_base                                 /* offObject ok?  */
            )
        {
            kprintf(("VerifyImportTab32: procedure no.%d has an invalid address or object number.!\n"
                     "                   %s  addr=0x%08x iObj=%d offObj=%d\n",
                     i, &_aImportTab[i].achName[0], _aImportTab[i].ulAddress,
                     _aImportTab[i].iObject, _aImportTab[i].offObject));
            return STATUS_DONE | STERR | 3;
        }



        if (_aImportTab[i].ulAddress < 0xffe00000UL)
        {
            kprintf(("VerifyImportTab32: procedure no.%d has an invalid address, %#08x!\n",
                     i, _aImportTab[i].ulAddress));
            return STATUS_DONE | STERR | 4;
        }

        switch (_aImportTab[i].fType & ~EPT_BIT_MASK)
        {
            case EPT_PROC:
            case EPT_PROCIMPORT:
                /*
                 * Verify known function prolog.
                 */
                if (EPT32BitEntry(_aImportTab[i]))
                {
                    cb = interpretFunctionProlog32((char*)_aImportTab[i].ulAddress,
                                                   _aImportTab[i].fType == EPT_PROC32);
                    cbmin = 5; /* Size of the jump instruction */
                }
                else
                {
                    cb = interpretFunctionProlog16((char*)_aImportTab[i].ulAddress,
                                                   _aImportTab[i].fType == EPT_PROC16);
                    cbmin = 7; /* Size of the far jump instruction */
                }

                /*
                 * Check result of the function prolog interpretations.
                 */
                if (cb <= 0 && cb + cbmin >= MAXSIZE_PROLOG)
                {   /* failed, too small or too large. */
                    kprintf(("VerifyImportTab32: verify failed for procedure no.%d (cd=%d)\n", i, cb));
                    return STATUS_DONE | STERR | 5;
                }
                break;

            case EPT_VARIMPORT:
                /* do nothing! */
                break;

            default:
                kprintf(("VerifyImportTab32: only EPT_PROC is implemented\n",i));
                Int3(); /* temporary fix! */
                return STATUS_DONE | STERR | 6;
        }
    }

    return STATUS_DONE;
}


/**
 * Initiates the overrided functions.
 * @returns   NO_ERROR on success. !0 on error.
 */
static int importTabInit(void)
{
    /* This table must be updated with the overloading functions. */
    static unsigned auFuncs[NBR_OF_KRNLIMPORTS] =
    {
        (unsigned)myldrRead,
        (unsigned)myldrOpen,
        (unsigned)myldrClose,
        (unsigned)myLDRQAppType,
        (unsigned)myldrEnum32bitRelRecs,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        (unsigned)&mytkExecPgm,
        0,
        0,
        0,
        0
    };

    int i;
    int cb;
    int cbmin;

    /*
     * verify proctable
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        /* EPT_VARIMPORTs are skipped */
        if ((_aImportTab[i].fType & ~EPT_BIT_MASK) == EPT_VARIMPORT)
            continue;

        if (EPT32BitEntry(_aImportTab[i]))
        {
            cb = interpretFunctionProlog32((char*)_aImportTab[i].ulAddress, _aImportTab[i].fType == EPT_PROC32);
            cbmin = 5; /* Size of the jump instruction */
        }
        else
        {
            cb = interpretFunctionProlog16((char*)_aImportTab[i].ulAddress, _aImportTab[i].fType == EPT_PROC16);
            cbmin = 7; /* Size of the far jump instruction */
        }
        if (cb <= 0 || cb + cbmin >= MAXSIZE_PROLOG)
        {
            kprintf(("ImportTabInit: Verify failed for procedure no.%d, cb=%d\n", i, cb));
            return 1;
        }
    }

    /*
     * rehook / import
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        switch (_aImportTab[i].fType)
        {
            /*
             * 32-bit procedure overload.
             * The overloading procedure is found in the auFuncs table (at the same index
             *   as the overloaded procedure has in aImportTab).
             * The overloaded procedure is called by issuing a call to the callTab entry.
             */
            case EPT_PROC32:
            {
                cb = interpretFunctionProlog32((char*)_aImportTab[i].ulAddress, TRUE);
                _aImportTab[i].cbProlog = (char)cb;
                if (cb >= 5 && cb + 5 < MAXSIZE_PROLOG) /* 5(1st): size of jump instruction in the function prolog which jumps to my overloading function */
                {                                       /* 5(2nd): size of jump instruction which jumps back to the original function after executing the prolog copied to the callTab entry for this function. */
                    /*
                     * Copy function prolog which will be overwritten by the jmp to calltabl.
                     */
                    memcpy(callTab[i], (void*)_aImportTab[i].ulAddress, (size_t)cb);

                    /*
                     * Make jump instruction which jumps from calltab to original function.
                     * 0xE9 <four bytes displacement>
                     * Note: the displacement is relative to the next instruction
                     */
                    callTab[i][cb] = 0xE9; /* jmp */
                    *(unsigned long*)(void*)&callTab[i][cb+1] = _aImportTab[i].ulAddress + cb - (unsigned long)&callTab[i][cb+5];

                    /*
                     * Jump from original function to my function - an cli(?) could be needed here
                     */
                    *(char*)_aImportTab[i].ulAddress = 0xE9; /* jmp */
                    *(unsigned long*)(_aImportTab[i].ulAddress + 1) = auFuncs[i] - (_aImportTab[i].ulAddress + 5);
                }
                else
                {   /* !fatal! - this could never happen really... */
                    kprintf(("ImportTabInit: FATAL verify failed for procedure no.%d when rehooking it!\n",i));
                    Int3(); /* ipe - later! */
                    return 1;
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

                cb = interpretFunctionProlog16((char*)_aImportTab[i].ulAddress, TRUE);
                _aImportTab[i].cbProlog = (char)cb;
                if (cb >= 8 && cb + 7 < MAXSIZE_PROLOG) /* 8: size of a 16:32 jump which jumps to my overloading function (prefixed with 66h in a 16-bit segment) */
                {                                       /* 7: size of a 16:32 jump which is added to the call tab */
                    /*
                     * Copy function prolog which is to be overwritten.
                     */
                    memcpy(callTab[i], (void*)_aImportTab[i].ulAddress, (size_t)cb);

                    /*
                     * Create far jump from calltab to original function.
                     * 0xEA <four byte target address> <two byte target selector>
                     */
                    callTab[i][cb] = 0xEA; /* jmp far ptr */
                    *(unsigned long*)(void*)&callTab[i][cb+1] = _aImportTab[i].offObject;
                    *(unsigned short*)(void*)&callTab[i][cb+5] = _aImportTab[i].usSel;

                    /*
                     * jump from original function to my function - an cli(?) could be needed here
                     * 0x66 0xEA <four byte target address> <two byte target selector>
                     */
                    *(char*)(_aImportTab[i].ulAddress    ) = 0x66;    /* operandsize prefix */
                    *(char*)(_aImportTab[i].ulAddress + 1) = 0xEA;    /* jmp far ptr */
                    *(unsigned long*)(_aImportTab[i].ulAddress + 2) = auFuncs[i];   /* FIXME? */
                    *(unsigned short*)(_aImportTab[i].ulAddress + 6) = _R0FlatCS16; /* FIXME */
                }
                else
                {   /* !fatal! - this could never happen really... */
                    kprintf(("ImportTabInit: FATAL verify failed for procedure no.%d when rehooking it!\n",i));
                    Int3(); /* ipe - later! */
                    return 1;
                }
                break;
            }


            /*
             * 32-bit imported procedure
             * This is called by issuing a near call to the callTab entry.
             */
            case EPT_PROCIMPORT32:
            {
                cb = interpretFunctionProlog32((char*)_aImportTab[i].ulAddress, FALSE);
                _aImportTab[i].cbProlog = (char)cb;
                if (cb > 0) /* Since no prolog part is copied to the function table, it's ok as long as the prolog has been recognzied. */
                {
                    /*
                     * Make jump instruction which jumps from calltab to original function.
                     * 0xE9 <four bytes displacement>
                     * Note: the displacement is relative to the next instruction
                     */
                    callTab[i][0] = 0xE9; /* jmp */
                    *(unsigned*)(void*)&callTab[i][1] = _aImportTab[i].ulAddress - (unsigned)&callTab[i][5];
                }
                else
                {   /* !fatal! - this should never really happen... */
                    kprintf(("ImportTabInit: FATAL verify failed for procedure no.%d when importing it!\n",i));
                    Int3(); /* ipe - later! */
                    return 1;
                }
                break;
            }


            /*
             * 16-bit imported procedure.
             * This is called by issuing a far call to the calltab entry.
             */
            case EPT_PROCIMPORT16:
            {
                cb = interpretFunctionProlog16((char*)_aImportTab[i].ulAddress, FALSE);
                _aImportTab[i].cbProlog = (char)cb;
                if (cb > 0) /* Since no prolog part is copied to the function table, it's ok as long as the prolog has been recognzied. */
                {
                    /*
                     * Create far jump from calltab to original function.
                     * 0xEA <four byte target address> <two byte target selector>
                     */
                    callTab[i][0] = 0xEA; /* jmp far ptr */
                    *(unsigned long*)(void*)&callTab[i][1] = _aImportTab[i].offObject;
                    *(unsigned short*)(void*)&callTab[i][5] = _aImportTab[i].usSel;
                }
                else
                {   /* !fatal! - this should never really happen... */
                    kprintf(("ImportTabInit: FATAL verify failed for procedure no.%d when importing it!\n",i));
                    Int3(); /* ipe - later! */
                    return 1;
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
                _aImportTab[i].cbProlog = (char)0;
                *(unsigned long*)(void*)&callTab[i][0] = _aImportTab[i].ulAddress;
                *(unsigned long*)(void*)&callTab[i][4] = _aImportTab[i].offObject;
                *(unsigned short*)(void*)&callTab[i][8] = _aImportTab[i].usSel;
                *(unsigned short*)(void*)&callTab[i][0xa] = (unsigned short)_aImportTab[i].offObject;
                *(unsigned short*)(void*)&callTab[i][0xc] = _aImportTab[i].usSel;
                break;

            default:
                kprintf(("ImportTabInit: unsupported type. (procedure no.%d, cb=%d)\n", i, cb));
                Int3(); /* ipe - later! */
                return 1;
        } /* switch - type */
    }   /* for */

    return NO_ERROR;
}
#endif /* !DEBUGR3 */




/*******************************************************************************
*   Ring-3 Debug Stuff
*******************************************************************************/
#ifdef DEBUGR3
#include <stdio.h>

void main(void)
{
    char ach_ldrRead[] = {
        0x55, 0x8b, 0xec, 0x8d, 0x65, 0xf8, 0x53, 0x56,
        0x57, 0x33, 0xd2, 0x42, 0x89, 0x55, 0xf8, 0x38,
        0x35, 0xce, 0x70, 0x00, 0x00, 0x75, 0x4d, 0x8b,
        0x55, 0x08, 0x66, 0x83, 0xfa, 0xff, 0x74, 0x62
    };
    char ach_ldrOpen[] = {
        0x55, 0x8b, 0xec, 0x33, 0xc0, 0x38, 0x05, 0xce,
        0x70, 0x00, 0x00, 0x75, 0x4b, 0x50, 0xff, 0x75,
        0x08, 0x6a, 0x01, 0x68, 0xa3, 0x00, 0x00, 0x00,
        0xff, 0x75, 0x0c, 0xff, 0x15, 0xc4, 0xc1, 0x0d
    };
    char ach_ldrClose[] = {
        0x55, 0x8b, 0xec, 0x33, 0xc0, 0x38, 0x05, 0xce,
        0x70, 0x00, 0x00, 0x75, 0x13, 0x8b, 0x55, 0x08,
        0x0f, 0xb7, 0xd2, 0x66, 0x83, 0xfa, 0xff, 0x74,
        0x07, 0x52, 0xff, 0x15, 0xcc, 0xc1, 0x0d, 0x00
    };
    char ach_LDRQAppType[] = {
        0x55, 0x8b, 0x0d, 0xa0, 0x0a, 0x00, 0x00, 0x8b,
        0xec, 0x83, 0xec, 0x3c, 0x53, 0x81, 0xc1, 0x24,
        0x06, 0x00, 0x00, 0x57, 0x56, 0x6a, 0xff, 0x66,
        0xc7, 0x45, 0xc6, 0x00, 0x00, 0x51, 0xe8, 0x38
    };
    char ach_ldrEnum32bitRelRecs[] = {
        0x55, 0xa1, 0xe8, 0xcf, 0x0d, 0x00, 0x8b, 0xec,
        0x83, 0xec, 0x5c, 0x53, 0x8b, 0x55, 0x08, 0x57,
        0x56, 0x8b, 0x52, 0x04, 0xf6, 0x40, 0x04, 0x80,
        0x89, 0x55, 0xfc, 0x0f, 0x84, 0x10, 0x01, 0x00
    };
    char ach_IOSftOpen[] = {
        0x55, 0x8b, 0xec, 0x8d, 0xa5, 0xa0, 0xfe, 0xff,
        0xff, 0xf6, 0x05, 0x95, 0x2d, 0x00, 0x00, 0x80,
        0x74, 0x13, 0x66, 0x68, 0x50, 0x30, 0xff, 0x75,
        0x08, 0x6a, 0x3c, 0x6a, 0x06, 0xe8, 0x5a, 0x03
    };
    char ach_IOSftClose[] = {
        0x55, 0x8b, 0xec, 0x8d, 0xa5, 0x28, 0xff, 0xff,
        0xff, 0xf6, 0x05, 0x95, 0x2d, 0x00, 0x00, 0x80,
        0x74, 0x16, 0x50, 0x66, 0x68, 0x51, 0x30, 0x8b,
        0x45, 0x08, 0x50, 0x6a, 0x0c, 0x6a, 0x06, 0xe8
    };
    char ach_IOSftTransPath[] = {
        0x55, 0x8b, 0xec, 0x8d, 0xa5, 0xd8, 0xfd, 0xff,
        0xff, 0x53, 0x56, 0x57, 0x1e, 0x06, 0xa1, 0xa4,
        0x0a, 0x00, 0x00, 0x66, 0x8d, 0x9d, 0xe2, 0xfe,
        0xff, 0xff, 0x66, 0x89, 0x98, 0xf6, 0x01, 0x00
    };
    char ach_IOSftReadAt[] = {
        0xb8, 0xc4, 0x68, 0x14, 0x00, 0xeb, 0x05, 0xb8,
        0xcc, 0x68, 0x14, 0x00, 0x55, 0x8b, 0xec, 0x8d,
        0xa5, 0x20, 0xff, 0xff, 0xff, 0xf6, 0x05, 0x95,
        0x2d, 0x00, 0x00, 0x80, 0x74, 0x28, 0x50, 0x66
    };
    char ach_IOSftWriteAt[] = {
        0xb8, 0xcc, 0x68, 0x14, 0x00, 0x55, 0x8b, 0xec,
        0x8d, 0xa5, 0x20, 0xff, 0xff, 0xff, 0xf6, 0x05,
        0x95, 0x2d, 0x00, 0x00, 0x80, 0x74, 0x28, 0x50,
        0x66, 0x68, 0x52, 0x30, 0x8b, 0x45, 0x08, 0x25
    };
    char ach_SftFileSize[] = {
        0x55, 0x8b, 0xec, 0x8d, 0xa5, 0x28, 0xff, 0xff,
        0xff, 0x57, 0x56, 0x53, 0xc6, 0x85, 0x2b, 0xff,
        0xff, 0xff, 0x00, 0x8b, 0x35, 0xa4, 0x0a, 0x00,
        0x00, 0xf6, 0x46, 0x04, 0x01, 0x74, 0x0a, 0x80
    };
    char ach_VMAllocMem[] = {
        0xa1, 0xe8, 0xcf, 0x0d, 0x00, 0x55, 0x8b, 0xec,
        0x83, 0xec, 0x44, 0x53, 0x57, 0xf6, 0x40, 0x04,
        0x80, 0x56, 0x0f, 0x84, 0x34, 0x01, 0x00, 0x00,
        0xf6, 0x00, 0x40, 0x74, 0x44, 0xa1, 0xcc, 0x02
    };
    char ach_VMGetOwner[] = {
        0x55, 0x8b, 0xec, 0x83, 0xec, 0x30, 0x57, 0x66,
        0x8b, 0x4d, 0x08, 0x66, 0x89, 0x4d, 0xd8, 0xf6,
        0xc1, 0x04, 0x75, 0x39, 0x80, 0x3d, 0x40, 0x8c,
        0x00, 0x00, 0x00, 0x74, 0x07, 0xa1, 0xac, 0x0a
    };
    char achg_tkExecPgm[] = {
        0x55, 0x8b, 0xec, 0x8d, 0x65, 0xa4, 0x66, 0x89,
        0x5d, 0xf4, 0x66, 0x8c, 0x45, 0xf6, 0x66, 0x89,
        0x55, 0xfc, 0x66, 0x8c, 0x5d, 0xfe, 0x66, 0x89,
        0x75, 0xf0, 0x66, 0x89, 0x7d, 0xf2, 0xc7, 0x45
    };
    char achf_FuStrLenZ[] = {
        0x6a, 0x02, 0x52, 0x66, 0x55, 0x0f, 0xa8, 0x68,
        0x0c, 0x00, 0x0f, 0xa9, 0x65, 0x67, 0x66, 0x8b,
        0x2d, 0xa8, 0x0a, 0x00, 0x00, 0x65, 0x67, 0xff,
        0xb5, 0xb4, 0x1f, 0x00, 0x00, 0x65, 0x67, 0x66
    };
    char achf_FuStrLen[] = {
        0x6a, 0x02, 0x52, 0x66, 0x55, 0x0f, 0xa8, 0x68,
        0x0c, 0x00, 0x0f, 0xa9, 0x65, 0x67, 0x66, 0x8b,
        0x2d, 0xa8, 0x0a, 0x00, 0x00, 0x65, 0x67, 0xff,
        0xb5, 0xb4, 0x1f, 0x00, 0x00, 0x65, 0x67, 0x66
    };
    char achf_FuBuff[] = {
        0x6a, 0x02, 0x52, 0x66, 0x55, 0x0f, 0xa8, 0x68,
        0x0c, 0x00, 0x0f, 0xa9, 0x65, 0x67, 0x66, 0x8b,
        0x2d, 0xa8, 0x0a, 0x00, 0x00, 0x65, 0x67, 0xff,
        0xb5, 0xb4, 0x1f, 0x00, 0x00, 0x65, 0x67, 0x66
    };
    char achf_VMObjHandleInfo[] =  {
        0x55, 0x8B, 0x0D, 0x9C, 0xBF, 0x0D, 0x00, 0x8B,
        0xEC, 0x83, 0xEC, 0x0C, 0x53, 0x57, 0x8D, 0x55,
        0xF8
    };


    char *aProcs[] =
    {
        ach_ldrRead            ,
        ach_ldrOpen            ,
        ach_ldrClose           ,
        ach_LDRQAppType        ,
        ach_ldrEnum32bitRelRecs,
        ach_IOSftOpen          ,
        ach_IOSftClose         ,
        ach_IOSftTransPath     ,
        ach_IOSftReadAt        ,
        ach_IOSftWriteAt       ,
        ach_SftFileSize        ,
        ach_VMAllocMem         ,
        ach_VMGetOwner         ,
        achg_tkExecPgm         ,
        achf_FuStrLenZ         ,
        achf_FuStrLen          ,
        achf_FuBuff            ,
        achf_VMObjHandleInfo   ,
        NULL
    };
    int i;

    /* loop thru procs */
    for (i = 0; aProcs[i] != NULL; i++)
    {
        unsigned cb;
        printf("Proc.no.%i\n", i);
        if (i < 14 || i > 16)
            cb = interpretFunctionProlog32(aProcs[i], i < 5 || i == 13);
        else
            cb = interpretFunctionProlog16(aProcs[i], FALSE);
        printf(" cb=%d\n\n", cb);
    }
}
#endif
