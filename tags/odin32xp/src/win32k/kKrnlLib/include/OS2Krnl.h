/* $Id: OS2Krnl.h,v 1.5 2002-12-19 01:49:06 bird Exp $
 *
 * OS/2 kernel structures, typedefs and macros.
 * Top-level include file.
 *
 * Copyright (c) 1999-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _OS2Krnl_h_
#define _OS2Krnl_h_



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
/*
 * "OS2KDefs.h"
 */
/* NOASM */
#include <os2def.h>
/* ASM */
#if !defined(OS2_INCLUDED)
    /* when used with h2inc.exe */
    #define UCHAR   unsigned char
    #define CHAR    char
    #define USHORT  unsigned short
    #define ULONG   unsigned long
    #define PCHAR   char *
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/* undefine everything defined below to quite compiler */
#undef  PAGESIZE
#undef  PAGESUB1
#undef  PAGEOFFSET
#undef  PAGESTART
#undef  PAGEALIGNUP
#undef  PAGEALIGNDOWN
#undef  PAGENEXT
#undef  PAGESTART
#undef  PAGESHIFT

#define PAGESHIFT                   12      /* bytes to pages or pages to bytes shift value. */
#define PAGESIZE                    0x1000UL/* pagesize on i386 */
#define PAGESUB1                    0x0FFFUL
#define PAGEALIGNMASK               0xFFFFF000UL
#define PAGEOFFSET(addr)     ((addr) &  PAGESUB1) /* Gets the offset into the page addr points into. */
#define PAGEALIGNUP(addr)    (((addr) + PAGESUB1) & PAGEALIGNMASK)  /* Aligns upwards */
#define PAGEALIGNDOWN(addr)  ((addr) &  PAGEALIGNMASK)              /* Aligns downwards */
#define PAGESTART(addr)      PAGEALIGNDOWN(addr)
#define PAGENEXT(addr)       PAGEALIGNUP(addr)


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
    #define INCL_OS2KRNL_VMOWNER
    #define INCL_OS2KRNL_SEL
    #define INCL_OS2KRNL_SM
    #define INCL_OS2KRNL_SEM
    #define INCL_OS2KRNL_TK
    #define INCL_OS2KRNL_PTDA
    #define INCL_OS2KRNL_PG
    #define INCL_OS2KRNL_TCB
    #define INCL_OS2KRNL_SEC
    #define INCL_OS2KRNL_LDR
#endif

#if defined(INCL_OS2KRNL_PTDA) && !defined(INCL_OS2KRNL_SEM)
    #define INCL_OS2KRNL_SEM
#endif

#ifdef INCL_OS2KRNL_IO
    #include "OS2KIO.h"
#endif

#ifdef INCL_OS2KRNL_VM
    #include "OS2KVM.h"
#endif

#ifdef INCL_OS2KRNL_VMOWNER
    #include "OS2KOwn.h"
#endif

#ifdef INCL_OS2KRNL_SM
    #include "OS2KSM.h"
#endif

#ifdef INCL_OS2KRNL_SEM
    #include "OS2KSEM.h"
#endif

#ifdef INCL_OS2KRNL_PTDA
    #include "OS2KPTDA.h"
#endif

#ifdef INCL_OS2KRNL_TCB
    #include "OS2KTCB.h"
#endif

#ifdef INCL_OS2KRNL_PG
    #include "OS2KPG.h"
#endif

#ifdef INCL_OS2KRNL_SEL
    #include "OS2KSEL.h"
#endif

#ifdef INCL_OS2KRNL_TK
    #include "OS2KTK.h"
#endif

#ifdef INCL_OS2KRNL_LDR
    #include "OS2KLDR.h"
#endif

#ifdef INCL_OS2KRNL_SEC
    #include "OS2KSEC.h"
#endif

#endif
