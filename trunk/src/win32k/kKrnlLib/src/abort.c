/* $Id: abort.c,v 1.1 2002-03-10 02:45:51 bird Exp $
 *
 * Abort replacement.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: abort.c,v 1.1 2002-03-10 02:45:51 bird Exp $";
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "dev32.h"
#include "kTypes.h"
#include "kLog.h"
#include "kKLasmutils.h"

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
    Int3();
    KLOGEXITVOID();
}
