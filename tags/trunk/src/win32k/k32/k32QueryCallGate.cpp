/* $Id: k32QueryCallGate.cpp,v 1.2 2001-02-21 15:42:11 bird Exp $
 *
 * k32QueryCallGate - Query the callgate selector for the callgate to the k32 APIs.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS
#define INCL_OS2KRNL_TK

#define NO_WIN32K_LIB_FUNCTIONS



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "OS2Krnl.h"
#include "win32k.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"
#include "log.h"
#include "macros.h"



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
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
APIRET k32QueryCallGate(PUSHORT pusCGSelector)
{
    APIRET  rc;

    /*
     * Callgate is not safte on SMP yet.
     */
    if (isSMPKernel())
        return ERROR_NOT_SUPPORTED;

    if (CallGateGDT > 7)
        rc = TKSuULongNF(pusCGSelector, &CallGateGDT);
    else
        rc = ERROR_INVALID_CALLGATE;

    return rc;
}

