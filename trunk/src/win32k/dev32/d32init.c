/* $Id: d32init.c,v 1.7 2000-02-08 12:38:38 bird Exp $
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
#define MAXSIZE_PROLOG 0x10             /* Note that this must be synced with */
                                        /* the one used in calltab.asm.       */
#define static

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
#include "probkrnl.h"
#include "log.h"
#include "asmutils.h"
#include "malloc.h"
#include "ldr.h"
#include "ldrCalls.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static ULONG    readnum(const char *pszNum);
static int      interpretFunctionProlog(char *p, BOOL fOverload);
static int      procInit(void);


/* externs located in 16-bit data segement */
extern ULONG    _TKSSBase16;
extern USHORT   _R0FlatCS16;
extern USHORT   _R0FlatDS16;


/* extern(s) located in calltab.asm */
extern char     callTab[NUMBER_OF_PROCS][MAXSIZE_PROLOG];


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
    char   *pszTmp2;
    char   *pszTmp;
    ULONG   ul;

    pulTKSSBase32 = (PULONG)_TKSSBase16;

    SET_OPTIONS_TO_DEFAULT(options);

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
    options.ulBuild    = _ulBuild;
    options.usVerMajor = _usVerMajor;
    options.usVerMinor = _usVerMinor;

    /* log option summary - FIXME */
    kprintf(("Options - Summary\n"));
    kprintf(("\tKernel: ver%d.%d  build %d  type %s\n",
                options.usVerMajor,
                options.usVerMinor,
                options.ulBuild,
                (options.fKernel & KF_SMP) ? "SMP" : "UNI"
              ));
    kprintf(("\tCom port no.%d\n", options.usCom));
    if (options.fQuiet)
        kprintf(("\tQuiet init\n"));
    else
        kprintf(("\tVerbose init\n"));
    if (options.fLogging)
        kprintf(("\tlogging enabled\n"));
    else
        kprintf(("\tlogging disabled\n"));
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
        if (procInit() != NO_ERROR)
            return STATUS_DONE | STERR | ERROR_I24_QUIET_INIT_FAIL;

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
 * Verifies the aProcTab.
 * @returns   0 if ok. !0 if not ok.
 * @remark    Called from IOCtl.
 */
USHORT _loadds _Far32 _Pascal VerifyProcTab32(void)
{
    int i;
    int cb;

    /* verify */
    for (i = 0; i < NUMBER_OF_PROCS; i++)
    {
        /* verify that it is found */
        if (!_aProcTab[i].fFound)
        {
            kprintf(("VerifyProcTab32: procedure no.%d was not found!\n", i));
            return STATUS_DONE | STERR | 1;
        }

        /* verify read/writeable. - FIXME */
        if (_aProcTab[i].ulAddress < 0xffe00000UL)
        {
            kprintf(("VerifyProcTab32: procedure no.%d has an invlalid address, %#08x!\n",
                     i, _aProcTab[i].ulAddress));
            return STATUS_DONE | STERR | 2;
        }

        switch (_aProcTab[i].fType)
        {
            case EPT_PROC:
            case EPT_PROCIMPORT:
                /* verify known function prolog. */
                if ((cb = interpretFunctionProlog((char*)_aProcTab[i].ulAddress, _aProcTab[i].fType == EPT_PROC))
                    <= 0 && cb + 5 >= MAXSIZE_PROLOG)
                {
                    kprintf(("VerifyProcTab32: verify failed for procedure no.%d\n",i));
                    return STATUS_DONE | STERR | 3;
                }
                break;

            default:
                kprintf(("VerifyProcTab32: only EPT_PROC is implemented\n",i));
                return STATUS_DONE | STERR | 4;
        }
    }

    return STATUS_DONE;
}


/**
 * Get kernel OTEs
 * @returns   Strategy return code:
 *            STATUS_DONE on success.
 *            STATUS_DONE | STERR | errorcode on failure.
 * @param     pOTEBuf  Pointer to output buffer.
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 * @remark    Called from IOCtl.
 */
USHORT _loadds _Far32 _Pascal GetOTEs32(PKRNLOBJTABLE pOTEBuf)
{
    PMTE    pMTE;
    PSMTE   pSMTE;
    POTE    pOTE;
    int     i;
    USHORT  usRc;

    pMTE = GetOS2KrnlMTE();
    if (pMTE != NULL)
    {
        pSMTE = pMTE->mte_swapmte;
        if (pSMTE != NULL)
        {
            pOTEBuf->cObjects = (unsigned char)pSMTE->smte_objcnt;
            if (pSMTE->smte_objcnt <= MAXKRNLOBJECTS)
            {
                pOTE = pSMTE->smte_objtab;
                if (pOTE != NULL)
                {
                    for (i = 0; i < pOTEBuf->cObjects; i++)
                        memcpy((void*)&pOTEBuf->aObjects[i], &pOTE[i], sizeof(OTE));
                    usRc = 0;
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


/**
 * Interpret function prolog to find where to jmp back.
 * @returns   Length of prolog need to be copied - which is also the offset of
 *            where the jmp instr should be placed.
 *            On error it returns 0.
 * @param     pach       Pointer to prolog.
 * @param     fOverload  TRUE:  Function is to be overloaded.
 *                       FALSE: Function is to be imported.
 */
static int interpretFunctionProlog(char *pach, BOOL fOverload)
{
    int rc;

    /*
     * check for the well known prolog (the only that is supported now)
     * which is:
     *     push ebp
     *     mov ebp,esp
     *  or
     *     push ebp
     *     mov ecx, dword ptr [xxxxxxxx]
     */

    if (pach[0] == 0x55 && pach[1] == 0x8b)
    {
        if (pach[2] == 0xec)
            rc = 3;
        else
            rc = 1;
        while (rc < 5)
        {
            /*
             * This is not at all very stable or correct - but it works
             * for the current functions.
             * There will never be any doubt when something goes wrong!
             */
            switch(pach[rc])
            {
                case 0x33: /* xor (ldrClose, ldrOpen) */
                    rc +=2;
                    break;
                case 0x8b:
                    if (pach[rc+1] == 0x0d)
                        rc += 6;
                    else
                        rc += 2; /*????!*/
                    break;
                case 0x8d: /* lea (ldrRead) */
                    rc += 3;
                    break;
                case 0x83: /* sub (LDRQAppType) */
                    rc += 3;
                    break;
                default:
                    kprintf(("interpretFunctionProlog: unknown instruction 0x%x\n", pach[rc]));
                    return 0;
            }
        }
    }
    else
    {
        /* special case for IOSftReadAt and IOSftWriteAt */
        if (fOverload == FALSE && pach[0] == 0xB8 && (pach[5] == 0xEB || pach[5] == 0x55))
            rc = 5;
        else
            rc = 0;
    }

    return rc;
}



/**
 * Initiates the overrided functions.
 * @returns   NO_ERROR on success. !0 on error.
 */
static int procInit(void)
{
    int i;
    int cb;

    /*
     * verify proctable
     */
    for (i = 0; i < NUMBER_OF_PROCS; i++)
    {
        if (_aProcTab[i].fType != EPT_PROC && _aProcTab[i].fType != EPT_PROCIMPORT)
        {
            kprintf(("procInit: EPT_VAR is not supported. (procedure no.%d, cb=%d)\n", i, cb));
            return 1;
        }
        cb = interpretFunctionProlog((char*)_aProcTab[i].ulAddress, _aProcTab[i].fType == EPT_PROC);
        if (cb <= 0 || cb + 5 >= MAXSIZE_PROLOG)
        {
            kprintf(("procInit: verify failed for procedure no.%d, cb=%d\n", i, cb));
            return 1;
        }
    }

    /*
     * rehook / import
     */
    for (i = 0; i < NUMBER_OF_PROCS; i++)
    {
        switch (_aProcTab[i].fType)
        {
            case EPT_PROC:
            {
                cb = interpretFunctionProlog((char*)_aProcTab[i].ulAddress, TRUE);
                if (cb > 0 && cb + 5 < MAXSIZE_PROLOG)
                {
                    static unsigned auFuncs[NUMBER_OF_PROCS] = /* This table must be updated with the overloading functions. */
                    {
                        (unsigned)myldrRead,
                        (unsigned)myldrOpen,
                        (unsigned)myldrClose,
                        (unsigned)myLDRQAppType,
                        (unsigned)myldrEnum32bitRelRecs
                        #if 0 /* Currently problems with with WS4eB */
                        ,
                        0,
                        0,
                        0,
                        0,
                        0
                        #endif
                    };

                    /* copy function prolog */
                    memcpy(callTab[i], (void*)_aProcTab[i].ulAddress, (size_t)cb);

                    /* jump from calltab to original function */
                    callTab[i][cb] = 0xE9; /* jmp */
                    *(unsigned*)(void*)&callTab[i][cb+1] = _aProcTab[i].ulAddress + cb - (unsigned)&callTab[i][cb+5];


                    /* jump from original function to my function - an cli(?) could be needed here */
                    *(char*)_aProcTab[i].ulAddress = 0xE9; /* jmp */
                    *(unsigned*)(_aProcTab[i].ulAddress + 1) = auFuncs[i] - (_aProcTab[i].ulAddress + 5);
                }
                else
                {   /* !fatal! - this could never happen really... */
                    kprintf(("procInit: FATAL verify failed for procedure no.%d when rehooking it!\n",i));
                    Int3(); /* ipe - later! */
                    return 1;
                }
                break;
            }

            case EPT_PROCIMPORT:
            {
                cb = interpretFunctionProlog((char*)_aProcTab[i].ulAddress, FALSE);
                if (cb > 0 && cb + 5 < MAXSIZE_PROLOG)
                {
                    /* jump from calltab to original function */
                    callTab[i][0] = 0xE9; /* jmp */
                    *(unsigned*)(void*)&callTab[i][1] = _aProcTab[i].ulAddress - (unsigned)&callTab[i][cb+5];
                }
                else
                {   /* !fatal! - this could never happen really... */
                    kprintf(("procInit: FATAL verify failed for procedure no.%d when importing it!\n",i));
                    Int3(); /* ipe - later! */
                    return 1;
                }
                break;
            }

            default:
                kprintf(("procInit: EPT_VAR is not supported. (procedure no.%d, cb=%d)\n", i, cb));
                Int3(); /* ipe - later! */
                return 1;
        } /* switch - type */
    }   /* for */

    return NO_ERROR;
}

