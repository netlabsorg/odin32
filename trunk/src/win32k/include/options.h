/* $Id: options.h,v 1.23 2001-09-28 07:43:38 sandervl Exp $
 *
 * Options.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/* NOINC */
#ifndef _options_h_
#define _options_h_
/* INC */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* fKernel */
#define KF_UNI              0x0000
#define KF_SMP              0x0001
#define KF_W4               0x0002
#define KF_REV_MASK         0x0FF0
#define KF_REV_SHIFT        4
#define KF_REV_0            0x0000
#define KF_REV_A            0x0010
#define KF_REV_B            0x0020
#define KF_REV_C            0x0030
#define KF_REV_D            0x0040
#define KF_REV_E            0x0050
#define KF_REV_F            0x0060
#define KF_REV_G            0x0070
#define KF_REV_H            0x0080
#define KF_REV_I            0x0090
#define KF_REV_J            0x00a0
#define KF_REV_K            0x00b0
#define KF_REV_L            0x00c0
#define KF_REV_M            0x00d0
#define KF_REV_N            0x00e0
#define KF_REV_O            0x00f0
#define KF_REV_P            0x0100
#define KF_REV_Q            0x0110
#define KF_REV_R            0x0120
#define KF_REV_S            0x0130
#define KF_REV_T            0x0140
#define KF_REV_U            0x0150
#define KF_REV_V            0x0160
#define KF_REV_W            0x0170
#define KF_REV_X            0x0180
#define KF_REV_Y            0x0190
#define KF_REV_Z            0x01a0
#define KF_REV_ECS          0x0900
#define KF_DEBUG            0x1000
#define KF_HAS_DEBUGTYPE    0x2000
#define KF_ALLSTRICT        0x3000
#define KF_HALFSTRICT       0x7000

/* fPE */
#define FLAGS_PE_NOT        0x00000000UL
#define FLAGS_PE_PE2LX      0x00000001UL
#define FLAGS_PE_PE         0x00000002UL
#define FLAGS_PE_MIXED      0x00000003UL

/* fPEOneObject */
#define FLAGS_PEOO_DISABLED 0x00000000UL
#define FLAGS_PEOO_ENABLED  0x00000001UL
#define FLAGS_PEOO_FORCED   0x00000002UL

/* ulInfoLevel */
#define INFOLEVEL_QUIET     0x00000000UL
#define INFOLEVEL_ERROR     0x00000001UL
#define INFOLEVEL_WARNING   0x00000002UL
#define INFOLEVEL_INFO      0x00000003UL
#define INFOLEVEL_INFOALL   0x00000004UL

/* default heapsizes */
#define CB_SWP_INIT         ((unsigned long)1024*512)      /* 512KB */
#define CB_SWP_MAX          ((unsigned long)1024*1024*16)  /*  16MB  */
#define CB_RES_INIT         ((unsigned long)1024*256)      /* 256KB */
#define CB_RES_MAX          ((unsigned long)1024*1024*10)  /*  10MB  */

/* NOINC */
/* default assignments */
#define DEFAULT_OPTION_ASSIGMENTS                           \
            {FALSE,                 /* fQuiet        */     \
            OUTPUT_COM2,            /* usCom         */     \
            FALSE,                  /* fLogging      */     \
            KF_UNI,                 /* fKernel       */     \
            ~0UL,                   /* ulBuild       */     \
            (unsigned short)~0,     /* usVerMajor    */     \
            (unsigned short)~0,     /* usVerMinor    */     \
            FLAGS_PE_PE,            /* fPE           */     \
            FLAGS_PEOO_FORCED,      /* fPEOneObject  */     \
            INFOLEVEL_QUIET,        /* ulInfoLevel   */     \
            FALSE,                  /* fElf          */     \
            TRUE,                   /* fUNIXScript   */     \
            TRUE,                   /* fREXXScript   */     \
            TRUE,                   /* fJava         */     \
            FALSE,                  /* fNoLoader     */     \
            FALSE,                  /* fSkipFixups   */     \
            NULL,                   /* pszCustomDll  */     \
            NULL,                   /* pszCustomExports  */ \
            TRUE,                   /* fDllFixes     */     \
            TRUE,                   /* fExeFixes     */     \
            FALSE,                  /* fForcePreload */     \
            FALSE,                  /* fApiEnh       */     \
            CB_SWP_INIT,            /* cbSwpHeapInit */     \
            CB_SWP_MAX,             /* cbSwpHeapMax  */     \
            CB_RES_INIT,            /* cbResHeapInit */     \
            CB_RES_MAX}             /* cbResHeapMax  */

#define isAnyLoaderEnabled()        (!options.fNoLoader && \
                                    (isPELoaderEnabled() || isELFEnabled() || isUNIXScriptEnabled() || isREXXScriptEnabled() || isJAVAEnabled()))
#define isPELoaderEnabled()         (options.fPE != FLAGS_PE_NOT)
#define isPELoaderDisabled()        (options.fPE == FLAGS_PE_NOT)
#define isPe2LxLoaderEnabled()      (options.fPE == FLAGS_PE_PE2LX)
#define isMixedPeLoaderEnabled()    (options.fPE == FLAGS_PE_MIXED)
#define isPEOneObjectEnabled()      (options.fPEOneObject == FLAGS_PEOO_ENABLED)
#define isPEOneObjectDisabled()     (options.fPEOneObject == FLAGS_PEOO_DISABLED)
#define isPEOneObjectForced()       (options.fPEOneObject == FLAGS_PEOO_FORCED)

#define isELFDisabled()             (!options.fElf)
#define isELFEnabled()              (options.fElf)
#define isUNIXScriptDisabled()      (!options.fUNIXScript)
#define isUNIXScriptEnabled()       (options.fUNIXScript)
#define isREXXScriptDisabled()      (!options.fREXXScript)
#define isREXXScriptEnabled()       (options.fREXXScript)
#define isJAVADisabled()            (!options.fJava)
#define isJAVAEnabled()             (options.fJava)

#define areFixupsEnabled()          (!options.fSkipFixups)
#define areFixupsDisabled()         (options.fSkipFixups)

#define hasCustomDll()              (options.pszCustomDll != NULL)
#define hasCustomExports()          (options.pszCustomExports != NULL)

#define isSMPKernel()               (options.fKernel & KF_SMP)
#define isUNIKernel()               (!(options.fKernel & KF_SMP))

#define isHighMemorySupported()     (options.ulBuild >= 14000 || isSMPKernel())

#define isDllFixesEnabled()         (options.fDllFixes)
#define isDllFixesDisabled()        (!options.fDllFixes)

#define isExeFixesEnabled()         (options.fExeFixes)
#define isExeFixesDisabled()        (!options.fExeFixes)

#define isForcePreloadEnabled()     (options.fForcePreload)
#define isForcePreloadDisabled()    (!options.fForcePreload)

#define isApiEnhEnabled()           (options.fApiEnh)
#define isApiEnhDisabled()          (!options.fApiEnh)

/* INC */

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/** Option struct */
#pragma pack(4)
struct options
{
    /** @cat misc */
    ULONG       fQuiet;                 /* Quiet initialization. */

    /** @cat logging options */
    USHORT      usCom;                  /* Output port no. */
    USHORT      fLogging;               /* Logging. */

    /** @cat kernel selection */
    ULONG       fKernel;                /* Smp or uni kernel. */
    ULONG       ulBuild;                /* Kernel build. */
    USHORT      usVerMajor;             /* OS/2 major ver - 20 */
    USHORT      usVerMinor;             /* OS/2 minor ver - 30,40 */

    /** @cat Options affecting the generated LX executables */
    ULONG       fPE;                    /* Flags set the type of conversion. */
    ULONG       fPEOneObject;           /* All in one object. */
    ULONG       ulInfoLevel;            /* Pe2Lx InfoLevel. */

    /** @cat Options affecting the generated ELF executables */
    ULONG       fElf;                   /* Elf flags. */

    /** @cat Options affecting the UNIX script executables */
    ULONG       fUNIXScript;            /* UNIX script flags. */

    /** @cat Options affecting the REXX script executables */
    ULONG       fREXXScript;            /* REXX script flags. */

    /** @cat Options affecting the JAVA executables */
    ULONG       fJava;                  /* Java flags. */

    /** @cat Options affecting the  executables */
    ULONG       fNoLoader;              /* No loader stuff. !FIXME! We should import / functions even if this flag is set!!! */

    ULONG       fSkipFixups;
    PSZ         pszCustomDll;           /* pointer to custom odin dll name */
    PSZ         pszCustomExports;       /* pointer to custom export table */

    /** @cat Options affecting the behaviour changes in the OS/2 loader */
    ULONG       fDllFixes;              /* Enables the long DLL name and non .DLL extention fixes. */
    ULONG       fExeFixes;              /* Enables EXE files to export entry points. */
    ULONG       fForcePreload;          /* Forces the loader to preload executable images. Handy for ICAT Ring-3 debugging. */
    ULONG       fApiEnh;                /* Enables the API enhancements */

    /** @cat Options affecting the heap. */
    ULONG       cbSwpHeapInit;          /* Initial heapsize. */
    ULONG       cbSwpHeapMax;           /* Maximum heapsize. */
    ULONG       cbResHeapInit;          /* Initial residentheapsize. */
    ULONG       cbResHeapMax;           /* Maxiumem residentheapsize. */
};
#pragma pack()

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/* NOINC */
extern struct options DATA16_GLOBAL options;    /* defined in d16globals.c */
extern char   szWin32kIni[160];                 /* defined in d16globals.c */
#ifdef RING0
#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map( options , "_options"  )
    #pragma map( szWin32kIni, "_szWin32kIni" )
#endif
#endif
/* INC */

/* NOINC */
#endif
/* INC */

