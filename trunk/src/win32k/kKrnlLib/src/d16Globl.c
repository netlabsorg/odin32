/* $Id: d16Globl.c,v 1.1 2002-03-10 02:45:52 bird Exp $
 *
 * d16globals - global data (16-bit)
 *
 * Copyright (c) 1999-2001 knut st. osmundsen
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
ULONG       TKSSBase16  = 0;
USHORT      R0FlatCS16  = 0;
USHORT      R0FlatDS16  = 0;
ULONG       linDT = 0;                  /* dostable */
ULONG       linDT2 = 0;                 /* dostable2 */
BOOL        fInitTime   = TRUE;
struct kKLOptions
            DATA16_GLOBAL options = DEFAULT_OPTION_ASSIGMENTS;

CHAR        szBuildDate[] = {__DATE__};
CHAR        szBuildTime[] = {__TIME__};

