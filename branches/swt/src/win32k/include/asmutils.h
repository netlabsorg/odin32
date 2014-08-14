/* $Id: asmutils.h,v 1.4 2000-01-24 18:18:59 bird Exp $
 *
 * Assembly utilities.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _asmutils_h_
#define _asmutils_h_

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned short _System GetCS(void);
extern unsigned short _System GetDS(void);
extern unsigned short _System GetES(void);
extern unsigned short _System GetFS(void);
extern unsigned short _System GetGS(void);
extern unsigned short _System GetSS(void);

extern void _System DisableInterrupts(void); /* uniprocessor only */
extern void _System EnableInterrupts(void);  /* uniprocessor only */

#if 0
    //extern void _System Int3(void);
    extern int _System Int3(void);
#else
#ifdef __GNUC__
    #define _Inline static inline
    #define __interrupt(n) ({ __asm__ __volatile__ ("int" #n "\n\tnop"); })
#else
    #include <builtin.h>
#endif
    #define Int3() __interrupt(3)
#endif

//Negative offsets don't work yet?
extern void __stdcall memmov(void *p, signed int off, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif

