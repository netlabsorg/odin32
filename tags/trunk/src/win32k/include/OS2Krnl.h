/* $Id: OS2Krnl.h,v 1.11 2001-02-11 15:11:34 bird Exp $
 *
 * OS/2 kernel structures, typedefs and macros.
 *
 * Top-level include file.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _OS2Krnl_h_
#define _OS2Krnl_h_



/*
 * "OS2KDefs.h"
 */
#if !defined(OS2_INCLUDED)
    /* when used with h2inc.exe */
    #define UCHAR   unsigned char
    #define CHAR    char
    #define USHORT  unsigned short
    #define ULONG   unsigned long
    #define PCHAR   char *
#endif

#ifndef PAGESHIFT
#define PAGESHIFT                   12      /* bytes to pages or pages to bytes shift value. */
#endif
#ifndef PAGESIZE
#define PAGESIZE                    0x1000  /* pagesize on i386 */
#endif
#ifndef PAGEOFFSET
#define PAGEOFFSET(addr) ((addr) &  (PAGESIZE-1)) /* Gets the offset into the page addr points into. */
#endif
#ifndef PAGESTART
#define PAGESTART(addr)  ((addr) & ~(PAGESIZE-1)) /* Gets the address of the page addr points into. */
#endif


/*
 * Handles
 */
typedef USHORT HMTE, *PHMTE;            /* Module Table Entry (MTE) handle */
typedef USHORT HPTDA, *PHPTDA;          /* Per-Task Data Area (PTDA) handle */
#ifdef RING0
    typedef USHORT SFN, *PSFN;          /* System File Number (SFN) - ie. file handle. */
#else
    #define SFN HFILE
    #define PSFN PHFILE
#endif




/*
 * Calling convention for 32-bit OS/2 Kernel functions.
 */
#define KRNLCALL __stdcall

/*
 * Calling convention for Security Helpers.
 */
#define SECCALL  _System

/*
 *
 * Other Kernel Parts
 *
 */
#ifdef INCL_OS2KRNL_ALL
    #define INCL_OS2KRNL_IO
    #define INCL_OS2KRNL_VM
    #define INCL_OS2KRNL_SM
    #define INCL_OS2KRNL_SEM
    #define INCL_OS2KRNL_TK
    #define INCL_OS2KRNL_PTDA
    #define INCL_OS2KRNL_TCB
    #define INCL_OS2KRNL_SEC
    #define INCL_OS2KRNL_LDR
#endif

#if defined(INCL_OS2KRNL_PTDA) && !defined(INCL_OS2KRNL_SEM)
    #define INCL_OS2KRNL_SEM
#endif

#ifdef INCL_OS2KRNL_IO
    #include <OS2KIO.h>
#endif

#ifdef INCL_OS2KRNL_VM
    #include <OS2KVM.h>
#endif

#ifdef INCL_OS2KRNL_PG
    #include <OS2KPG.h>
#endif

#ifdef INCL_OS2KRNL_SM
    #include <OS2KSM.h>
#endif

#ifdef INCL_OS2KRNL_SEM
    #include <OS2KSEM.h>
#endif

#ifdef INCL_OS2KRNL_PTDA
    #include <OS2KPTDA.h>
#endif

#ifdef INCL_OS2KRNL_TK
    #include <OS2KTK.h>
#endif

#ifdef INCL_OS2KRNL_TCB
    #include <OS2KTCB.h>
#endif

#ifdef INCL_OS2KRNL_LDR
    #include <OS2KLDR.h>
#endif

#ifdef INCL_OS2KRNL_SEC
    #include <OS2KSEC.h>
#endif

#endif
