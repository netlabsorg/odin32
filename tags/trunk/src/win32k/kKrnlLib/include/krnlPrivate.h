/* $Id: krnlPrivate.h,v 1.1 2001-09-17 00:12:09 bird Exp $
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

extern struct OTE               KKL_ObjTab[4];                  /* calltaba.asm */
extern struct OTE               KKL_ObjTab_DosCalls[20];        /* calltaba.asm */
extern char                     KKL_EntryTab[1];                /* calltaba.asm */
extern char DATA16_INIT         KKL_EntryTabFixups[1];          /* calltaba.asm */
#ifdef _OS2KLDR_H_
extern MTE                      kKrnlLibMTE;                    /* calltaba.asm */
#endif
extern char                     callTab[1];                     /* calltaba.asm */
extern char                     callTab16[1];                   /* calltaba.asm */
extern unsigned                 auNopFuncs[NBR_OF_KRNLIMPORTS]; /* calltaba.asm */


/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
extern void _Optlink LockedWrite(unsigned long ulAddr, unsigned char chOpcode, unsigned long ulDword);


#endif
