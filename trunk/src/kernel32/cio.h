/* $Id: cio.h,v 1.6 2000-09-21 19:59:30 sandervl Exp $ */

/* Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de) */
/* Use at your own risk! No Warranty! The author is not responsible for
 * any damage or loss of data caused by proper or improper use of this
 * device driver.
 */

#ifndef _CIO_H_
#define _CIO_H_

#include <odin.h>

#ifdef __cplusplus
extern "C" {
#endif

int CDECL io_init(void);
int CDECL io_exit(void);

int CDECL io_init1(void);
int CDECL io_exit1(void);

char CDECL  c_inb(short);
short CDECL c_inw(short);
long CDECL  c_inl(short);
void CDECL  c_outb(short,char);
void CDECL  c_outw(short,short);
void CDECL  c_outl(short,long);
void CDECL  c_readmsr(long, long *);

#ifdef __cplusplus
}
#endif

#endif
