/* $Id: yield.h,v 1.2 1999-10-31 23:57:05 bird Exp $
 *
 * Yield - conversion may take some time. So it is necessary to
 *         check it's time to yield the processor to other processes.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */
#ifndef _yield_h_
#define _yield_h_

#ifdef __cplusplus
extern "C" {
#endif


#ifdef RING0
    BOOL Yield(void);
#else
    #define Yield() FALSE
#endif


#ifdef __cplusplus
}
#endif

#endif

