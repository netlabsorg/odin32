/* $Id: d16globl.c,v 1.1.2.2 2002-04-01 09:57:10 bird Exp $
 *
 * d16globals - global data (16-bit)
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
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


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/

/* Note: All global variables must be initialized!  *
 *       Uninitialized variables ends up in DATA32. */
PFN         Device_Help = NULL;
BOOL        fInitTime   = TRUE;
struct Win32kOptions
            DATA16_GLOBAL options = DEFAULT_OPTION_ASSIGMENTS;

CHAR        szBuildDate[] = {__DATE__};
CHAR        szBuildTime[] = {__TIME__};

