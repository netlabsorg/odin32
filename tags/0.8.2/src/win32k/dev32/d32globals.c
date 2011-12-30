/* $Id: d32globals.c,v 1.6 2001-02-21 07:44:57 bird Exp $
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
PULONG          pulTKSSBase32;

