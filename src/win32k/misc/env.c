/* $Id: env.c,v 1.1 2000-04-17 01:56:50 bird Exp $
 *
 * Enviroment access functions
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "dev32.h"
#include "dev32hlp.h"
#include <string.h>

#include "env.h"
#include <infoseg.h>                    /* Infosegments definitions. */


/**
 * Scans the given environment data for a given enviroment variable and returns
 * its value if found.
 * @returns   Pointer to environment variable value for the variable given in
 *            pszVar.
 *            If the variable wasn't found NULL is returned.
 * @param     paszEnv   Pointer to the environment data to search.
 *                      The environment data is a list of zero-strings terminated
 *                      by an empty string. The strings are paired, that means
 *                      that the first string is the variable name and the
 *                      following string is the value of the variable.
 *                      AFAIK a variable can't have an empty value string!
 * @param     pszVar    Name of the environment variable to find.
 */
const char *ScanEnv(const char *paszEnv, const char *pszVar)
{
    /*
     * Loop thru the environment data until an empty string is reached.
     */
    while (*paszEnv != '\0')
    {
        register int i;                 /* Variable use to store the compare result. */

        /*
         * Variable name.
         * Check if it's matching the name we're searching for and skip the variable name.
         */
        i = stricmp(paszEnv, pszVar);
        paszEnv += strlen(paszEnv) + 1;
        if (i == 0)
        {   /* Variable was found. Return pointer to the value. */
            return paszEnv;
        }

        /*
         * !Paranoia!
         * If no value string we'll quit. This may be an IPE, if not it might
         * cause one if we continue processing the environment data.
         */
        if (*paszEnv == '\0')
            break;

        /* Skip value */
        paszEnv += strlen(paszEnv) + 1;
    }

    return NULL;
}


/**
 * Get the linear pointer to the environment data.
 * @returns   Pointer to environment data.
 *            NULL on failure.
 */
const char *GetEnv()
{
    /*  There is probably two ways of getting the environment data for a the
     *  current process: 1) get it from the PTDA->ptda_environ
     *                   2) Local infosegment (LIS from GetDosVar devhlp)
     *  I am not sure which one of these works best. What I know is that
     *  method 1) is used by the w_GetEnv API worker. This API is called at
     *  Ring-0 from some delete file operation. (Which uses it to get the
     *  DELETEDIR environment variable.) The w_GetEnv API worker is 16-bit
     *  I don't want to thunk around using that. There for I'll implement
     *  my own GetEnv. So, currently I'll write the code for both 1) and
     *  2), testing will show which one of them are most handy.
     */

    #if 0
    PPTDA   pPTDA;                      /* Current PTDA */
    PPTDA   pPTDAExecChild;             /* Current PTDAs pPTDAExecChild */
    USHORT  hobEnviron;                 /* Object handle of the environ block */
    APIRET  rc;                         /* Return from VMObjHandleInfo. */
    USHORT  ushPTDA;                    /* Handle of the context PTDA. (VMObjH..) */
    ULONG   ulAddr = 0;                 /* Address of the environment data.  */

    /*
     *  Use PTDA (1):
     *  Get the current PTDA. (Fail if this call failes.)
     *  IF pPTDAExecChild isn't NULL THEN try get environment for that first.
     *  IF failed or no pPTDAExecChild THEN try get enviroment from pPTDA.
     */
    pPTDA = ptdaGetCur();
    if (pPTDA == NULL)
    {
        kprintf(("GetEnv: Failed to get current PTDA.\n"));
    }
    pPTDAExecChild = ptdaGetpPTDAExecChild(pPTDA);
    if (pPTDAExecChild != NULL)
    {
        hobEnviron = ptdaGetptda_environ(pPTDAExecChild);
        if (hobEnviron != 0)
        {
            rc = VMObjHandleInfo(hobEnviron, &ulAddr, &ushPTDA);
            if (rc != NO_ERROR)
                ulAddr = 0;
        }
    }

    if (ulAddr == 0) /* failed or non pPTDAExecChild */
    {
        hobEnviron = ptdaGetptda_environ(pPTDA);
        if (hobEnviron != 0)
        {
            rc = VMObjHandleInfo(hobEnviron, &ulAddr, &ushPTDA);
            if (rc != NO_ERROR)
                ulAddr = 0;
        }
    }

    return (const char *)ulAddr;
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
        return NULL
    }

    if (pLIS->LIS_AX <= 3)
    {
        kprintf(("GetEnv: enviroment selector is %d, ie. NULL\n", pLIS->LIS_AX));
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
