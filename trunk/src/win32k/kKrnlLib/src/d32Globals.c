/* $Id: d32Globals.c,v 1.1 2002-03-10 02:45:54 bird Exp $
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

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*
 * This is the pointer to the TK(TasK)SS(Stack Segment) FLAT Base address variable.
 * It is used by the SSToDS macro to convert stack based addresses to FLAT ds
 * based addressed.
 */
PULONG          pulTKSSBase32 = 0;

