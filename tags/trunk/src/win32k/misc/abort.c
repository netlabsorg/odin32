/* $Id: abort.c,v 1.2 2000-12-11 06:41:19 bird Exp $
 *
 * Abort replacement.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#if defined(RING0) || defined(RING3)
    #include "dev32.h"
#else
    #define SSToDS(a) (a)
#endif
#include "string.h"
#include "log.h"

#include <builtin.h>
#include <asmutils.h>
#define assert(a) ((a) ? (void)0 : __interrupt(3))




/**
 * Abort substitute.
 * Pure virtual fallback function jump to abort.
 * We'll set up an IPE later, currently we'll do a breakpoint.
 */
void abort(void)
{
    kprintf(("abort() !Internal Processing Error! abort()\n"));
    Int3();
}
