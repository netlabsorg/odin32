/* $Id: abort.c,v 1.3 2002-12-16 02:24:27 bird Exp $
 *
 * Abort replacement.
 *
 * Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>
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
static const char szFileId[] = "$Id: abort.c,v 1.3 2002-12-16 02:24:27 bird Exp $";
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <kLib/kTypes.h>
#include <kLib/kLog.h>
#include <kLib/kDevHlp.h>


/**
 * Abort substitute.
 * Pure virtual fallback function jump to abort.
 * We'll set up an IPE later, currently we'll do a breakpoint.
 */
void abort(void)
{
    KLOGENTRY0("void");
    kprintf(("!Internal Processing Error! should not be here! - abort!\n"));
    kBreakPoint();
    KLOGEXITVOID();
}

