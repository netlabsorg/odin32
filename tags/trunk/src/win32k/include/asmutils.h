/* $Id: asmutils.h,v 1.3 1999-11-10 01:45:32 bird Exp $
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

//extern void _System Int3(void);
extern int _System Int3(void);

//Negative offsets don't work yet?
extern void __stdcall memmov(void *p, signed int off, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif

