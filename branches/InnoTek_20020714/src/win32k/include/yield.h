/* $Id: yield.h,v 1.3 1999-11-10 01:45:34 bird Exp $
 *
 * Yield - conversion may take some time. So it is necessary to
 *         check it's time to yield the processor to other processes.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
    #define Yield() (DosSleep(0) == NO_ERROR)
#endif


#ifdef __cplusplus
}
#endif

#endif

