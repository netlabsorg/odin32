/* $Id: kHll.cpp,v 1.1 2000-03-25 21:09:59 bird Exp $
 *
 * kHll - Implementation of the class kHll.
 *        That class is used to create HLL debuginfo.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_TYPES
#define INCL_DOSERRORS
#define FOR_EXEHDR          1           /* exe386.h flag */
#define DWORD               ULONG       /* Used by exe386.h / newexe.h */
#define WORD                USHORT      /* Used by exe386.h / newexe.h */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include <newexe.h>
#include <exe386.h>

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "hll.h"
#include "kHll.h"




