/* $Id: abort.c,v 1.2 2002-03-31 19:01:12 bird Exp $
 *
 * Abort replacement.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: abort.c,v 1.2 2002-03-31 19:01:12 bird Exp $";
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "dev32.h"
#include <kLib/kTypes.h>
#include <kLib/kLog.h>

#include <builtin.h>


/**
 * Abort substitute.
 * Pure virtual fallback function jump to abort.
 * We'll set up an IPE later, currently we'll do a breakpoint.
 */
void abort(void)
{
    KLOGENTRY0("void");
    kprintf(("!Internal Processing Error! should not be here! - abort!\n"));
    INT3();
    KLOGEXITVOID();
}
