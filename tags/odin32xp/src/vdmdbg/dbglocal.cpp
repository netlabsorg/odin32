/* $Id: dbglocal.cpp,v 1.1 2001-06-13 04:45:32 bird Exp $
 *
 * debug logging functions for OS/2
 *
 * Copyright 2000 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2wrap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dbglocal.h"

#ifdef DEBUG

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
USHORT DbgEnabled[DBG_MAXFILES] = {0};
USHORT DbgEnabledLvl2[DBG_MAXFILES] = {0};

char  *DbgFileNames[DBG_MAXFILES] =
{
    "vdmdbg",
};

/**
 * Parses the log env.vars and inits the DbgEnabled and DbgEnabledLvl2
 * arrays accordingly.
 */
void ParseLogStatus()
{
    char *pszEnvVar = getenv(DBG_ENVNAME);
    char *pszEnvVar2= getenv(DBG_ENVNAME_LVL2);
    char *pszDbgVar;
    int   i;

    for (i = 0; i < DBG_MAXFILES; i++)
    {
        DbgEnabled[i] = 1;
    }

    if (pszEnvVar)
    {
        pszDbgVar = strstr(pszEnvVar, "dll");
        if (pszDbgVar)
        {
            if (*(pszDbgVar-1) == '-')
            {
                for (i = 0; i < DBG_MAXFILES; i++)
                {
                    DbgEnabled[i] = 0;
                }
            }
        }
        for (i = 0; i < DBG_MAXFILES; i++)
        {
            pszDbgVar = strstr(pszEnvVar, DbgFileNames[i]);
            if (pszDbgVar)
            {
                if (*(pszDbgVar-1) == '-')
                {
                    DbgEnabled[i] = 0;
                }
                else if (*(pszDbgVar-1) == '+')
                {
                    DbgEnabled[i] = 1;
                }
            }
        }
    }
    if (pszEnvVar2)
    {
        pszDbgVar = strstr(pszEnvVar2, "dll");
        if (pszDbgVar)
        {
            if (*(pszDbgVar-1) == '+')
            {
                for (i = 0;i < DBG_MAXFILES; i++)
                {
                    DbgEnabledLvl2[i] = 1;
                }
            }
        }
        for (i = 0; i < DBG_MAXFILES; i++)
        {
            pszDbgVar = strstr(pszEnvVar2, DbgFileNames[i]);
            if (pszDbgVar)
            {
                if (*(pszDbgVar-1) == '-')
                {
                    DbgEnabledLvl2[i] = 0;
                }
                else if (*(pszDbgVar-1) == '+')
                {
                    DbgEnabledLvl2[i] = 1;
                }
            }
        }
    }
}

#endif
