/* $Id: myLDRGetProcAddr.cpp,v 1.1 2000-12-17 22:44:46 bird Exp $
 *
 * LDRGetProcAddr - Get an entry point to a module.
 *      We override this and allow querying entrypoints from executable too.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_PTDA

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "log.h"
#include "avl.h"
#include <peexe.h>
#include <exe386.h>
#include "OS2Krnl.h"
#include "dev32.h"
#include "ldr.h"
#include "ldrCalls.h"
#include "options.h"


/**
 * LDRGetProcAddr gets address and proctype for a entry point to a module.
 *
 * We would like to treat hmte == NULLHANDLE as the handle of the
 * executable module of the calling (current) process. So, we'll
 * have to correct it.
 *
 * @returns NO_ERROR if the module was LoadModuled or executable.
 *          ERROR_INVALID_HANDLE if not LoadModuled.
 * @param   hmte            Handle of module.
 * @param   ulOrdinal       Procedure ordinal.
 * @param   pszName         Pointer to procedure name.
 *                          NULL is allowed. Ignored if ulOrdinal is not zero.
 * @param   pulAddress      Pointer to address variable. (output)
 * @param   fFlat           TRUE if a flat 0:32 address is to be returned.
 *                          FALSE if a far 16:16 address is to be returned.
 * @param   pulProcType     Pointer to procedure type variable. (output)
 *                          NULL is allowed. (DosQueryProcAddr uses NULL)
 *                          In user space.
 * @sketch  Check if enabled.
 *          Correct hmte == NULLHANDLE to the EXE hmte for the current process.
 */
ULONG LDRCALL myLDRGetProcAddr(HMTE     hmte,
                               ULONG    ulOrdinal,
                               PCSZ     pszName,
                               PULONG   pulAddress,
                               BOOL     fFlat,
                               PULONG   pulProcType)
{
    /*
     * Check if the fix is enabled, and needed, and possible to apply.
     */
    if (    isExeFixesEnabled()
        &&  hmte == NULLHANDLE
        &&  ptdaGetCur() != NULL)
    {
        hmte = ptdaGet_ptda_module(ptdaGetCur());
        kprintf(("myLDRGetProcAddr: Set hmte to exe handle. hmte=%04x\n", hmte));
    }

    /*
     * Let the real function do rest of the work.
     */
    return LDRGetProcAddr(hmte, ulOrdinal, pszName, pulAddress, fFlat, pulProcType);
}

