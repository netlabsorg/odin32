/* $Id: env.c,v 1.4 2000-12-11 06:53:54 bird Exp $
 *
 * Environment access functions
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS                  /* Error codes */
#define INCL_OS2KRNL_VM                 /* OS2KRNL: Virtual Memory Management */
#define INCL_OS2KRNL_PTDA               /* OS2KNRL: (per)ProcessTaskDataArea */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "dev32.h"
#include "dev32hlp.h"
#include "log.h"
#include "OS2Krnl.h"
#include <string.h>
#include "macros.h"
#include "env.h"


/**
 * Scans the given environment data for a given environment variable and returns
 * its value if found.
 * @returns   Pointer to environment variable value for the variable given in
 *            pszVar.
 *            If the variable wasn't found NULL is returned.
 * @param     paszEnv   Pointer to the environment data to search.
 *                      The environment data is a list of zero-strings terminated
 *                      by an empty string. The strings consists of two parts which
 *                      are separated by a euqal char ('='). The first part is the
 *                      variable name. The second part is the value of the variable.
 *
 *                      IF this is NULL we'll simply return NULL.
 * @param     pszVar    Name of the environment variable to find. (NULL not allowed.)
 */
const char *ScanEnv(const char *paszEnv, const char *pszVar)
{
    int     cchVar;
    /*
     * Return if environment not found.
     */
    #ifdef DEBUG
    if (pszVar < (const char *)0x10000 || *pszVar == '\0')
        kprintf(("ScanEnv: Invalid parameter pszVar (%p)\n", pszVar));
    #endif
    if (paszEnv == NULL)
        return NULL;
    #ifdef DEBUG
    if (paszEnv < (const char *)0x10000)
        kprintf(("ScanEnv: Invalid parameter paszEnv (%p)\n", paszEnv));
    #endif

    /*
     * Loop thru the environment data until an empty string is reached.
     */
    cchVar = strlen(pszVar);
    while (*paszEnv != '\0')
    {
        /*
         * Check if the variable name is it's matching the one we're searching for.
         */
        const char *pszEqual = strchr(paszEnv, '=');
        if (pszEqual != NULL && (pszEqual - paszEnv) == cchVar
            && memcmp(paszEnv, pszVar, cchVar) == 0
            )
        {
            /*
             * Variable was found. Return pointer to the value.
             */
            return pszEqual + 1;
        }

        /*
         * Skip this variable. (Don't use pszEqual since it might be NULL)
         */
        paszEnv += strlen(paszEnv) + 1;
    }

    return NULL;
}


/**
 * Get the linear pointer to the environment data for the current
 * process or the process being started (EXECed).
 *
 * @param       fExecChild      TRUE:  Get exec child environment.
 *                                     (Not supported by method 2)
 *                              FALSE: Get current process environment.
 * @returns     Pointer to environment data.
 *              NULL on failure.
 */
const char *GetEnv(BOOL fExecChild)
{
    /*  There are probably two ways of getting the environment data for the
     *  current process: 1) get it from the PTDA->ptda_environ
     *                   2) Local infosegment (LIS from GetDosVar devhlp)
     *  I am not sure which one of these which works best. What I know is that
     *  method 1) is used by the w_GetEnv API worker. This API is called at
     *  Ring-0 from some delete file operation. (Which uses it to get the
     *  DELETEDIR environment variable.) The w_GetEnv API worker is 16-bit
     *  I don't want to thunk around using that. There for I'll implement
     *  my own GetEnv. So, currently I'll write the code for both 1) and
     *  2), testing will show which one of them are the better.
     */

    #if 1
    PPTDA   pPTDACur;                   /* Pointer to the current (system context) PTDA */
    PPTDA   pPTDA;                      /* PTDA in question. */
    USHORT  hobEnviron;                 /* Object handle of the environ block */
    APIRET  rc;                         /* Return from VMObjHandleInfo. */
    USHORT  ushPTDA;                    /* Handle of the context PTDA. (VMObjH..) */
    ULONG   ulAddr = 0;                 /* Address of the environment data.  */

    /*
     *  Use PTDA (1):
     *  Get the current PTDA. (Fail if this call failes.)
     *  IF pPTDAExecChild isn't NULL THEN try get environment for that first.
     *  IF failed or no pPTDAExecChild THEN try get environment from pPTDA.
     */
    pPTDACur = ptdaGetCur();
    if (pPTDACur != NULL)
    {
        pPTDA = ptdaGet_pPTDAExecChild(pPTDACur);
        if (pPTDA != NULL && fExecChild)
        {
            hobEnviron = ptdaGet_ptda_environ(pPTDA);
            if (hobEnviron != 0)
            {
                rc = VMObjHandleInfo(hobEnviron, SSToDS(&ulAddr), SSToDS(&ushPTDA));
                if (rc == NO_ERROR)
                    return (const char *)ulAddr;
                kprintf(("GetEnv: (1) VMObjHandleInfo failed with rc=%d for hob=0x%04x \n", rc, hobEnviron));
            }
        }

        hobEnviron = ptdaGet_ptda_environ(pPTDACur);
        if (hobEnviron != 0)
        {
            rc = VMObjHandleInfo(hobEnviron, SSToDS(&ulAddr), SSToDS(&ushPTDA));
            if (rc == NO_ERROR)
                return (const char *)ulAddr;
            kprintf(("GetEnv: (2) VMObjHandleInfo failed with rc=%d for hob=0x%04x\n", rc, hobEnviron));
        }
    }
    else
    {   /* Not called at task time? No current task! */
        kprintf(("GetEnv: Failed to get current PTDA.\n"));
    }

    return NULL;


    #else


    struct InfoSegLDT * pLIS;           /* Pointer to local infosegment. */
    PVOID               pv;             /* Address to return. */


    /*
     *  Use LocalInfoSegment (2)
     *  Get the LIS using Dh32_GetDosVar
     */
    pLIS = (struct InfoSegLDT*)D32Hlp_GetDOSVar(DHGETDOSV_LOCINFOSEG, 0);
    if (pLIS == NULL)
    {
        kprintf(("GetEnv: Failed to get local info segment\n"));
        NOREF(fExecChild);
        return NULL;
    }

    if (pLIS->LIS_AX <= 3)
    {
        kprintf(("GetEnv: environment selector is %d, ie. NULL\n", pLIS->LIS_AX));
        return NULL;
    }

    pv = D32Hlp_VirtToLin2(pLIS->LIS_AX, 0);
    if (pv != NULL)
    {
        kprintf(("GetEnv: VirtToLin2 failed to thunk %04x:0000 to linar address\n", pLIS->LIS_AX));
    }
    return (const char *)pv;
    #endif
}
