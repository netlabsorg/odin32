/* $Id: asmutils.h,v 1.4.6.1 2002-04-01 10:01:01 bird Exp $
 *
 * Assembly utilities.
 *
 * Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
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
    #include <builtin.h>
    #define Int3() __interrupt(3)
#endif

//Negative offsets don't work yet?
extern void __stdcall memmov(void *p, signed int off, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif

