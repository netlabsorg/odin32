/* $Id: options.h,v 1.4 1999-11-10 01:45:33 bird Exp $
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
#define KF_UNI      0x00000000UL
#define KF_SMP      0x00000001UL

/* fPE */
#define FLAGS_PE_NOT        0x00000000UL
#define FLAGS_PE_PE2LX      0x00000001UL
#define FLAGS_PE_PE         0x00000002UL
#define FLAGS_PE_MIXED      0x00000003UL

/* ulInfoLevel */
#define INFOLEVEL_QUIET      0x00000000UL
#define INFOLEVEL_ERROR      0x00000001UL
#define INFOLEVEL_WARNING    0x00000002UL
#define INFOLEVEL_INFO       0x00000003UL
#define INFOLEVEL_INFOALL    0x00000004UL

/* Set defaults. */
#define SET_OPTIONS_TO_DEFAULT(o)           \
            o.fQuiet      = FALSE;          \
            o.usCom       = OUTPUT_COM2;    \
            o.fLogging    = FALSE;          \
            o.fKernel     = KF_UNI;         \
            o.ulBuild     = ~0UL;           \
            o.usVerMajor  = ~0;             \
            o.usVerMinor  = ~0;             \
            o.fPE         = FLAGS_PE_PE2LX; \
            o.ulInfoLevel = INFOLEVEL_QUIET;\
            o.fElf        = FALSE;          \
            o.fScript     = FALSE;          \
            o.fNoLoader   = FALSE;          \
            o.cbHeap      = 0x100000; /* 1MB */ \
            o.cbHeapMax   = 0x100000; /* 1MB */ \
            o.cbHeapResident    = 0x10000; /* 64KB */ \
            o.cbHeapMaxResident = 0x1000;  /* 4KB */ \


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/** Option struct */
struct options
{
    /** @cat misc */
    BOOL        fQuiet;              /* Quiet initialization. */

    /** @cat logging options */
    USHORT      usCom;               /* Output port no. */
    BOOL        fLogging;            /* Logging. */

    /** @cat kernel selection */
    ULONG       fKernel;            /* Smp or uni kernel. */
    ULONG       ulBuild;            /* Kernel build. */
    USHORT      usVerMajor;         /* OS/2 major ver - 20 */
    USHORT      usVerMinor;         /* OS/2 minor ver - 30,40 */

    /** @cat Options affecting the generated LX executables */
    BOOL        fPE;                /* Flags set the type of conversion. */
    ULONG       ulInfoLevel;        /* Pe2Lx InfoLevel. */

    /** @cat Options affecting the generated ELF executables */
    BOOL        fElf;               /* Elf flags. */

    /** @cat Options affecting the script executables */
    BOOL        fScript;            /* Script flags. */

    /** @cat Options affecting the script executables */
    BOOL        fNoLoader;          /* No loader stuff. */

    /** @cat Options affecting the heap. */
    ULONG       cbHeap;             /* Initial heapsize. */
    ULONG       cbHeapMax;          /* Maximum heapsize. */
    ULONG       cbHeapResident;     /* Initial residentheapsize. */
    ULONG       cbHeapMaxResident;  /* Maxiumem residentheapsize. */
};


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern struct options options; /* defined in d32globals.c */

#endif

