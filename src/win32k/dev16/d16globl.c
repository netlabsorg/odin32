/* $Id: d16globl.c,v 1.3 2001-01-20 23:53:27 bird Exp $
 *
 * d16globals - global data (16-bit)
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_NOPMAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"
#include "options.h"
#include "log.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/

/* Note: All global variables must be initialized!  *
 *       Uninitialized variables ends up in DATA32. */
PFN         Device_Help = NULL;
ULONG       TKSSBase16  = 0;
USHORT      R0FlatCS16  = 0;
USHORT      R0FlatDS16  = 0;
BOOL        fInitTime   = TRUE;
struct options
            DATA16_GLOBAL options = DEFAULT_OPTION_ASSIGMENTS;

CHAR        szBuildDate[] = {__DATE__};
CHAR        szBuildTime[] = {__TIME__};
CHAR        szWin32kIni[CCHMAXPATH] = {"\win32k.ini"};

