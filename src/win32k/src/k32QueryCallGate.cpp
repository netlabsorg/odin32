/* $Id: k32QueryCallGate.cpp,v 1.1.2.3 2002-04-01 09:57:14 bird Exp $
 *
 * k32QueryCallGate - Query the callgate selector for the callgate to the k32 APIs.
 *
 * Copyright (c) 2001-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: k32QueryCallGate.cpp,v 1.1.2.3 2002-04-01 09:57:14 bird Exp $";
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS
#define INCL_OS2KRNL_TK
#define INCL_KKL_LOG

#define NO_WIN32K_LIB_FUNCTIONS



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "win32k.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"



/**
 * Query the callgate selector for the callgate to the k32 APIs.
 * @returns NO_ERROR on success.
 *          Appropriate error code on failure.
 * @param   pusCGSelector   Pointer to the variable which should have the CallGate
 *                          assigned.
 *                          User memory!!!
 * @sketch  Check if the CallGate is no NULL. Fail if so.
 *          Try copy the CallGate selecto to the passed in variable pointer.
 *          return the result of this attempt.
 * @status  Completely implelemnted.
 * @author  knut st. osmundsen (bird@anduin.net)
 */
APIRET k32QueryCallGate(PUSHORT pusCGSelector)
{
    KLOGENTRY1("APIRET","PUSHORT pusCGSelector", pusCGSelector);
    APIRET  rc;

    if (CallGateGDT > 7)
        rc = TKSuULongNF(pusCGSelector, &CallGateGDT);
    else
        rc = ERROR_INVALID_CALLGATE;

    KLOGEXIT(rc);
    return rc;
}

