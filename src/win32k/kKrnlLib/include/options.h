/* $Id: options.h,v 1.1 2002-03-31 19:32:54 bird Exp $
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


/* default heapsizes */
#define CB_SWP_INIT         ((unsigned long)1024*512)      /* 512KB */
#define CB_SWP_MAX          ((unsigned long)1024*1024*16)  /*  16MB  */
#define CB_RES_INIT         ((unsigned long)1024*256)      /* 256KB */
#define CB_RES_MAX          ((unsigned long)1024*1024*10)  /*  10MB  */

/*
 * COM-Port port numbers.
 */
#define OUTPUT_COM1 0x3f8
#define OUTPUT_COM2 0x2f8
#define OUTPUT_COM3 0x3e8
#define OUTPUT_COM4 0x2e8

/* NOINC */
/* default assignments */
#define DEFAULT_OPTION_ASSIGMENTS                           \
            {FALSE,                 /* fQuiet        */     \
            OUTPUT_COM2,            /* usCom         */     \
            TRUE,                   /* fLogging      */     \
            KF_UNI,                 /* fKernel       */     \
            ~0UL,                   /* ulBuild       */     \
            (unsigned short)~0,     /* usVerMajor    */     \
            (unsigned short)~0,     /* usVerMinor    */     \
            CB_SWP_INIT,            /* cbSwpHeapInit */     \
            CB_SWP_MAX,             /* cbSwpHeapMax  */     \
            CB_RES_INIT,            /* cbResHeapInit */     \
            CB_RES_MAX}             /* cbResHeapMax  */

#define isSMPKernel()               (options.fKernel & KF_SMP)
#define isUNIKernel()               (!(options.fKernel & KF_SMP))

#define isHighMemorySupported()     (options.ulBuild >= 14000 || isSMPKernel())

/* INC */

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/** Option struct */
#pragma pack(4)
struct kKLOptions
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
extern struct kKLOptions DATA16_GLOBAL options;  /* defined in d16globals.c */
#ifdef RING0
#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map(options, "_options")
#endif
#endif
/* INC */

/* NOINC */
#endif
/* INC */

