/* $Id: kKLasmutils.h,v 1.3 2001-10-16 02:21:05 bird Exp $
 *
 * Assembly utilities.
 *
 * Copyright (c) 1998-2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _asmutils_h_
#define _asmutils_h_

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned short _System   GetCS(void);
extern unsigned short _System   GetDS(void);
extern unsigned short _System   GetES(void);
extern unsigned short _System   GetFS(void);
extern unsigned short _System   GetGS(void);
extern unsigned short _System   GetSS(void);

extern void _System             DisableInterrupts(void);
extern void _System             EnableInterrupts(void);

extern unsigned long _Optlink   AtomicInc(unsigned long *pul);
extern unsigned long _Optlink   AtomicIncWrap(unsigned long *pul, unsigned long ulMax);
extern unsigned long _Optlink   AtomicDec(unsigned long *pul);
extern unsigned long _Optlink   AtomicSet(unsigned long *pul, unsigned long ul);
extern unsigned long _Optlink   AtomicSetEqual(unsigned long *pul, unsigned long ulSet, unsigned long ulCurrent);

#if 0
    //extern void _System Int3(void);
    extern int _System Int3(void);
#else
    #include <builtin.h>
    #define Int3() __interrupt(3)
#endif

//Negative offsets don't work yet?
extern void __stdcall memmov(void *p, signed int off, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif

