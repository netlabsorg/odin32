/* $Id: d32globals.c,v 1.3 1999-11-10 01:45:30 bird Exp $
 *
 * d32globals - global data (32-bit)
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
#include "options.h"

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
PULONG          pulTKSSBase32;
struct options  options;

