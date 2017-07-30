/* $Id: myldrWasLoadModuled.cpp,v 1.2 2001-02-10 11:11:47 bird Exp $
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

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_PTDA
#define INCL_OS2KRNL_LDR

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
    return ldrWasLoadModuled(hmte, pptda, pcUsage);
}

