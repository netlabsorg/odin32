/* $Id: myldrWasLoadModuled.cpp,v 1.1.2.2 2002-04-01 09:06:10 bird Exp $
 *
 * ldrWasLoadModuled - Tells OS/2 that the executable module was LoadModuled
 *      too. This way DosQueryProcAddr and DosQueryProcType will work for
 *      executables too.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: myldrWasLoadModuled.cpp,v 1.1.2.2 2002-04-01 09:06:10 bird Exp $";
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_PTDA
#define INCL_OS2KRNL_LDR
#define INCL_KKL_LOG
#define INCL_KKL_AVL

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "NEexe.h"                      /* Wine NE structs and definitions. */
#include "LXexe.h"                      /* OS/2 LX structs and definitions. */
#include "PEexe.h"                      /* Wine PE structs and definitions. */
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "dev32.h"
#include "ldr.h"
#include "options.h"


/**
 * Checks if a module was loaded using DosLoadModule.
 * This is called from LDRGetProcAddr and LDRFreeModule.
 *
 * We would like to get entry points from executables (EXEs) too.
 * So, when called from LDRGetProcAddr we'll tell OS/2 a white lie and
 * say that the executable module for the given process was LoadModuled.
 *
 * @returns NO_ERROR if the module was LoadModuled or executable.
 *          ERROR_INVALID_HANDLE if not LoadModuled.
 * @param   hmte    MTE handle.
 * @param   pptda   Pointer to the PTDA of the process calling. (current)
 * @param   pcUsage Pointer to usage variable. (output)
 *                  The usage count is returned.
 * @sketch  Check if enabled.
 *          If      called from LDRGetProcAddr
 *              AND hmte = hmteEXE
 *          Then return NO_ERROR.
 *          return thru ldrWasLoadModuled.
 */
ULONG LDRCALL myldrWasLoadModuled(HMTE hmte, PPTDA pptda, PULONG pcUsage)
{
    KLOGENTRY3("ULONG","HMTE hmte, PPTDA pptda, PULONG pcUsage", hmte, pptda, pcUsage);
    ULONG   rc;

    /*
     * Check if the fix is enabled.
     */
    if (isExeFixesEnabled())
    {
        /*
         * If pcUsage is NULL we're called from LDRGetProcAddr.
         */
        if (    pcUsage == NULL
            &&  hmte == ptdaGet_ptda_module(pptda)
            )
        {
            kprintf(("myldrWasLoadModuled: Executable hmte=%04x\n", hmte));
            return NO_ERROR;
        }
    }

    /*
     * Let the real function do the work.
     */
    rc = ldrWasLoadModuled(hmte, pptda, pcUsage);
    KLOGEXIT(rc);
    return rc;
}

