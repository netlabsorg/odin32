/* $Id: d32Globals.c,v 1.3 2002-04-01 12:48:26 bird Exp $
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
#include <odinbuild.h>

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*
 * This is the pointer to the TK(TasK)SS(Stack Segment) FLAT Base address variable.
 * It is used by the SSToDS macro to convert stack based addresses to FLAT ds
 * based addressed.
 */
PULONG          pulTKSSBase32 = 0;
ULONG           ulkKrnlLibBuild = ODIN32_BUILD_NR;
