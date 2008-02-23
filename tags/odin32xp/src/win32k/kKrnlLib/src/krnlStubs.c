/* $Id: krnlStubs.c,v 1.2 2002-12-19 01:49:09 bird Exp $
 *
 * krnlStubs - Stubs for not-required (NR) imported functions and variables.
 *
 * Copyright (c) 2000-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NOFILEID
static const char szFileId[] = "$Id: krnlStubs.c,v 1.2 2002-12-19 01:49:09 bird Exp $";
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_OS2KRNL_ALL
#define INCL_OS2KRNL_LDR
#define LDR_INCL_INITONLY
#include "OS2Krnl.h"

#define INCL_KKL_LOG
#include "kKrnlLib.h"


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

