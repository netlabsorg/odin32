/* $Id: options.h,v 1.22.2.1 2001-09-27 03:08:19 bird Exp $
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

/* NOINC */
/* default assignments */
#define DEFAULT_OPTION_ASSIGMENTS                           \
            {FALSE,                 /* fQuiet        */     \
            FALSE,                  /* fLogging      */     \
            FLAGS_PE_PE,            /* fPE           */     \
            FLAGS_PEOO_FORCED,      /* fPEOneObject  */     \
            INFOLEVEL_QUIET,        /* ulInfoLevel   */     \
            FALSE,                  /* fElf          */     \
            TRUE,                   /* fUNIXScript   */     \
            TRUE,                   /* fREXXScript   */     \
            TRUE,                   /* fJava         */     \
            FALSE,                  /* fNoLoader     */     \
            TRUE,                   /* fDllFixes     */     \
            TRUE,                   /* fExeFixes     */     \
            FALSE,                  /* fForcePreload */     \
            FALSE                   /* fApiEnh       */     \
            }

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
struct Win32kOptions
{
    /** @cat misc */
    ULONG       fQuiet;                 /* Quiet initialization. */

    /** @cat logging options */
    USHORT      fLogging;               /* Logging. */

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

    /** @cat Options affecting the behaviour changes in the OS/2 loader */
    ULONG       fDllFixes;              /* Enables the long DLL name and non .DLL extention fixes. */
    ULONG       fExeFixes;              /* Enables EXE files to export entry points. */
    ULONG       fForcePreload;          /* Forces the loader to preload executable images. Handy for ICAT Ring-3 debugging. */
    ULONG       fApiEnh;                /* Enables the API enhancements */
};
#pragma pack()

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/* NOINC */
extern struct Win32kOptions DATA16_GLOBAL options;    /* defined in d16globals.c */
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

