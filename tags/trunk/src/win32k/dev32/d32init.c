/* $Id: d32init.c,v 1.12 2000-02-20 04:27:23 bird Exp $
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
#define static                          /* just to make all symbols visible in the kernel debugger.  */


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
static signed char interpretFunctionProlog32(char *pach, BOOL fOverload);
static signed char interpretFunctionProlog16(char *pach, BOOL fOverload);
static int      ImportTabInit(void);


/* externs located in 16-bit data segement in ProbKrnl.c */
extern ULONG    _TKSSBase16;
extern USHORT   _R0FlatCS16;
extern USHORT   _R0FlatDS16;

/* extern(s) located in calltab.asm */
extern char     callTab[NBR_OF_KRNLIMPORTS][MAXSIZE_PROLOG];

/* extern(s) located in mytkExecPgm.asm  */
extern char     mytkExecPgm;



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
        if (ImportTabInit() != NO_ERROR)
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
 * 32-bit! Interpret function prolog to find where to jmp back.
 * @returns   Length of prolog need to be copied - which is also the offset of
 *            where the jmp instr should be placed.
 *            On error it returns 0.
 * @param     pach       Pointer to prolog.
 * @param     fOverload  TRUE:  Function is to be overloaded.
 *                       FALSE: Function is to be imported.
 */
static signed char interpretFunctionProlog32(char *pach, BOOL fOverload)
{
    int cb;

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
            cb = 3;
        else
            cb = 1;
        while (cb < 5)
        {
            /*
             * This is not at all very stable or correct - but it works
             * for the current functions.
             * There will never be any doubt when something goes wrong!
             */
            switch(pach[cb])
            {
                case 0x33: /* xor (ldrClose, ldrOpen) */
                    cb +=2;
                    break;
                case 0x8b:
                    if (pach[cb+1] == 0x0d)
                        cb += 6;
                    else
                        cb += 2; /*????!*/
                    break;
                case 0x8d: /* lea (ldrRead) */
                    cb += 3;
                    break;
                case 0x83: /* sub (LDRQAppType) */
                    cb += 3;
                    break;
                default:
                    kprintf(("interpretFunctionProlog: unknown instruction 0x%x\n", pach[cb]));
                    return 0;
            }
        }
    }
    else if (pach[0] == 0x55 && pach[1] == 0xa1) /* ldrEnum32bitRelRecs on WS4eB */
    {
        cb = 1 + 5;
    }
    else
    {
        /* special case for IOSftReadAt and IOSftWriteAt */
        if (fOverload == FALSE && pach[0] == 0xB8 && (pach[5] == 0xEB || pach[5] == 0x55))
            cb = 5;
        else
            cb = 0;
    }

    return (signed char)cb;
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
static signed char interpretFunctionProlog16(char *pach, BOOL fOverload)
{
    int cb;

    /*
     * Check for the well known prolog (the only that is supported now)
     * which is:
     *     push 2
     */
    if (*pach == 0x6A)                  /* push 2 (don't check for the 2) */
    {
        BOOL fForce;
        cb = 0;
        while (cb < 8 || fForce)
        {
            fForce = FALSE;
            switch (*pach)
            {
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
                    break;

                case 0x6a:              /* push <byte> */
                    pach++;
                    cb++;
                    break;

                case 0x68:              /* push <word> */
                    pach += 2;
                    cb += 2;
                    break;

                case 0x8b:              /* mov /r */
                    if ((pach[1] & 0xc0) == 10  /* ex. mov ax,bp+1114h */
                        || ((pach[1] & 0xc0) == 0 && (pach[1] & 0xc0) == 6)) /* ex. mov bp,0ff23h */
                    {   /* 16-bit displacement */
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
                    kprintf(("interpretFunctionProlog: unknown instruction 0x%x 0x%x 0x%x\n", pach[0], pach[1], pach[2]));
                    return 0;
            }
            pach++;
            cb++;
        }
    }

    fOverload = fOverload;
    return (signed char)cb;
}


/**
 * Verifies the aImportTab.
 * @returns   0 if ok. !0 if not ok.
 * @remark    Called from IOCtl.
 *            WARNING! VerifyImporTab32 is called before the initroutine!
 */
USHORT _loadds _Far32 _Pascal VerifyImportTab32(void)
{
    int i;
    int cb;
    int cbmin;

    /* VerifyImporTab32 is called before the initroutine! */
    pulTKSSBase32 = (PULONG)_TKSSBase16;

    /* verify */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        /* verify that it is found */
        if (!_aImportTab[i].fFound)
        {
            kprintf(("VerifyImportTab32: procedure no.%d was not found!\n", i));
            return STATUS_DONE | STERR | 1;
        }

        /* verify read/writeable. - FIXME */
        if (_aImportTab[i].ulAddress < 0xffe00000UL)
        {
            kprintf(("VerifyImportTab32: procedure no.%d has an invlalid address, %#08x!\n",
                     i, _aImportTab[i].ulAddress));
            return STATUS_DONE | STERR | 2;
        }

        switch (_aImportTab[i].fType & ~EPT_BIT_MASK)
        {
            case EPT_PROC:
            case EPT_PROCIMPORT:
                /*
                 * Verify known function prolog.
                 */
                if (_aImportTab[i].fType & EPT_32BIT)
                {
                    cb = interpretFunctionProlog32((char*)_aImportTab[i].ulAddress, _aImportTab[i].fType == EPT_PROC32);
                    cbmin = 5;
                }
                else
                {
                    cb = interpretFunctionProlog16((char*)_aImportTab[i].ulAddress, _aImportTab[i].fType == EPT_PROC16);
                    cbmin = 8;
                }

                /*
                 * Check result of the function prolog interpretations.
                 */
                if (cb <= 0 && cb + cbmin >= MAXSIZE_PROLOG)
                {   /* failed, too small or too large. */
                    kprintf(("VerifyImportTab32: verify failed for procedure no.%d (cd=%d)\n", i, cb));
                    return STATUS_DONE | STERR | 3;
                }
                break;

            case EPT_VARIMPORT:
                /* do nothing! */
                break;

            default:
                kprintf(("VerifyImportTab32: only EPT_PROC is implemented\n",i));
                return STATUS_DONE | STERR | 4;
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
        0,//(unsigned)myLDRQAppType,
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
        0
    };

    int i;
    int cb;


    /*
     * verify proctable
     */
    for (i = 0; i < NBR_OF_KRNLIMPORTS; i++)
    {
        /* EPT_VARIMPORTs are skipped */
        if ((_aImportTab[i].fType & ~EPT_BIT_MASK) == EPT_VARIMPORT)
            continue;

        if (_aImportTab[i].fType & EPT_32BIT)
            cb = interpretFunctionProlog32((char*)_aImportTab[i].ulAddress, _aImportTab[i].fType == EPT_PROC32);
        else
            cb = interpretFunctionProlog16((char*)_aImportTab[i].ulAddress, _aImportTab[i].fType == EPT_PROC16);
        if (cb <= 0 || cb + 5 >= MAXSIZE_PROLOG)
        {
            kprintf(("ImportTabInit: verify failed for procedure no.%d, cb=%d\n", i, cb));
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
                cb = _aImportTab[i].cbProlog = interpretFunctionProlog32((char*)_aImportTab[i].ulAddress, TRUE);
                if (cb > 0 && cb + 5 < MAXSIZE_PROLOG)
                {
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

                cb = _aImportTab[i].cbProlog = interpretFunctionProlog16((char*)_aImportTab[i].ulAddress, TRUE);
                if (cb > 0 && cb + 8 < MAXSIZE_PROLOG) /* a 16:32 jump must be prefixed with 66h in a 16-bit segment */
                {
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
                cb = _aImportTab[i].cbProlog = interpretFunctionProlog32((char*)_aImportTab[i].ulAddress, FALSE);
                if (cb > 0 && cb + 5 < MAXSIZE_PROLOG)
                {
                    /*
                     * Make jump instruction which jumps from calltab to original function.
                     * 0xE9 <four bytes displacement>
                     * Note: the displacement is relative to the next instruction
                     */
                    callTab[i][0] = 0xE9; /* jmp */
                    *(unsigned*)(void*)&callTab[i][1] = _aImportTab[i].ulAddress - (unsigned)&callTab[i][cb+5];
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
                cb = _aImportTab[i].cbProlog = interpretFunctionProlog16((char*)_aImportTab[i].ulAddress, FALSE);
                if (cb > 0 && cb + 8 < MAXSIZE_PROLOG)
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
             * callTab-entry + c holds the selector for the object. (These two fiels is the 16:16-bit pointer to the variable.)
             */
            case EPT_VARIMPORT32:
            case EPT_VARIMPORT16:
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

