/* $Id: cio.h,v 1.1 1999-05-24 20:19:43 ktk Exp $ */

/* Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de) */
/* Use at your own risk! No Warranty! The author is not responsible for
 * any damage or loss of data caused by proper or improper use of this
 * device driver.
 */

#ifndef _CIO_H_
#define _CIO_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int CDECL io_init(void);
extern int CDECL io_exit(void);

extern int CDECL io_init1(void);
extern int CDECL io_exit1(void);

extern char CDECL  c_inb(short);
extern short CDECL c_inw(short);
extern long CDECL  c_inl(short);
extern void CDECL  c_outb(short,char);
extern void CDECL  c_outw(short,short);
extern void CDECL  c_outl(short,long);
extern void CDECL  c_readmsr(long, long *);

#ifdef __cplusplus
}
#endif

#endif
