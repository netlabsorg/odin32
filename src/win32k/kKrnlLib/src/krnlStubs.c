/* $Id: krnlStubs.c,v 1.1 2002-03-10 02:45:56 bird Exp $
 *
 * krnlStubs - Stubs for not-required (NR) imported functions and variables.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: krnlStubs.c,v 1.1 2002-03-10 02:45:56 bird Exp $";
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#include <os2.h>

#define INCL_OS2KRNL_ALL
#define INCL_OS2KRNL_LDR
#define LDR_INCL_INITONLY
#include "OS2Krnl.h"

#define INCL_KKL_LOG
#include "kKrnlLib.h"


PSZ SECCALL     nopSecPathFromSFN(SFN hFile);


/**
 * Dummy nop function if SecPathFromSFN isn't found.
 */
PSZ SECCALL nopSecPathFromSFN(SFN hFile)
{
    KLOGENTRY1("PSZ SECCALL","SFN hFile", hFile);
    KNOREF(hFile);
    KLOGEXIT(NULL);
    return NULL;
}

