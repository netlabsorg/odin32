/* $Id: macros.h,v 1.3 2000-09-02 21:08:02 bird Exp $
 *
 * Common macros for Win32k/Elf.
 *
 * Copyright (c) 2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _MACROS_H_
#define _MACROS_H_


#define MAX(a, b)           (((a) > (b))? (a) : (b))
#define MIN(a, b)           (((a) < (b))? (a) : (b))
#define MINNOTNULL(a, b)    ((a) != NULL && (b) != NULL ? MIN(a,b) : ((a) != NULL ? (a) : (b)))
#define MAXNOTNULL(a, b)    ((a) != NULL && (b) != NULL ? MAX(a,b) : ((a) != NULL ? (a) : (b)))

/*
 * Aligns something, a, up to nearest alignment boundrary-
 * Note: Aligment must be a 2**n number.
 */
#define ALIGN(a, alignment) (((a) + (alignment - 1UL)) & ~(alignment - 1UL))

/*
 * Not referenced parameter warning fix.
 */
#define NOREF(a) (a=a)

/*
 * Makes an unsigned long from 4 (unsigned or signed) chars.
 */
#define MAKEULONG4(ch1, ch2, ch3, ch4) \
            ((ULONG)(MAKEULONG(MAKEUSHORT(ch1, ch2), MAKEUSHORT(ch3, ch4))))

#endif
