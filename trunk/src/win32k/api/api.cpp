/* $Id: api.cpp,v 1.1 2001-01-20 15:47:33 bird Exp $
 *
 * API Overload Init and Helper Function.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "api.h"


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _MaskArray
{
    int         cMasks;
    PSZ *       papszMasks;
} MASKARRAY, *PMASKARRAY;

typedef struct _ApiDataEntry
{
    MASKARRAY   ProcessInc;
    MASKARRAY   ProcessExc;
    MASKARRAY   ModuleInc;
    MASKARRAY   ModuleExc;
} APIDATAENTRY, *PAPIDATAENTRY;


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
APIDATAENTRY    aApiData[API_MAX];


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
int     apiReadIniFile(PSZ pszIniFile);







int     apiReadIniFile(PSZ pszIniFile)
{

}


