/* $Id: log.h,v 1.2 1999-10-14 01:16:50 bird Exp $
 *
 * log - C-style logging - kprintf.
 * Dual mode, RING0 and RING3.
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

/*
 * output macros
 */
#define dprintf kprintf
#ifdef DEBUG
    #include <stdarg.h>
    #include "vprintf.h"
    #define kprintf(a)          printf a
    #define kernel_printf(a)    printf a, printf("\n") /* obsolete */
#else
    #define kprintf(a)          (void)0
    #define kernel_printf(a)    (void)0
#endif

#ifdef __cplusplus
}
#endif
#endif

