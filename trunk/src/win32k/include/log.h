/* $Id: log.h,v 1.1 1999-09-06 02:19:58 bird Exp $
 * log - C-style logging - kprintf.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */

#ifndef _log_h_
#define _log_h_

#ifdef __cplusplus
extern "C" {
#endif

#define OUTPUT_COM1 0x3f8
#define OUTPUT_COM2 0x2f8

#ifndef RELEASE
    #ifndef RING0_DEBUG_IN_RING3
        void _kprintf(const char *, ...);
        #define kprintf(a)          _kprintf a
        #define kernel_printf(a)    kprintf a; _kprintf("\n")
    #else
        #include <stdio.h>
        #define kprintf(a)          printf a; flushall()
        #define kernel_printf(a)    printf a; printf("\n"); flushall()
    #endif
#else
    #ifndef RING0_DEBUG_IN_RING3
        #define kprintf(a)          Yield()
        #define kernel_printf(a)    Yield()
    #else
        #include <stdio.h>
        #define kprintf(a)          (void)0
        #define kernel_printf(a)    (void)0
    #endif
#endif

#ifdef __cplusplus
}
#endif
#endif

