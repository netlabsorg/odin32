/* $Id: tstkKrnlLib.c,v 1.1 2002-04-07 22:39:16 bird Exp $
 *
 * kKrnlLib test module.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

#define SEL_FLATMASK        0x01fff0000
#define SEL_FLAT_SHIFT      0x0d
#define SEL_LDT_RPL3        0x07

#define SelToFlat(sel, off) \
    (PVOID)( (((unsigned)(sel) << SEL_FLAT_SHIFT) & SEL_FLAT_MASK) + (unsigned)(off))

#define FlatToSel(flataddr) \
    (PVOID)( ( (((unsigned)(flataddr) << 3) & 0xfff80000) | (SEL_LDT_RPL3 << 16) ) | ((unsigned)(flataddr) & 0xffff) )

#define assert(expr) ((expr) ? (void)0 : __interrupt(3))

#define DWORD   ULONG
#define WORD    USHORT

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#define INCL_BASE
#define INCL_SSTODS
#define FOR_EXEHDR 1
#include <os2.h>
#include <exe386.h>
#include <newexe.h>

#include "devSegDf.h"                   /* kKrnlLib segment definitions. */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "options.h"
#include "dev1632.h"
#include "dev32.h"
#include "devcmd.h"
#define INCL_OS2KRNL_ALL
#include "os2krnl.h"

#define INCL_KKL_ALL
#include "kKrnlLib.h"

#include "ProbKrnl.h"
#include "krnlPrivate.h"
#include "testcase.h"

#include <stdlib.h>




/** @design kKrnlLib Ring-3 Testing
 * I'll try to make it possible to test parts or all the kKrnlLib code in ring-3.
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
 * kKrnlHlp strategy routine. We'll also have to provide fakes for kernel probing,
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
*   Structures and Typedefs                                                    *
*******************************************************************************/
#if !defined(QS_MTE) || defined(QS_MODVER)
   /* From OS/2 Toolkit v4.5 (BSEDOS.H) */

   /* Global Record structure
    * Holds all global system information. Placed first in user buffer
    */
   typedef struct qsGrec_s {  /* qsGrec */
           ULONG         cThrds;
           ULONG         c32SSem;
           ULONG         cMFTNodes;
   }qsGrec_t;

   /*
    *      System wide MTE information
    *      ________________________________
    *      |       pNextRec                |----|
    *      |-------------------------------|    |
    *      |       hmte                    |    |
    *      |-------------------------------|    |
    *      |       ctImpMod                |    |
    *      |-------------------------------|    |
    *      |       ctObj                   |    |
    *      |-------------------------------|    |
    *      |       pObjInfo                |----|----------|
    *      |-------------------------------|    |          |
    *      |       pName                   |----|----|     |
    *      |-------------------------------|    |    |     |
    *      |       imported module handles |    |    |     |
    *      |          .                    |    |    |     |
    *      |          .                    |    |    |     |
    *      |          .                    |    |    |     |
    *      |-------------------------------| <--|----|     |
    *      |       "pathname"              |    |          |
    *      |-------------------------------| <--|----------|
    *      |       Object records          |    |
    *      |       (if requested)          |    |
    *      |_______________________________|    |
    *                                      <-----
    *      NOTE that if the level bit is set to QS_MTE, the base Lib record will be followed
    *      by a series of object records (qsLObj_t); one for each object of the
    *      module.
    */

   typedef struct qsLObjrec_s {  /* qsLOrec */
           ULONG         oaddr;  /* object address */
           ULONG         osize;  /* object size */
           ULONG         oflags; /* object flags */
   } qsLObjrec_t;

   typedef struct qsLrec_s {     /* qsLrec */
           void  FAR        *pNextRec;      /* pointer to next record in buffer */
           USHORT           hmte;           /* handle for this mte */
           USHORT           fFlat;          /* true if 32 bit module */
           ULONG            ctImpMod;       /* # of imported modules in table */
           ULONG            ctObj;          /* # of objects in module (mte_objcnt)*/
           qsLObjrec_t FAR  *pObjInfo;      /* pointer to per object info if any */
           UCHAR     FAR    *pName;         /* -> name string following struc */
   } qsLrec_t;



   /* Pointer Record Structure
    *      This structure is the first in the user buffer.
    *      It contains pointers to heads of record types that are loaded
    *      into the buffer.
    */

   typedef struct qsPtrRec_s {   /* qsPRec */
           qsGrec_t        *pGlobalRec;
           void            *pProcRec;      /* ptr to head of process records */
           void            *p16SemRec;     /* ptr to head of 16 bit sem recds */
           void            *p32SemRec;     /* ptr to head of 32 bit sem recds */
           void            *pMemRec;       /* ptr to head of shared mem recs */
           qsLrec_t        *pLibRec;       /* ptr to head of mte records */
           void            *pShrMemRec;    /* ptr to head of shared mem records */
           void            *pFSRec;        /* ptr to head of file sys records */
   } qsPtrRec_t;

#endif


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern BOOL     fInited;                /* malloc.c */
int             iTestcaseNo;
int             cObjectsFake = 14;
OTE             aKrnlOTE[24];


/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
#ifndef QS_MTE
   /* from OS/2 Toolkit v4.5 */

   APIRET APIENTRY DosQuerySysState(ULONG EntityList, ULONG EntityLevel, PID pid,
                                    TID tid, PVOID pDataBuf, ULONG cbBuf);
    #define QS_MTE         0x0004
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
void    syntax(void);
int     kernelInit(int iTest, int argc, char **argv);
void    workersinit(void);
void    initRPInit(RP32INIT *pRpInit, char *pszInitArgs);
int     tests(int iTest, int argc, char **argv);
int     TestCase1(int argc, char **argv);
int     TestCase2(void);
int     CompareOptions(struct kKLOptions *pOpt, ULONG fExpKernel, ULONG ulExpKernelBuild, USHORT usExpVerMajor, USHORT usExpVerMinor);
int     TestCaseExeLoad2(void);
int     WritekKrnlLibDll(void);


/**
 * Main function. Arguments _currently_ unused.
 */
int main(int argc, char **argv)
{
    int         rc;

    /*
     * Init devhelp subsystems.
     */
    dhinit();

    /*
     * Parse arguments.
     * (printf don't work before dhinit is called...)
     */
    if (argc < 2 || (iTestcaseNo = atoi(argv[1])) <= 0)
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
     * Init Kernel
     */
    if (!kernelInit(iTestcaseNo, argc, argv))
        return -2;

    /*
     * Thunk Stack to 16-bits.
     *
     * IMPORTANT! From now on SSToDS have to be used when making pointers
     *            to stack objects.
     */
    ThunkStack32To16();

    rc = tests(iTestcaseNo, argc, argv);

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
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
void    syntax(void)
{
    printf(
           "kKrnlLibTst.exe v%d.%d.%d - Ring 3 testing of kKrnlLib.sys\n"
           "syntax: kKrnlLibTst.exe <testcase number> [optional arguments]\n",
           0,0,4
           );
}


/**
 * test case 1: Load the specified kernel
 * other cases: Load running kernel.
 * @returns Success indicator. (true/false)
 * @param   iTest   Testcase number.
 * @param   argc    main argc
 * @param   argv    main argv
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int     kernelInit(int iTest, int argc, char **argv)
{
    static char     achBuffer[1024*256];
    char            szError[256];
    HMODULE         hmod = NULLHANDLE;
    int             rc;
    char            szName[CCHMAXPATH];
    char *          pszSrcName;
    char *          pszTmp;
    ULONG           ulAction;
    HFILE           hFile;
    struct e32_exe* pe32 = (struct e32_exe*)(void*)&achBuffer[0];
    qsPtrRec_t *    pPtrRec = (qsPtrRec_t*)(void*)&achBuffer[0];
    qsLrec_t *      pLrec;
    int             i;
    FILESTATUS3     fsts3;

    /*
     * If not testcase 1, use the running kernel.
     */
    if (iTest != 1)
    {
        ULONG   ulBootDrv = 3;
        pszSrcName = "c:\\os2krnl";
        DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, SSToDS(&ulBootDrv), sizeof(ulBootDrv));
        pszSrcName[0] = (char)(ulBootDrv + 'a' - 1);
    }
    else
    {
        if (argc < 3)
        {
            printf("Missing parameter!\n");
            return FALSE;
        }
        pszSrcName = argv[2];
    }

    /*
     * Make a temporary copy of the kernel.
     */
    if (DosScanEnv("TMP", &pszTmp) != NO_ERROR || pszTmp == NULL)
    {
        printf("Environment variable TMP is not set.\n");
        return FALSE;
    }
    strcpy(szName, pszTmp);
    if (szName[strlen(pszTmp) - 1] != '\\' && szName[strlen(pszTmp) - 1] != '/')
        strcat(szName, "\\");
    strcat(szName, "os2krnl");
    rc = DosCopy(pszSrcName, szName, DCPY_EXISTING);
    if (rc != NO_ERROR)
    {
        printf("Failed to copy %s to %s.\n", pszSrcName, szName);
        return FALSE;
    }
    if (DosQueryPathInfo(szName, FIL_STANDARD, &fsts3, sizeof(fsts3)) != NO_ERROR
        ||  !(fsts3.attrFile = FILE_ARCHIVED)
        ||  DosSetPathInfo(szName, FIL_STANDARD, &fsts3, sizeof(fsts3), 0) != NO_ERROR
        )
    {
        printf("Failed to set attributes for %s.\n", szName);
        return FALSE;
    }

    /*
     * Patch the kernel.
     *      Remove the entrypoint.
     */
    ulAction = 0;
    rc = DosOpen(szName, &hFile, &ulAction, 0, FILE_NORMAL,
                 OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                 OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
                 NULL);
    if (rc != NO_ERROR)
    {
        printf("Failed to open temporary kernel file. rc = %d\n", rc);
        return FALSE;
    }
    rc = DosRead(hFile, &achBuffer[0], 0x200, &ulAction);
    if (rc != NO_ERROR)
    {
        DosClose(hFile);
        printf("Failed to read LX header from temporary kernel file.\n");
        return FALSE;
    }
    pe32 = (struct e32_exe*)(void*)&achBuffer[*(unsigned long*)(void*)&achBuffer[0x3c]];
    if (*(PUSHORT)pe32->e32_magic != E32MAGIC)
    {
        DosClose(hFile);
        printf("Failed to read LX header from temporary kernel file (2).\n");
        return FALSE;
    }
    pe32->e32_eip = 0;
    pe32->e32_startobj = 0;
    pe32->e32_mflags &= ~(E32LIBTERM | E32LIBINIT);
    if ((rc = DosSetFilePtr(hFile, *(unsigned long*)(void*)&achBuffer[0x3c], FILE_BEGIN, &ulAction)) != NO_ERROR
        || (rc = DosWrite(hFile, pe32, sizeof(struct e32_exe), &ulAction)) != NO_ERROR)
    {
        DosClose(hFile);
        printf("Failed to write patched LX header to temporary kernel file.\n");
        return FALSE;
    }
    DosClose(hFile);

    /*
     * Load the module.
     */
    rc = DosLoadModule(szError, sizeof(szError), szName, SSToDS(&hmod));
    if (rc != NO_ERROR && (rc != ERROR_INVALID_PARAMETER && hmod == NULLHANDLE))
    {
        printf("Failed to load OS/2 kernel image %s.");
        return FALSE;
    }

    /*
     * Get object information.
     */
    rc = DosQuerySysState(QS_MTE, QS_MTE, 0L, 0L, pPtrRec, sizeof(achBuffer));
    if (rc != NO_ERROR)
    {
        printf("DosQuerySysState failed with rc=%d.\n", rc);
        return FALSE;
    }

    pLrec = pPtrRec->pLibRec;
    while (pLrec != NULL)
    {
        /*
         * Bug detected in OS/2 FP13. Probably a problem which occurs
         * in _LDRSysMteInfo when qsCheckCache is calle before writing
         * object info. The result is that the cache flushed and the
         * attempt of updating the qsLrec_t next and object pointer is
         * not done. This used to work earlier and on Aurora AFAIK.
         *
         * The fix for this problem is to check if the pObjInfo is NULL
         * while the number of objects isn't 0 and correct this. pNextRec
         * will also be NULL at this time. This will be have to corrected
         * before we exit the loop or moves to the next record.
         * There is also a nasty alignment of the object info... Hope
         * I got it right. (This aligment seems new to FP13.)
         */
        if (pLrec->pObjInfo == NULL /*&& pLrec->pNextRec == NULL*/ && pLrec->ctObj > 0)
            {
            pLrec->pObjInfo = (qsLObjrec_t*)(void*)(
                (char*)(void*)pLrec
                + ((sizeof(qsLrec_t)                            /* size of the lib record */
                   + pLrec->ctImpMod * sizeof(short)            /* size of the array of imported modules */
                   + strlen((char*)(void*)pLrec->pName) + 1     /* size of the filename */
                   + 3) & ~3));                                 /* the size is align on 4 bytes boundrary */
            pLrec->pNextRec = (qsLrec_t*)(void*)((char*)(void*)pLrec->pObjInfo
                                                 + sizeof(qsLObjrec_t) * pLrec->ctObj);
            }
        if (pLrec->hmte == hmod)
            break;

        /*
         * Next record
         */
        pLrec = (qsLrec_t*)pLrec->pNextRec;
    }

    if (pLrec == NULL)
    {
        printf("DosQuerySysState(os2krnl): not found\n");
        return FALSE;
    }
    if (pLrec->pObjInfo == NULL)
    {
        printf("DosQuerySysState(os2krnl): no object info\n");
        return FALSE;
    }

    /*
     * Fill the aKrnlOTE array.
     */
    for (i = 0; i < pLrec->ctObj; i++)
    {
        aKrnlOTE[i].ote_size    = pLrec->pObjInfo[i].osize;
        aKrnlOTE[i].ote_base    = pLrec->pObjInfo[i].oaddr;
        aKrnlOTE[i].ote_flags   = pLrec->pObjInfo[i].oflags;
        aKrnlOTE[i].ote_pagemap = i > 0 ? aKrnlOTE[i-1].ote_pagemap + aKrnlOTE[i-1].ote_mapsize : 0;
        aKrnlOTE[i].ote_mapsize = (pLrec->pObjInfo[i].osize + 0x0FFF) / 0x1000;
        aKrnlOTE[i].ote_sel     = (USHORT)FlatToSel(pLrec->pObjInfo[i].oaddr);
        aKrnlOTE[i].ote_hob     = 0;
    }
    cObjectsFake = pLrec->ctObj;

    return TRUE;
}


/**
 * Initiates a init reqest packet.
 * @param   pRpInit         Pointer to request packet to init.
 * @param   pszInitArgs     Pointer to init arguments.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
void initRPInit(RP32INIT *pRpInit, char *pszInitArgs)
{
    /* kKrnlHlp */
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
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int tests(int iTest, int argc, char **argv)
{
    int     rc;

    printf("-------------------------------- Testcase %d:\n", iTest);
    switch (iTest)
    {
        case 1:     rc = TestCase1(argc, argv);   break;
        case 2:     rc = TestCase2();   break;

        default:
            printf("testcase no. %d is not found\n", iTest);
            rc = -2;
    }
    printf("result %d -----------------------------------\n", rc);

    KNOREF(argc);
    KNOREF(argv);
    return rc;
}



/**
 * Test case 1.
 * Checks that default initiation works fine for a given kernel.
 *
 * Syntax:  win32ktst.exe 1 <os2krnl> <majorver> <minorver> <build> <kerneltype: S|U|4> <buildtype: A|H|R> [rev] [os2krnl.sym]
 *
 * @sketch  Create init packet with no arguments.
 *          Initiate kKrnlHlp
 *          Create init packet with no arguments.
 *          Initiate kKrnlLib
 * @returns 0 on success.
 *          1 on failure.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int TestCase1(int argc, char **argv)
{
    static char szInitArgs[CCHMAXPATH + 10];
    int         rc = 1;
    RP32INIT    rpinit;

    /* verify argument count */
    if (argc < 8 || argc > 10)
    {
        printf("Invalid parameter count for testcase 1.\n");
        return ERROR_INVALID_PARAMETER;
    }

    /* init fake variabels */
    _usFakeVerMajor = (USHORT)atoi(argv[3]);
    _usFakeVerMinor = (USHORT)atoi(argv[4]);

    /* make init string */
    strcpy(szInitArgs, "-w3");
    if (argc >= 9 && argv[argc-1][1] != '\0')
        strcat(strcat(szInitArgs, " -S:"), argv[argc-1]);

    /* kKrnlHlp */
    initRPInit(SSToDS(&rpinit), szInitArgs);
    rc = InitkKrnlHlp(&rpinit);              /* no SSToDS! */
    printf("InitkKrnlHlp returned status=0x%04x\n", rpinit.rph.Status);
    if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
    {
        /* kKrnlLib */
        initRPInit(SSToDS(&rpinit), szInitArgs);
        rc = InitkKrnlLib(&rpinit);       /* no SSToDS! */
        printf("InitkKrnlLib returned status=0x%04x\n", rpinit.rph.Status);
        if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
        {
            struct kKLOptions opt = DEFAULT_OPTION_ASSIGMENTS;
            ULONG   fExpKernel = 0;
            if (argv[6][0] == 'S')  fExpKernel |= KF_SMP;
            if (argv[6][0] == '4')  fExpKernel |= KF_W4;
            if (argv[6][0] == 'U')  fExpKernel |= KF_UNI;
            if (argv[7][0] == 'A')  fExpKernel |= KF_ALLSTRICT;
            if (argv[7][0] == 'H')  fExpKernel |= KF_HALFSTRICT;

            if (argc >= 9 && argv[8][1] == '\0')
                switch (argv[8][0])
                {
                    case '\0': break;
                    default:
                    fExpKernel |= (argv[8][0] - (argv[8][0] >= 'a' ? 'a'-1 : 'A'-1)) << KF_REV_SHIFT;
                }
            rc = CompareOptions(SSToDS(&opt), fExpKernel, atoi(argv[5]), (USHORT)atoi(argv[3]), (USHORT)atoi(argv[4]));
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
 * Checks that all parameters are read correctly (1).
 *
 * @sketch  Create init packet with no arguments.
 *          Initiate kKrnlHlp
 *          Create init packet with no arguments.
 *          Initiate kKrnlLib
 * @returns 0 on success.
 *          1 on failure.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int TestCase2(void)
{
    int         rc = 1;
    RP32INIT    rpinit;
    char *      pszInitArgs = "-C1 -L:E -Verbose -Elf:Yes -Pe:Mixed -Script:Yes -W4 -Heap:512000 -ResHeap:0256000 -HeapMax:4096000 -ResHeapMax:0x100000";

    options.fLogging = TRUE;

    /* kKrnlHlp */
    initRPInit(SSToDS(&rpinit), pszInitArgs);
    rc = InitkKrnlHlp(&rpinit);              /* no SSToDS! */
    printf("InitkKrnlHlp returned status=0x%04x\n", rpinit.rph.Status);
    if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
    {
        /* kKrnlLib */
        initRPInit(SSToDS(&rpinit), pszInitArgs);
        rc = InitkKrnlLib(&rpinit);       /* no SSToDS! */
        printf("InitkKrnlLib returned status=0x%04x\n", rpinit.rph.Status);
        if ((rpinit.rph.Status & (STDON | STERR)) == STDON)
        {
            struct kKLOptions opt = DEFAULT_OPTION_ASSIGMENTS;
            opt.cbSwpHeapInit   = 512000;
            opt.cbSwpHeapMax    = 4096000;
            opt.cbResHeapInit   = 0256000;
            opt.cbResHeapMax    = 0x100000;
            opt.fQuiet          = FALSE;
            opt.fLogging        = TRUE;
            opt.usCom           = OUTPUT_COM1;

            rc = CompareOptions(SSToDS(&opt), -1, -1, (USHORT)-1, (USHORT)-1);
            if (rc == NO_ERROR)
            {
                rc = WritekKrnlLibDll();
            }
        }
        else
            printf("!failed!\n");
    }
    else
        printf("!failed!\n");

    return rc;
}



int WritekKrnlLibDll(void)
{
    ULONG   ulAction;
    int     rc;
    HFILE   hFile = NULLHANDLE;

    /*
     * Open the file.
     */
    rc = DosOpen("kKrnlLib.Dll", SSToDS(&hFile), SSToDS(&ulAction), 0, FILE_NORMAL,
                 OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                 OPEN_FLAGS_SEQUENTIAL | OPEN_SHARE_DENYREAD | OPEN_ACCESS_WRITEONLY,
                 NULL);
    if (rc == NO_ERROR)
    {
        static struct e32_exe   exehdr;
        static struct o32_obj   aObjTab[24];
        struct o32_map          page;
        int                     i;
        int                     j;
        ULONG                   ulWritten;
        ULONG                   ulPos;
        ULONG                   ulNULL = 0;


        /* make exehdr */
        memset(&exehdr, 0, sizeof(exehdr));
        exehdr.e32_magic[0] = E32MAGIC1;
        exehdr.e32_magic[1] = E32MAGIC2;
        exehdr.e32_cpu      = E32CPU486;
        exehdr.e32_os       = NE_OS2;
        exehdr.e32_mflags   = E32MODDLL | E32PMW;
        exehdr.e32_mpages   = 0;
        for (i = 0; i < KKL_SwapMTE.smte_objcnt; i++)
            exehdr.e32_mpages += (KKL_SwapMTE.smte_objtab[i].ote_size + 0xfff) >> 12;
        exehdr.e32_startobj = 2;
        exehdr.e32_eip      = 0;
        exehdr.e32_pagesize = 0x1000;
        exehdr.e32_fixupsize= exehdr.e32_mpages * sizeof(unsigned long)
                            + 4;
        exehdr.e32_ldrsize  = KKL_SwapMTE.smte_objcnt * sizeof(struct o32_obj)
                            + exehdr.e32_mpages * sizeof(struct o32_map)
                            + &KKL_ResNameTabEND[0] - (char*)KKL_SwapMTE.smte_restab
                            + exehdr.e32_mpages * sizeof(unsigned long)
                            + 4;
        exehdr.e32_objtab   = sizeof(exehdr);
        exehdr.e32_objcnt   = KKL_SwapMTE.smte_objcnt;
        exehdr.e32_objmap   = exehdr.e32_objtab + exehdr.e32_objcnt * sizeof(struct o32_obj);
        exehdr.e32_restab   = exehdr.e32_objmap + exehdr.e32_mpages * sizeof(struct o32_map);
        exehdr.e32_enttab   = exehdr.e32_restab + &KKL_ResNameTabEND[0] - (char*)KKL_SwapMTE.smte_restab;
        exehdr.e32_fpagetab = exehdr.e32_enttab + &KKL_EntryTabEND[0] - (char*)KKL_SwapMTE.smte_enttab;
        exehdr.e32_frectab  = exehdr.e32_fpagetab+exehdr.e32_mpages * sizeof(unsigned long);
        exehdr.e32_datapage = exehdr.e32_frectab + 4;
        exehdr.e32_nrestab  = 0;
        exehdr.e32_cbnrestab= 0;

        /* make object table */
        memcpy(&aObjTab[0], KKL_SwapMTE.smte_objtab, KKL_SwapMTE.smte_objcnt * sizeof(struct o32_obj));
        for (i = 0, j = 1; i < KKL_SwapMTE.smte_objcnt; i++)
        {
            aObjTab[i].o32_reserved = 0;
            aObjTab[i].o32_pagemap = j;
            j += aObjTab[i].o32_mapsize = (aObjTab[i].o32_size + 0xfff) >> 12;
            printf("obj %02d: base=%08x size=%08x page=%08x pages=%08x\n",
                   i, aObjTab[i].o32_base, aObjTab[i].o32_size, aObjTab[i].o32_pagemap, aObjTab[i].o32_mapsize);
        }

        /* write the header stuff */
        DosWrite(hFile, &exehdr, sizeof(exehdr), SSToDS(&ulWritten));
        assert(!DosSetFilePtr(hFile, 0, FILE_CURRENT, SSToDS(&ulPos)) && ulPos == exehdr.e32_objtab);
        DosWrite(hFile, &aObjTab[0], exehdr.e32_objcnt * sizeof(struct o32_obj), SSToDS(&ulWritten));
        assert(!DosSetFilePtr(hFile, 0, FILE_CURRENT, SSToDS(&ulPos)) && ulPos == exehdr.e32_objmap);
        for (i = 0, page.o32_pagedataoffset = 0; i < KKL_SwapMTE.smte_objcnt; i++)
        {
            for (j = 0; j < aObjTab[i].o32_mapsize;j++, page.o32_pagedataoffset += 0x1000)
            {
                page.o32_pageflags = VALID;
                page.o32_pagesize = 0x1000;
                DosWrite(hFile, SSToDS(&page), sizeof(struct o32_map), SSToDS(&ulWritten));
            }
        }
        assert(!DosSetFilePtr(hFile, 0, FILE_CURRENT, SSToDS(&ulPos)) && ulPos == exehdr.e32_restab);
        DosWrite(hFile, &KKL_ResNameTab[0], &KKL_ResNameTabEND[0] - (char*)KKL_SwapMTE.smte_restab, SSToDS(&ulWritten));
        assert(!DosSetFilePtr(hFile, 0, FILE_CURRENT, SSToDS(&ulPos)) && ulPos == exehdr.e32_enttab);
        DosWrite(hFile, &KKL_EntryTab[0], &KKL_EntryTabEND[0] - (char*)KKL_SwapMTE.smte_enttab, SSToDS(&ulWritten));
        assert(!DosSetFilePtr(hFile, 0, FILE_CURRENT, SSToDS(&ulPos)) && ulPos == exehdr.e32_fpagetab);
        for (i = 0, page.o32_pagedataoffset = 0; i < KKL_SwapMTE.smte_objcnt; i++)
        {
            for (j = 0; j < aObjTab[i].o32_mapsize;j++, page.o32_pagedataoffset += 0x1000)
                DosWrite(hFile, SSToDS(&ulNULL), sizeof(ulNULL), SSToDS(&ulWritten));
        }
        assert(!DosSetFilePtr(hFile, 0, FILE_CURRENT, SSToDS(&ulPos)) && ulPos == exehdr.e32_frectab);
        DosWrite(hFile, SSToDS(&ulNULL), sizeof(ulNULL), SSToDS(&ulWritten));

        /* Write data pages. */
        assert(!DosSetFilePtr(hFile, 0, FILE_CURRENT, SSToDS(&ulPos)) && ulPos == exehdr.e32_datapage);
        for (i = 0; i < KKL_SwapMTE.smte_objcnt; i++)
            for (j = 0; j < aObjTab[i].o32_mapsize; j++)
            {
                static char achPage[0x1000];
                char    *pchData = (char*)aObjTab[i].o32_base + j * 0x1000;
                int k;
                if (aObjTab[i].o32_flags & (OBJINVALID | OBJIOPL))
                    memset(achPage[0], 0x1000, 0);
                else
                    for (k = 0; k < 0x1000; k++, pchData++)
                        achPage[k] = *pchData;
                DosWrite(hFile, &achPage[0], 0x1000, SSToDS(&ulWritten));
            }

        /*
         * Close file - We're finished in a way.
         */
        DosClose(hFile);
    }

    return 0;
}

/**
 * Compares the options with the option struct passed in.
 * @returns 0 on success.
 *          number of mismatches on error.
 * @param   pOpt                Expected option struct.
 * @param   fExpKernel          Expected stuff.
 * @param   ulExpKernelBuild    Expected stuff.
 * @param   usExpVerMajor       Expected stuff.
 * @param   usExpVerMinor       Expected stuff.
 * @status  completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
int CompareOptions(struct kKLOptions *pOpt, ULONG fExpKernel, ULONG ulExpKernelBuild,
                   USHORT usExpVerMajor, USHORT usExpVerMinor)
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
    if (ulExpKernelBuild != ~0UL)
    {
        if (fKernel != fExpKernel)
            printf("fKernel = %x - should be %x\n", fKernel, fExpKernel, rc++);
        if (ulKernelBuild != ulExpKernelBuild)
            printf("ulBuild = %d - should be %d\n", ulKernelBuild, ulExpKernelBuild, rc++);
        if (usVerMajor != usExpVerMajor)
            printf("usVerMajor = %d - should be %d\n", usVerMajor, usExpVerMajor, rc++);
        if (usVerMinor != usExpVerMinor)
            printf("usVerMinor = %d - should be %d\n", usVerMinor, usExpVerMinor, rc++);
    }
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
