/* $Id: yield.h,v 1.1 1999-09-06 02:20:00 bird Exp $
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

void Yield(void);

#ifdef __cplusplus
}
#endif

#endif

