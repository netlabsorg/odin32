/* $Id: log.h,v 1.4 2000-02-25 18:15:04 bird Exp $
 *
 * log - C-style logging - kprintf.
 * Dual mode, RING0 and RING3.
 * Dual 16 and 32 bit.
 *
 * Define NOLOGGING to disable logging for the given sourcefile or the entire system.
 *
 * Copyright (c) 1998-2000 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _log_h_
#define _log_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * COM-Port port numbers.
 */
#define OUTPUT_COM1 0x3f8
#define OUTPUT_COM2 0x2f8


/*
 * output macros
 */
#define dprintf kprintf
#if defined(DEBUG) && !defined(NOLOGGING)
    #ifndef INCL_16
        /* 32-bit */
        #include <stdarg.h>
        #include "vprintf.h"
        #define kprintf(a)          printf a
    #else
        /* 16-bit */
        #include "vprntf16.h"
        #define kprintf(a)          printf16 a
    #endif
#else
    #define kprintf(a)              (void)0
#endif

#ifdef __cplusplus
}
#endif
#endif

