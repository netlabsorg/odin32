/* $Id: krnlPrivate.h,v 1.3 2001-09-26 04:01:36 bird Exp $
 *
 * Private header file for the krnl*.c* files.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
extern KSEMMTX                  kmtxImports;

extern char                     KKL_EntryTab[1];                /* calltaba.asm */
extern char                     KKL_EntryTabEND[1];             /* calltaba.asm */
extern char DATA16_INIT         KKL_EntryTabFixups[1];          /* calltaba.asm */
#ifdef _OS2KLDR_H_
extern MTE                      kKrnlLibMTE;                    /* calltaba.asm */
extern SMTE                     KKL_SwapMTE;                    /* calltaba.asm */
extern OTE                      KKL_ObjTab[4];                  /* calltaba.asm */
extern OTE                      KKL_ObjTab_DosCalls[20];        /* calltaba.asm */
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
extern void _Optlink LockedWrite(unsigned long ulAddr, unsigned char chOpcode, unsigned long ulDword);
extern int  _Optlink MakeCalltab16CodeSegment(void);

#endif
