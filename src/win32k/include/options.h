/* $Id: options.h,v 1.10 2000-02-25 18:15:06 bird Exp $
 *
 * Options.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _options_h_
#define _options_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* fKernel */
#define KF_UNI              0x00000000UL
#define KF_SMP              0x00000001UL

/* fPE */
#define FLAGS_PE_NOT        0x00000000UL
#define FLAGS_PE_PE2LX      0x00000001UL
#define FLAGS_PE_PE         0x00000002UL
#define FLAGS_PE_MIXED      0x00000003UL

/* ulInfoLevel */
#define INFOLEVEL_QUIET     0x00000000UL
#define INFOLEVEL_ERROR     0x00000001UL
#define INFOLEVEL_WARNING   0x00000002UL
#define INFOLEVEL_INFO      0x00000003UL
#define INFOLEVEL_INFOALL   0x00000004UL

/* default heapsizes */
#define CB_SWP_INIT         (1024*512)      /* 512KB */
#define CB_SWP_MAX          (1024*1024*16)  /*  16MB  */
#define CB_RES_INIT         (1024*256)      /* 256KB */
#define CB_RES_MAX          (1024*1024*10)  /*  10MB  */

/* default assignments */
#define DEFAULT_OPTION_ASSIGMENTS                           \
            {FALSE,                 /* fQuiet        */     \
            OUTPUT_COM2,            /* usCom         */     \
            FALSE,                  /* fLogging      */     \
            KF_UNI,                 /* fKernel       */     \
            ~0UL,                   /* ulBuild       */     \
            (unsigned short)~0,     /* usVerMajor    */     \
            (unsigned short)~0,     /* usVerMinor    */     \
            FLAGS_PE_PE2LX,         /* fPE           */     \
            INFOLEVEL_QUIET,        /* ulInfoLevel   */     \
            FALSE,                  /* fElf          */     \
            TRUE,                   /* fScript       */     \
            FALSE,                  /* fNoLoader     */     \
            CB_SWP_INIT,            /* cbSwpHeapInit */     \
            CB_SWP_MAX,             /* cbSwpHeapMax  */     \
            CB_RES_INIT,            /* cbResHeapInit */     \
            CB_RES_MAX}             /* cbResHeapMax  */



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
    ULONG       fLogging;               /* Logging. */

    /** @cat kernel selection */
    ULONG       fKernel;                /* Smp or uni kernel. */
    ULONG       ulBuild;                /* Kernel build. */
    USHORT      usVerMajor;             /* OS/2 major ver - 20 */
    USHORT      usVerMinor;             /* OS/2 minor ver - 30,40 */

    /** @cat Options affecting the generated LX executables */
    ULONG       fPE;                    /* Flags set the type of conversion. */
    ULONG       ulInfoLevel;            /* Pe2Lx InfoLevel. */

    /** @cat Options affecting the generated ELF executables */
    ULONG       fElf;                   /* Elf flags. */

    /** @cat Options affecting the script executables */
    ULONG       fScript;                /* Script flags. */

    /** @cat Options affecting the script executables */
    ULONG       fNoLoader;              /* No loader stuff. */

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
extern struct options options;      /* defined in d32globals.c */
#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map( options , "_options"  )
#endif

#endif

