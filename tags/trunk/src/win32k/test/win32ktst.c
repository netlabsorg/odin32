/* $Id: win32ktst.c,v 1.1 2000-07-16 22:18:16 bird Exp $
 *
 * Win32k test module.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_BASE
#define INCL_OS2KRNL_ALL
#define INCL_SSTODS

#define SEL_FLATMASK        0x01fff0000
#define SEL_FLAT_SHIFT      0x0d
#define SEL_LDT_RPL3        0x07

#define SelToFlat(sel, off) \
    (PVOID)( (((unsigned)(sel) << SEL_FLAT_SHIFT) & SEL_FLAT_MASK) + (unsigned)(off))

#define FlatToSel(flataddr) \
    (PVOID)( ( (((unsigned)(flataddr) << 3) & 0xfff80000) | (SEL_LDT_RPL3 << 16) ) | ((unsigned)(flataddr) & 0xffff) )

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>

#include "malloc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "options.h"
#include "dev1632.h"
#include "dev32.h"
#include "devcmd.h"
#include "os2krnl.h"
#include "ldrCalls.h"
#include "test.h"
#include "asmutils.h"
#include "macros.h"
#include "log.h"


/** @design Win32k Ring-3 Testing
 * I'll try to make it possible to test parts or all the win32k code in ring-3.
 * To archive this the interface against the kernel has to be faked/emulated.
 * More precisely:
 *      - DevHelps.
 *      - Worker routines for imported kernel functions. (calltab.asm jumps to them.)
 *      - 16-bit stack 1Kb.
 *      - Strategy calls.
 *      - Fake module loadings (= testcases).
 *      - ?
 *
 * Some of the initstuff has to be omitted, at least in the start. The first
 * goal is to be able to test _ldrOpenPath and _ldrOpen.
 *
 *
 * @subsection  Device Helper Routines
 *
 * These I think we'll implemented by providing the kernel interface, a far 16:16
 * pointer to a dh-router. Our router will in most cases thunk back to 32-bit
 * code and implementet the required devhelp routines in pure C code.
 *
 * These are the needed routines:
 *      - DevHelp_VirtToLin   - ok
 *      - DevHelp_VMAlloc     - ok
 *      - DevHelp_VMFree      - ok
 *      - DevHelp_GetDOSVar   - ok
 *      - DevHelp_VMLock
 *      - DevHelp_VMUnLock  ?
 *      - DevHelp_VMSetMem  ?
 *      - DevHelp_Yield     ?
 *
 *
 * @subsection  Worker routines for imported kernel functions
 *
 * Create worker routines for the imported kernel functions. Calltab will be
 * set up to jump to them. This is done in d32init.c, in stead of using
 * the importtab.
 *
 * Some of these workers will be parts of testcases. For example g_tkExecPgm
 * and _LDRQAppType.
 *
 * Only the imported functions are implemented on demand. Initially these
 * functions will be provided:
 *      - ldrOpen
 *      - ldrRead
 *      - ldrClose
 *      - ldrOpenPath
 *      - SftFileSize
 *
 *
 * @subsection  16-bit stack
 *
 * To make this test real the stack has to be 16-bit and _very_ small (1KB).
 * TKSSBase have to be implemented by the DevHelp_GetDOSVar DosTable2 stuff.
 * The stack will be thunked to 16-bit by a thunking procedure called from
 * main. This procedure thunks the stack (quite easy, we're in tiled memory!),
 * set the value of TKSSBase, calls a C function which does all the rest of
 * the testing. When taht function returns, the stack will be thunked back
 * to 32-bit, TKSSBase will be zeroed, and the procedure returns to main.
 *
 *
 * @subsection  Straegy routine calls (not implemented)
 *
 * We'll call the strategy entry points with init request packets. The initiation
 * will require a replacement for DosDevIOCtl (16-bit) which calls the
 * $elf strategy routine. We'll also have to provide fakes for kernel probing,
 * verifing and overloading in d32init.c.
 *
 *
 * @subsection  Order of events
 *
 * This is the order this testing environment currently works:
 *      1) init devhelp subsystem
 *      2) init workers
 *      3) thunk stack
 *      4) Fake 16-bit init. Set TKSSBase, FlatDS, FlatCS, DevHelp pointer....
 *      5) Call R0Init32().
 *         (d32init.c is modified a bit to setup the calltab correctly.)
 *      6) Start testing...
 *      7) Testing finished - thunk stack back to 32-bit.
 */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void    syntax(void);
void    workersinit(void);
void    initRPInit(RP32INIT *pRpInit, char *pszInitArgs);
int     tests(int iTest, int argc, char **argv);
int     TestCase1(void);
int     TestCase2(void);
int     TestCase3(void);
int     TestCase4(void);
int     TestCase5(void);
int     CompareOptions(struct options *pOpt);
int     TestCaseExeLoad1(void);


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern BOOL     fInited;                /* malloc.c */
const char *    pszInternalRevision = "\r\nInternal revision 14.040_W4";
int             cObjectsFake = 14;



/**
 * Main function. Arguments _currently_ unused.
 */
int main(int argc, char **argv)
{
    int         iTest;
    int         rc;

    /*
     * Init devhelp subsystems.
     */
    dhinit();

    /*
     * Parse arguments.
     * (printf don't work before dhinit is called...)
     */
    if (argc < 2 || (iTest = atoi(argv[1])) <= 0)
    {
        syntax();
        printf("syntax error\n");
        return -1;
    }

    /*
     * Init workers if necessary.
     */
    workersinit();


    /*
     * Thunk Stack to 16-bits.
     *
     * IMPORTANT! From now on SSToDS have to be used when making pointers
     *            to stack objects.
     */
    ThunkStack32To16();

    rc = tests(iTest, argc, argv);

    /*
     * Thunk Stack back to 32-bits.
     */
    ThunkStack16To32();


    /*
     * To avoid a sfree error message we'll set fInited (heap init flag) to false.
     */
    fInited = FALSE;
    return rc;
}


/**
 * Prints syntax information.
 * @status  partially implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
void    syntax(void)
{
    printf(
        "Win32kTst.exe v%d.%d.%d - Ring 3 testing of win32k.sys\n"
        "syntax: Win32kTst.exe <testcase number> [optional arguments]\n",
        0,0,4
           );
}


/**
 * Initiate workers (imported kernel functions / vars)
 * @status    partially implemented.
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
void  workersinit(void)
{
    DosSetMem(&CODE16START, &CODE16END - &CODE16START, PAG_WRITE | PAG_READ);
    DosSetMem(&CODE32START, &CODE32END - &CODE32START, PAG_WRITE | PAG_READ);
}


/**
 * Initiates a init reqest packet.
 * @param   pRpInit         Pointer to request packet to init.
 * @param   pszInitArgs     Pointer to init arguments.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
void initRPInit(RP32INIT *pRpInit, char *pszInitArgs)
{
    /* $elf */
    memset(pRpInit, 0, sizeof(RP32INIT));
    pRpInit->rph.Len  = sizeof(RP32INIT);
    pRpInit->rph.Cmd  = CMDInit;
    pRpInit->DevHlpEP = getDHRouterFarPtr();
    pRpInit->InitArgs = (PSZ)FlatToSel(pszInitArgs);
}


/**
 * Testcase fan-out.
 * @returns Testcase return value.
 *          -2 if testcase not found.
 * @param   iTest   Testcase number.
 * @param   argc    main argc
 * @param   argv    main argv
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int tests(int iTest, int argc, char **argv)
{
    int     rc;

    printf("-------------------------------- Testcase %d:\n", iTest);
    switch (iTest)
    {
        case 1:     rc = TestCase1();   break;
        case 2:     rc = TestCase2();   break;
        case 3:     rc = TestCase3();   break;
        case 4:     rc = TestCase4();   break;
        case 5:     rc = TestCase5();   break;

        default:
            printf("testcase no. %d is not found\n", iTest);
            rc = -2;
    }
    printf("result %d -----------------------------------\n", rc);

    NOREF(argc);
    NOREF(argv);
    return rc;
}



/**
 * Test case 1.
 * Checks that default initiation works fine for Aurora SMP kernels.
 *
 * @sketch  Create init packet with no arguments.
 *          Initiate elf$
 *          Create init packet with no arguments.
 *          Initiate win32k$
 * @returns 0 on success.
 *          1 on failure.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int TestCase1(void)
{
    int         rc = 1;
    RP32INIT    rpinit;

    /* init fake variabels */
    pszInternalRevision ="\r\nInternal revision 14.040_SMP";
    cObjectsFake = 15;
    _usFakeVerMajor = 20;
    _usFakeVerMinor = 45;

    /* $elf */
    initRPInit(SSToDS(&rpinit), "-w3");
    rc = InitElf(&rpinit);              /* no SSToDS! */
    printf("InitElf returned status=0x%04x\n", rpinit.rph.Status);
    if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
    {
        /* $win32k */
        initRPInit(SSToDS(&rpinit), "-w3");
        rc = InitWin32k(&rpinit);       /* no SSToDS! */
        printf("InitWin32k returned status=0x%04x\n", rpinit.rph.Status);
        if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
        {
            struct options opt = DEFAULT_OPTION_ASSIGMENTS;
            opt.fKernel = KF_SMP;
            opt.ulBuild = 14040;
            opt.usVerMajor = 20;
            opt.usVerMinor = 45;
            opt.ulInfoLevel = 3;

            rc = CompareOptions(SSToDS(&opt));
            if (rc == NO_ERROR)
            {
                rc = TestCaseExeLoad1();
            }
        }
        else
            printf("!failed!\n");
    }
    else
        printf("!failed!\n");

    return rc;
}


/**
 * Test case 2.
 * Checks that default initiation works fine for Aurora UNI kernels.
 *
 * @sketch  Create init packet with no arguments.
 *          Initiate elf$
 *          Create init packet with no arguments.
 *          Initiate win32k$
 * @returns 0 on success.
 *          1 on failure.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int TestCase2(void)
{
    int         rc = 1;
    RP32INIT    rpinit;

    /* init fake variabels */
    pszInternalRevision ="\r\nInternal revision 14.040_UNI";
    cObjectsFake = 14;
    _usFakeVerMajor = 20;
    _usFakeVerMinor = 45;

    /* $elf */
    initRPInit(SSToDS(&rpinit), "");
    rc = InitElf(&rpinit);              /* no SSToDS! */
    printf("InitElf returned status=0x%04x\n", rpinit.rph.Status);
    if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
    {
        /* $win32k */
        initRPInit(SSToDS(&rpinit), "");
        rc = InitWin32k(&rpinit);       /* no SSToDS! */
        printf("InitWin32k returned status=0x%04x\n", rpinit.rph.Status);
        if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
        {
            struct options opt = DEFAULT_OPTION_ASSIGMENTS;
            opt.fKernel = KF_UNI;
            opt.ulBuild = 14040;
            opt.usVerMajor = 20;
            opt.usVerMinor = 45;

            rc = CompareOptions(SSToDS(&opt));
        }
        else
            printf("!failed!\n");
    }
    else
        printf("!failed!\n");

    return rc;
}


/**
 * Test case 3.
 * Checks that default initiation works fine for Warp FP13 kernel.
 *
 * @sketch  Create init packet with no arguments.
 *          Initiate elf$
 *          Create init packet with no arguments.
 *          Initiate win32k$
 * @returns 0 on success.
 *          1 on failure.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int TestCase3(void)
{
    int         rc = 1;
    RP32INIT    rpinit;

    /* init fake variabels */
    pszInternalRevision ="\r\nInternal revision 14.040_W4";
    cObjectsFake = 14;
    _usFakeVerMajor = 20;
    _usFakeVerMinor = 45;

    /* $elf */
    initRPInit(SSToDS(&rpinit), "");
    rc = InitElf(&rpinit);              /* no SSToDS! */
    printf("InitElf returned status=0x%04x\n", rpinit.rph.Status);
    if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
    {
        /* $win32k */
        initRPInit(SSToDS(&rpinit), "");
        rc = InitWin32k(&rpinit);       /* no SSToDS! */
        printf("InitWin32k returned status=0x%04x\n", rpinit.rph.Status);
        if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
        {
            struct options opt = DEFAULT_OPTION_ASSIGMENTS;
            opt.fKernel = KF_UNI | KF_W4;
            opt.ulBuild = 14040;
            opt.usVerMajor = 20;
            opt.usVerMinor = 45;

            rc = CompareOptions(SSToDS(&opt));
        }
        else
            printf("!failed!\n");
    }
    else
        printf("!failed!\n");

    return rc;
}


/**
 * Test case 4.
 * Checks that default initiation works fine for Aurora SMP kernels.
 *
 * @sketch  Create init packet with no arguments.
 *          Initiate elf$
 *          Create init packet with no arguments.
 *          Initiate win32k$
 * @returns 0 on success.
 *          1 on failure.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int TestCase4(void)
{
    int         rc = 1;
    RP32INIT    rpinit;

    /* init fake variabels */
    pszInternalRevision ="\r\nInternal revision 9.036";
    cObjectsFake = 14;
    _usFakeVerMajor = 20;
    _usFakeVerMinor = 40;

    /* $elf */
    initRPInit(SSToDS(&rpinit), "");
    rc = InitElf(&rpinit);              /* no SSToDS! */
    printf("InitElf returned status=0x%04x\n", rpinit.rph.Status);
    if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
    {
        /* $win32k */
        initRPInit(SSToDS(&rpinit), "");
        rc = InitWin32k(&rpinit);       /* no SSToDS! */
        printf("InitWin32k returned status=0x%04x\n", rpinit.rph.Status);
        if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
        {
            struct options opt = DEFAULT_OPTION_ASSIGMENTS;
            opt.fKernel     = KF_UNI;
            opt.ulBuild     = 9036;
            opt.usVerMajor  = 20;
            opt.usVerMinor  = 40;

            rc = CompareOptions(SSToDS(&opt));
        }
        else
            printf("!failed!\n");
    }
    else
        printf("!failed!\n");

    return rc;
}


/**
 * Test case 5.
 * Checks that all parameters are read correctly (1) (Warp FP13 kernel).
 *
 * @sketch  Create init packet with no arguments.
 *          Initiate elf$
 *          Create init packet with no arguments.
 *          Initiate win32k$
 * @returns 0 on success.
 *          1 on failure.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int TestCase5(void)
{
    int         rc = 1;
    RP32INIT    rpinit;
    char *      pszInitArgs = "-C1 -L:N -Verbose -Quiet -Elf:Yes -Pe:Mixed -Script:No -W4 -Heap:512000 -ResHeap:0256000 -HeapMax:4096000 -ResHeapMax:0x100000";

    /* init fake variabels */
    pszInternalRevision ="\r\nInternal revision 14.040_W4";
    cObjectsFake = 14;
    _usFakeVerMajor = 20;
    _usFakeVerMinor = 45;

    /* $elf */
    initRPInit(SSToDS(&rpinit), pszInitArgs);
    rc = InitElf(&rpinit);              /* no SSToDS! */
    printf("InitElf returned status=0x%04x\n", rpinit.rph.Status);
    if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
    {
        /* $win32k */
        initRPInit(SSToDS(&rpinit), pszInitArgs);
        rc = InitWin32k(&rpinit);       /* no SSToDS! */
        printf("InitWin32k returned status=0x%04x\n", rpinit.rph.Status);
        if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
        {
            struct options opt = DEFAULT_OPTION_ASSIGMENTS;
            opt.fKernel         = KF_UNI | KF_W4;
            opt.ulBuild         = 14040;
            opt.usVerMajor      = 20;
            opt.usVerMinor      = 45;
            opt.cbSwpHeapInit   = 512000;
            opt.cbSwpHeapMax    = 4096000;
            opt.cbResHeapInit   = 0256000;
            opt.cbResHeapMax    = 0x100000;
            opt.fElf            = TRUE;
            opt.fUNIXScript     = FALSE;
            opt.fPE             = FLAGS_PE_MIXED;
            opt.fQuiet          = TRUE;
            opt.fLogging        = FALSE;
            opt.usCom           = OUTPUT_COM1;
            opt.ulInfoLevel     = INFOLEVEL_INFOALL;

            rc = CompareOptions(SSToDS(&opt));
        }
        else
            printf("!failed!\n");
    }
    else
        printf("!failed!\n");

    return rc;
}

/**
 * Compares the options with the option struct passed in.
 * @returns 0 on success.
 *          number of mismatches on error.
 * @param   pOpt
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
int CompareOptions(struct options *pOpt)
{
    int rc = 0;
    /*
     *  Check that the option struct is correct.
     */
    if (options.fQuiet != pOpt->fQuiet)
        printf("fQuiet = %d - should be %d\n", options.fQuiet, pOpt->fQuiet, rc++);
    if (options.usCom != pOpt->usCom)
        printf("usCom = %d - should be %d\n", options.usCom, pOpt->usCom, rc++);
    if (options.fLogging != pOpt->fLogging)
        printf("fLogging = %d - should be %d\n", options.fLogging, pOpt->fLogging, rc++);
    if (options.fKernel != pOpt->fKernel)
        printf("fKernel = %d - should be %d\n", options.fKernel, pOpt->fKernel, rc++);
    if (options.ulBuild != pOpt->ulBuild)
        printf("ulBuild = %d - should be %d\n", options.ulBuild, pOpt->ulBuild, rc++);
    if (options.usVerMajor != pOpt->usVerMajor)
        printf("usVerMajor = %d - should be %d\n", options.usVerMajor, pOpt->usVerMajor, rc++);
    if (options.usVerMinor != pOpt->usVerMinor)
        printf("usVerMinor = %d - should be %d\n", options.usVerMinor, pOpt->usVerMinor, rc++);
    if (options.fPE != pOpt->fPE)
        printf("fPE = %d - should be %d\n", options.fPE, pOpt->fPE, rc++);
    if (options.ulInfoLevel != pOpt->ulInfoLevel)
        printf("ulInfoLevel = %d - should be %d\n", options.ulInfoLevel, pOpt->ulInfoLevel, rc++);
    if (options.fElf != pOpt->fElf)
        printf("fElf = %d - should be %d\n", options.fElf, pOpt->fElf, rc++);
    if (options.fUNIXScript != pOpt->fUNIXScript)
        printf("fUNIXScript = %d - should be %d\n", options.fUNIXScript, pOpt->fUNIXScript, rc++);
    if (options.fREXXScript != pOpt->fREXXScript)
        printf("fREXXScript = %d - should be %d\n", options.fREXXScript, pOpt->fREXXScript, rc++);
    if (options.fJava != pOpt->fJava)
        printf("fJava = %d - should be %d\n", options.fJava, pOpt->fJava, rc++);
    if (options.fNoLoader != pOpt->fNoLoader)
        printf("fNoLoader = %d - should be %d\n", options.fNoLoader, pOpt->fNoLoader, rc++);
    if (options.cbSwpHeapInit != pOpt->cbSwpHeapInit)
        printf("cbSwpHeapInit = %d - should be %d\n", options.cbSwpHeapInit, pOpt->cbSwpHeapInit, rc++);
    if (options.cbSwpHeapMax != pOpt->cbSwpHeapMax)
        printf("cbSwpHeapMax = %d - should be %d\n", options.cbSwpHeapMax, pOpt->cbSwpHeapMax, rc++);
    if (options.cbResHeapInit != pOpt->cbResHeapInit)
        printf("cbResHeapInit = %d - should be %d\n", options.cbResHeapInit, pOpt->cbResHeapInit, rc++);
    if (options.cbResHeapMax != pOpt->cbResHeapMax)
        printf("cbResHeapMax = %d - should be %d\n", options.cbResHeapMax, pOpt->cbResHeapMax, rc++);

    return rc;
}




/**
 * Simulates a executable loading (no errors).
 * This test requires a PE executable file named ExecLoad1.exe which
 * imports the dll ExecLoad1d.dll.
 *
 * @returns 0 on success.
 *          > 0 on failure.
 * @sketch
 * @status
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 * @remark
 */
int TestCaseExeLoad1(void)
{
    APIRET rc;

    /*
     * Set global parameters... FIXME.
     */

    /*
     * Do the real execution.
     */
    printf("--- TestcaseExeLoad1 - loading win32ktst.exe (LX image) ----\n");
    rc = CalltkExecPgm(EXEC_LOAD, NULL, NULL, "win32ktst.exe");
    if (rc == NO_ERROR)
    {
        printf("--- TestcaseExeLoad1 - loading libconv.exe (LX image) ----\n");
        rc = CalltkExecPgm(EXEC_LOAD, NULL, NULL, "bin\\debug\\libconv.exe");
    }

    if (rc == NO_ERROR)
    {
        printf("--- TestcaseExeLoad1 - loading rexx\\tst.cmd (REXX script) ----\n");
        rc = CalltkExecPgm(EXEC_LOAD, NULL, NULL, "rexx\\tst.cmd");
    }

    if (rc == NO_ERROR)
    {
        printf("--- TestcaseExeLoad1 - loading SOL.EXE (PE image) ----\n");
        rc = CalltkExecPgm(EXEC_LOAD, NULL, NULL, "e:\\Win32Prog\\Sol\\Sol.exe");
    }

    /*
     * The test is successful if rc == NO_ERROR (== 0).
     */
    return rc;
}
