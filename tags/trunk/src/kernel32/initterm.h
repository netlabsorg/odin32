/* $Id: initterm.h,v 1.2 1999-10-23 12:34:47 sandervl Exp $ */
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


void CloseLogFile(); //misc.cpp

extern int globLoadNr; //global data
extern int loadNr;

/* Tue 03.03.1998: knut */
/* flag to optimize DosAllocMem to use all the memory on SMP machines */
extern ULONG flAllocMem;
extern char  kernel32Path[];

#endif
