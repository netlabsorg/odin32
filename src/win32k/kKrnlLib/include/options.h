/* $Id: options.h,v 1.4 2002-12-16 01:37:07 bird Exp $
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
            CB_SWP_INIT,            /* cbSwpHeapInit */     \
            CB_SWP_MAX,             /* cbSwpHeapMax  */     \
            CB_RES_INIT,            /* cbResHeapInit */     \
            CB_RES_MAX}             /* cbResHeapMax  */

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
extern struct kKLOptions    options;  /* defined in d16Globl.c */
extern USHORT               usVerMajor;
extern USHORT               usVerMinor;
extern char                 szSymbolFile[128];

#ifdef RING0
#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map(options,        "_options")
    #pragma map(usVerMajor,     "_usVerMajor")
    #pragma map(usVerMinor,     "_usVerMinor")
    #pragma map(szSymbolFile,   "_szSymbolFile")
#endif
#endif

/* INC */

/* NOINC */
#endif
/* INC */

