/* $Id: kKLlog.h,v 1.1 2001-09-14 01:50:17 bird Exp $
 *
 * log - C-style logging - kprintf.
 * Dual mode, RING0 and RING3.
 * Dual 16 and 32 bit.
 *
 * Define NOLOGGING to disable logging for the given sourcefile or the entire system.
 *
 * Copyright (c) 1998-2001 knut st. osmundsen
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
 * output macros
 */
#define dprintf kprintf
#if defined(DEBUG) && !defined(NOLOGGING)
    #ifdef INCL_16
        /* 16-bit */
        #include "vprntf16.h"
        #define kprintf(a)          printf16 a
        #define printf this function is not used in 16-bit code! Use printf16!
    #else
        /* 32-bit */
        #include <stdarg.h>
        #include "kKLprintf.h"
        #ifdef __cplusplus
            #define kprintf(a)      ::printf a
        #else
            #define kprintf(a)      printf a
        #endif
    #endif
#else
    #define kprintf(a)              ((void)0)
#endif

#ifdef __cplusplus
}
#endif
#endif

