/* $Id: initterm.h,v 1.5 2000-06-21 20:51:52 sandervl Exp $ */
/*
 * KERNEL32 DLL entry point header
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef __INITTERM_H__
#define __INITTERM_H__

#ifndef PAG_ANY
    #define PAG_ANY    0x00000400
#endif

#ifndef QSV_VIRTUALADDRESSLIMIT
    #define QSV_VIRTUALADDRESSLIMIT 30
#endif

#ifndef QSV_NUMPROCESSORS
    #define QSV_NUMPROCESSORS 26
#endif

void CloseLogFile(); //misc.cpp

extern int globLoadNr; //global data
extern int loadNr;

/* Tue 03.03.1998: knut */

extern ULONG flAllocMem; /* flag to optimize DosAllocMem to use all the memory on SMP machines */
extern ULONG ulMaxAddr; /* end of user address space */
extern char  kernel32Path[];

void InitEnvironment(int nrcpus); //initsystem.cpp

#endif
