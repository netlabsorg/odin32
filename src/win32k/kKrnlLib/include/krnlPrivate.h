/* $Id: krnlPrivate.h,v 1.5 2002-12-19 01:49:06 bird Exp $
 *
 * Private header file for the krnl part.
 *
 * Copyright (c) 2001-2003 knut st. osmundsen <bird@anduin.net>
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

#ifndef _krnlPrivate_h_
#define _krnlPrivate_h_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define IMPORT_LOCK()       KSEMRequestMutex(&kmtxImports, KSEM_INDEFINITE_WAIT)
#define IMPORT_UNLOCK()     KSEMReleaseMutex(&kmtxImports)


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifdef _OS2KSEM_h_
extern KSEMMTX                  kmtxImports;
#endif

extern char                     KKL_EntryTab[1];                /* calltaba.asm */
extern char                     KKL_EntryTabEND[1];             /* calltaba.asm */
extern char                     KKL_EntryTabFixups[1];          /* calltaba.asm */
#ifdef _OS2KLDR_H_
extern MTE                      kKrnlLibMTE;                    /* calltaba.asm */
extern SMTE                     KKL_SwapMTE;                    /* calltaba.asm */
extern OTE                      KKL_ObjTab[4];                  /* calltaba.asm */
extern OTE                      KKL_ObjTab_DosCalls[20];        /* calltaba.asm */
extern PMTE                     pKrnlMTE;                       /* krnlInit.c */
extern PSMTE                    pKrnlSMTE;                      /* krnlInit.c */
extern POTE                     pKrnlOTE;                       /* krnlInit.c */
extern int                      cKernelObjects;                 /* krnlInit.c */
#endif
extern char                     KKL_ResNameTab[1];              /* calltaba.asm */
extern char                     KKL_ResNameTabEND[1];           /* calltaba.asm */
extern char                     callTab[1];                     /* calltaba.asm */
extern char                     callTabEND[1];                  /* calltaba.asm */
extern char                     callTab16[1];                   /* calltaba.asm */
extern char                     callTab16END[1];                /* calltaba.asm */
extern unsigned                 auNopFuncs[NBR_OF_KRNLIMPORTS]; /* calltaba.asm */



/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
extern void _Optlink    krnlLockedWrite(unsigned long ulAddr, unsigned char chOpcode, unsigned long ulDword);
extern int  _System     krnlMakeCalltab16CodeSegment(void);
extern int              krnlLoadKernelSym(void);
extern int              krnlVerifyImportTab(void);
#ifdef _OS2KLDR_H_
extern int              krnlLoadKernelSymFile(const char *pszFilename, POTE paOTEs, int cOTEs);
extern PMTE _System     krnlGetOS2KrnlMTE(void);
#endif
extern int              krnlInterpretProlog32(char *pach);
extern int              krnlInterpretProlog16(char *pach);


#endif
