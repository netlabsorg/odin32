/* $Id: cio.h,v 1.5 2000-09-20 21:32:54 hugh Exp $ */

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

int io_init1(void);
int io_init2(short);
int io_exit1(void);

char c_inb(short);
short c_inw(short);
long c_inl(short);
void c_outb(short,char);
void c_outw(short,short);
void c_outl(short,long);
//void CDECL  c_readmsr(long, long *);

#ifdef __cplusplus
}
#endif

#endif
