/* $Id: d32globals.c,v 1.4 2000-02-21 15:59:20 bird Exp $
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
#ifdef DEBUG
char szStartOfOptions[] = "START-OF-OPTIONS";
#endif
struct options  options = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
#ifdef DEBUG
char szEndOfOptions[] = "END-OF-OPTIONS";
#endif

